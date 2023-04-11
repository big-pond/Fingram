#include "formeddlg.h"
#include "ui_formeddlg.h"

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QItemSelection>
#include <QSettings>
#include <QMessageBox>

#include "database.h"
#include "questioneddlg.h"

#include <QDebug>

const QList<int> FormEdDlg::default_questcolwidthlst = QList<int>() << 50 << 300;
const QList<int> FormEdDlg::default_answcolwidthlst = QList<int>() << 0 << 0 << 350 << 0 << 80;

FormEdDlg::FormEdDlg(const QSqlRecord& record, Database *db, QWidget *parent) :
    QDialog(parent),  ui(new Ui::FormEdDlg)
{
    ui->setupUi(this);
    this->db = db;
    answcolwidthlst << default_answcolwidthlst;

    connect(ui->tbAddQuestion, SIGNAL(clicked()), SLOT(addQuestion()));
    connect(ui->tbEditQuestion, SIGNAL(clicked()), SLOT(editQuestion()));
    connect(ui->tbDelQuestion, SIGNAL(clicked()), SLOT(delQuestion()));

    connect(ui->tbQuesColWidth, SIGNAL(clicked()), SLOT(defaultQuestionColWidth()));
    connect(ui->tbAnsColWidth, SIGNAL(clicked()), SLOT(defaultAnswerColWidth()));

    questModel = new QSqlQueryModel(this);
    answrModel = new QSqlTableModel(this, db->database());
    ui->tvQuestion->setModel(questModel);
    ui->tvQuestion->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvQuestion->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvAnswer->setModel(answrModel);
    setForm(record);
    connect(ui->tvQuestion->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(questionSelectionChanged(QItemSelection,QItemSelection)));

    for(int i=0; i<default_questcolwidthlst.count(); i++)
         ui->tvQuestion->setColumnWidth(i, default_questcolwidthlst[i]);
    readSettings();
}

FormEdDlg::~FormEdDlg()
{
    writeSettings();
    delete ui;
}

QString FormEdDlg::getFormName()
{
    return ui->leName->text();
}

QString FormEdDlg::getFormNote()
{
    return ui->leNote->text();
}

void FormEdDlg::setForm(const QSqlRecord &record)
{
    form_id = record.value("id").toInt();

    ui->leId->setText(record.value("id").toString());
    ui->leName->setText(record.value("name").toString());
    ui->leNote->setText(record.value("note").toString());

    questQueryStr = QString("SELECT id, content FROM questions WHERE form_id=%1").arg(form_id);
    answrModel->setTable("answers");
    updateQuestionModel();
}

void FormEdDlg::questionSelectionChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    if (selected.indexes().count()>0)
    {
        int question_id = questModel->data(selected.indexes()[0]).toInt();
        updateAnswerModel(question_id);
    }
}

void FormEdDlg::addQuestion()
{
    QSqlRecord record = questModel->record();
//    qDebug() << "form_id" << form_id << "record =" << record.value("id").toInt() << record.value("content").toString();
    QuestionEdDlg* dlg = new QuestionEdDlg(form_id, record, db);
    if (dlg->exec()==QDialog::Accepted)
    {
        updateQuestionModel();
        int count = db->getQuestiondCount(form_id);
        ui->tvQuestion->setCurrentIndex(questModel->index(count, Question::Content));
    }
    delete dlg;
}

void FormEdDlg::editQuestion()
{
    QModelIndex index = ui->tvQuestion->currentIndex();
    if (index.isValid())
    {
        QSqlRecord record = questModel->record(index.row());
        QuestionEdDlg* dlg = new QuestionEdDlg(form_id, record, db);
        if (dlg->exec()==QDialog::Accepted)
        {
            updateQuestionModel();
            ui->tvQuestion->setCurrentIndex(index);
        }
        delete dlg;
    }
}

void FormEdDlg::delQuestion()
{
    QModelIndex index = ui->tvQuestion->currentIndex();
    if (index.isValid())
    {
        QSqlRecord record = questModel->record(index.row());
        int id = record.value(Question::Id).toInt();
        int count = db->isQuestionHasAnswers(id);
        QString msg = tr("Delete question?");
        if (count>0)
                msg += tr("Together with this, %1 answer will be deleted.").arg(count);
        if (QMessageBox::question(this, "", msg)==QMessageBox::Yes)
        {
            db->deleteQuestionWithAnswers(id);
            updateQuestionModel();
        }
    }
}

void FormEdDlg::defaultQuestionColWidth()
{
    for(int i=0; i<default_questcolwidthlst.count(); i++)
        ui->tvAnswer->setColumnWidth(i, default_questcolwidthlst[i]);
}

void FormEdDlg::defaultAnswerColWidth()
{
    for(int i=0; i<default_answcolwidthlst.count(); i++)
        ui->tvAnswer->setColumnWidth(i, default_answcolwidthlst[i]);
}

void FormEdDlg::updateQuestionModel()
{
    questModel->setQuery(questQueryStr, db->database());
    questModel->setHeaderData(0, Qt::Horizontal, "ID");
    questModel->setHeaderData(1, Qt::Horizontal, tr("Content"));
}

void FormEdDlg::updateAnswerModel(int question_id)
{
    if(question_id<=0) return;
    answrModel->setFilter(QString("question_id=%1").arg(question_id));
    answrModel->select();
    answrModel->setHeaderData(0, Qt::Horizontal, "ID");
    answrModel->setHeaderData(1, Qt::Horizontal, tr("Question ID"));
    answrModel->setHeaderData(2, Qt::Horizontal, tr("Content"));
    answrModel->setHeaderData(3, Qt::Horizontal, tr("Correct"));
    for(int i=0; i<answrModel->columnCount(); i++)
        ui->tvAnswer->setColumnWidth(i, answcolwidthlst[i]);
    QByteArray byte_array = db->getQuestionImage(question_id);
    if (!byte_array.isEmpty())
    {
        QPixmap pixmap = QPixmap();
        pixmap.loadFromData(byte_array );
        ui->lbImage->setPixmap(pixmap.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
    }
    else
    {
        ui->lbImage->setText(tr("No picture"));
    }
}

void FormEdDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("FormEdDlg");
    settings.setValue("geometry", saveGeometry());
    settings.beginWriteArray("quest_col_width");
    for(int i=0; i<ui->tvQuestion->model()->columnCount(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", ui->tvQuestion->columnWidth(i));
    }
    settings.endArray();
    settings.beginWriteArray("answr_col_width");
    for(int i=0; i<ui->tvAnswer->model()->columnCount(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", ui->tvAnswer->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();
}

void FormEdDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("FormEdDlg");
    restoreGeometry(settings.value("geometry", this->geometry()).toByteArray());
    int size = settings.beginReadArray("quest_col_width");
    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        ui->tvQuestion->setColumnWidth(i, settings.value("width", default_questcolwidthlst[i]).toInt());
    }
    settings.endArray();
    size = settings.beginReadArray("answr_col_width");
    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        answcolwidthlst[i] = settings.value("width", default_answcolwidthlst[i]).toInt();
    }
    settings.endArray();
    settings.endGroup();
}
