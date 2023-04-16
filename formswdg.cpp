#include "formswdg.h"
#include "ui_formswdg.h"

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSettings>
#include <QTextDocument>

#include "database.h"
#include "formeddlg.h"
#include "printformdlg.h"

#include <QDebug>

FormsWdg::FormsWdg(Database* db, QWidget *parent)
    : QWidget(parent), ui(new Ui::FormsWdg)
{
    ui->setupUi(this);
    this->db = db;
    initPanel();

    connect(ui->tbEdit, SIGNAL(clicked()), this, SLOT(editForm()));
    connect(ui->tbAdd, SIGNAL(clicked()), this, SLOT(addForm()));
    connect(ui->tbDel, SIGNAL(clicked()), this, SLOT(deleteForm()));
    connect(ui->tbPrint, SIGNAL(clicked(bool)), this, SLOT(printForm()));
    readSettings();
}

FormsWdg::~FormsWdg()
{
    writeSettings();
    delete ui;
}

void FormsWdg::addForm()
{
    if (QMessageBox::question(this, "", tr("Add a new questionnaire?"))==QMessageBox::Yes)
    {
        int row = formModel->rowCount();
        formModel->insertRow(row);
        QModelIndex index = formModel->index(row, Form::Name);
        formModel->setData(index, tr("New questionnaire"));
        formModel->submitAll();
        int id = db->getMaxId(Form::TABLE);
        row = findRow(id); //Из-за сортировки по имени может измениться номер строки в tvForm
//        qDebug() << "id" << id << "row" << row;
        ui->tvForm->setCurrentIndex(index.sibling(row, Form::Name));
        QSqlRecord record = formModel->record(row);
        FormEdDlg* dlg = new FormEdDlg(record, db);
        dlg->exec();
        bool changes_formnamenote = false;
        if(QString::compare(record.value(Form::Name).toString(), dlg->getFormName())!=0)
        {
            record.setValue(Form::Name, dlg->getFormName());
            changes_formnamenote = true;
        }
        if(QString::compare(record.value(Form::Note).toString(), dlg->getFormNote())!=0)
        {
            record.setValue(Form::Note, dlg->getFormNote());
            changes_formnamenote = true;
        }
        delete dlg;
        if (changes_formnamenote)
            formModel->setRecord(row, record);
        formModel->submitAll();
        row = findRow(id); //Из-за сортировки по имени может измениться номер строки в tvForm
        ui->tvForm->setCurrentIndex(index.sibling(row, Form::Name));
    }
}

void FormsWdg::editForm()
{
    QModelIndex index = ui->tvForm->currentIndex();
    if (!index.isValid())
        return;
    int row = index.row();
    QSqlRecord record = formModel->record(index.row());
    int id = record.value(Form::Id).toInt();
    FormEdDlg* dlg = new FormEdDlg(record, db);
    dlg->exec();
    bool changes_formnamenote = false;
    if(QString::compare(record.value(Form::Name).toString(), dlg->getFormName())!=0)
    {
        record.setValue(Form::Name, dlg->getFormName());
        changes_formnamenote = true;
    }
    if(QString::compare(record.value(Form::Note).toString(), dlg->getFormNote())!=0)
    {
        record.setValue(Form::Note, dlg->getFormNote());
        changes_formnamenote = true;
    }
    delete dlg;
    if (changes_formnamenote)
        formModel->setRecord(row, record);
    formModel->submitAll();
    row = findRow(id); //Из-за сортировки по имени может измениться номер строки в tvForm
    ui->tvForm->setCurrentIndex(index.sibling(row, Form::Name));
}

void FormsWdg::deleteForm()
{
    QModelIndex index = ui->tvForm->currentIndex();
    if (!index.isValid())
        return;

    db->database().transaction();
    QSqlRecord record = formModel->record(index.row());
    int id = record.value("id").toInt();
    int numQuests = 0;

    QSqlQuery query(QString("SELECT COUNT(*) FROM questions WHERE form_id = %1").arg(id),db->database());
    if (query.next())
        numQuests = query.value(0).toInt();
    if (numQuests > 0) {
        int r = QMessageBox::warning(this, tr("Deleting the questionnaire"),
                    tr("Delete the questionnaire from the database? Questions and answer will also be deleted!"),
                    QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::No) {
            db->database().rollback();
            return;
        }
        query.exec(QString("SELECT id FROM questions WHERE form_id = %1").arg(id));
        while(query.next())
        {
            int q_id = query.value(0).toInt();
            QSqlQuery query1(db->database());
            query1.exec(QString("DELETE FROM answers WHERE question_id = %1").arg(q_id));
        }
        query.exec(QString("DELETE FROM questions WHERE form_id = %1").arg(id));
    }
    formModel->removeRow(index.row());
    formModel->submitAll();
    db->database().commit();

//    updateChildView();
    ui->tvForm->setFocus();
}

void FormsWdg::printForm()
{
    QModelIndex index = ui->tvForm->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::information(this, tr("Printing the questionnaire"), tr("Questionnaire not selected."));
        return;
    }
    int id = formModel->data(index.sibling(index.row(),Form::Id)).toInt();
    PrintFormDlg* dlg = new PrintFormDlg(id, db, this);
    dlg->exec();
    delete dlg;
}

void FormsWdg::initPanel()
{
    formModel = new QSqlTableModel(this, db->database());
    formModel->setTable(Form::TABLE);
    formModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    formModel->setSort(1, Qt::AscendingOrder);
    formModel->setHeaderData(Form::Id, Qt::Horizontal, "ID");
    formModel->setHeaderData(Form::Name, Qt::Horizontal, tr("Name"));
    formModel->setHeaderData(Form::Note, Qt::Horizontal, tr("Note"));
    formModel->select();

    ui->tvForm->setModel(formModel);
    ui->tvForm->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvForm->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvForm->setColumnHidden(Form::Id, true);
    ui->tvForm->resizeColumnsToContents();
    ui->tvForm->horizontalHeader()->setStretchLastSection(true);
}

int FormsWdg::findRow(int id)
{
    int row = -1;
    while (formModel->canFetchMore())
          formModel->fetchMore();
    for(int i=0; i<formModel->rowCount(); i++)
        if (formModel->data(formModel->index(i, Form::Id)).toInt()==id)
        {
            row = i;
            break;
        }
    return row;
}

void FormsWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("FormsWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void FormsWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("FormsWdg");
    restoreGeometry(settings.value("geometry", geometry()).toByteArray());
    settings.endGroup();
}

