#include "groupwdg.h"
#include "ui_groupwdg.h"

#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSettings>

#include "database.h"
#include "groupeddlg.h"
#include "childdlg.h"

#include <QDebug>

//const QList<int> GroupWdg::default_chcolwidths = QList<int>() << 0 << 0 << 160 << 160 <<80 << 250;

GroupWdg::GroupWdg(Database *db, QWidget *parent) :
    QWidget(parent), ui(new Ui::GroupWdg)
{
    ui->setupUi(this);
    this->db = db;
//    chcolwidths << default_chcolwidths;
    initGroupPanel();
    initChildPanel();
    connect(ui->tbAddGroup, SIGNAL(clicked()), this, SLOT(addGroup()));
    connect(ui->tbDelGroup, SIGNAL(clicked()), this, SLOT(deleteGroup()));
    connect(ui->tbEditGroup, SIGNAL(clicked()), this, SLOT(editGroup()));
    connect(ui->tbEditChild, SIGNAL(clicked()), this, SLOT(editChild()));
    connect(ui->tbAddChild, SIGNAL(clicked()), this, SLOT(addChild()));
    connect(ui->tbDelChild, SIGNAL(clicked()), this, SLOT(deleteChild()));
    readSettings();
}

GroupWdg::~GroupWdg()
{
    writeSettings();
    delete ui;
}

void GroupWdg::updateChildView()
{
    QModelIndex index = ui->tvGroup->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = groupModel->record(index.row());
        int id = record.value("id").toInt();
        childModel->setFilter(QString("group_id = %1").arg(id));
    } else {
        childModel->setFilter("group_id = -1");
    }
    childModel->select();
    ui->tvChild->resizeColumnsToContents();
}

void GroupWdg::addGroup()
{
    GroupEdDlg dlg(this);
    if(dlg.exec()==QDialog::Accepted)
    {
        int row = groupModel->rowCount();
        groupModel->insertRow(row);
        QModelIndex index = groupModel->index(row, Group::Name);
        ui->tvGroup->setCurrentIndex(index);
        groupModel->setData(index, dlg.getName());
        groupModel->setData(index.sibling(row, Group::Note), dlg.getNote());
        groupModel->submitAll();
        ui->tvGroup->setCurrentIndex(index);
        updateChildView();
    }
}

void GroupWdg::editGroup()
{
    QModelIndex index = ui->tvGroup->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        QSqlRecord record = groupModel->record(row);
        QString name = record.value(Group::Name).toString();
        QString note = record.value(Group::Note).toString();
        GroupEdDlg dlg(this);
        dlg.setData(name, note);
        if(dlg.exec()==QDialog::Accepted)
        {
            if(QString::compare(name, dlg.getName())!=0)
                record.setValue(Group::Name, dlg.getName());
            if(QString::compare(note, dlg.getNote())!=0)
                record.setValue(Group::Note, dlg.getNote());
            groupModel->setRecord(row, record);
            groupModel->submitAll();
            ui->tvGroup->setCurrentIndex(index);
            updateChildView();
        }
    }
}

void GroupWdg::deleteGroup()//ПРОВЕРКА!!!!
{
    QModelIndex index = ui->tvGroup->currentIndex();
    if (!index.isValid())
        return;

    QSqlRecord record = groupModel->record(index.row());
    int id = record.value(Group::Id).toInt();

    int group_count = db->getTestingGroupCount(id);
    if (group_count>0)
    {
        QMessageBox::information(this, "", tr("The group cannot be deleted because "
                                              "it has already participated in testing."));
        //Группа не может быть удалена так как она уже участвовала в тестировании.
        return;
    }

    db->database().transaction();
    int numChilds = 0;

    QSqlQuery query(QString("SELECT COUNT(*) FROM childs WHERE group_id = %1").arg(id), db->database());
    if (query.next())
        numChilds = query.value(0).toInt();
    if (numChilds > 0) {
        int r = QMessageBox::warning(this, tr("Deleting a group"),
                    tr("Delete %1  from the database? Data about children will also be deleted!")
                    .arg(record.value(Group::Name).toString()),
                    QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::No) {
            db->database().rollback();
            return;
        }
        query.exec(QString("DELETE FROM childs WHERE group_id = %1").arg(id));
    }
    groupModel->removeRow(index.row());
    groupModel->submitAll();
    db->database().commit();

    updateChildView();
    ui->tvGroup->setFocus();
}

void GroupWdg::addChild()
{
    QModelIndex index0 = ui->tvGroup->currentIndex();
    if (index0.isValid())
    {
        int group_id = groupModel->data(index0.sibling(index0.row(), Group::Id)).toInt();
        ChildDlg dlg(this);
        if(dlg.exec()==QDialog::Accepted)
        {
            int row = childModel->rowCount();
            childModel->insertRow(row);
            QModelIndex index = childModel->index(row, Child::GroupId);
            childModel->setData(index, group_id);
            childModel->setData(index.sibling(row, Child::Surname), dlg.getSurname());
            childModel->setData(index.sibling(row, Child::Name), dlg.getName());
            childModel->setData(index.sibling(row, Child::DateBirth), dlg.getDateBirthStr());
            childModel->setData(index.sibling(row, Child::Note), dlg.getNote());
            childModel->submitAll();
            updateChildView();
        }
    }
}

void GroupWdg::deleteChild()
{
    QModelIndex index = ui->tvChild->currentIndex();
    if (!index.isValid())
        return;
    int child_id = childModel->record(index.row()).value(Child::Id).toInt();
    int child_count = db->getTestingChildAnswerCount(child_id);
    if (child_count>0)
    {
        QMessageBox::information(this, "", tr("The child cannot be deleted because "
                                              "he has already participated in testing."));
        //ребенок не может быть удален так как он уже участвовал в тестировании.
        return;
    }

    QString sname = childModel->record(index.row()).value(Child::Surname).toString();
    int r = QMessageBox::question(this, tr("Deleting data about a child"),
                tr("Delete record %1 from the group?").arg(sname));
    if (r == QMessageBox::No)
        return;

    childModel->removeRow(index.row());
    childModel->submitAll();

    updateChildView();
    ui->tvChild->setFocus();
}

void GroupWdg::editChild()
{
    QModelIndex index = ui->tvChild->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        QSqlRecord record = childModel->record(row);
        QString surname = record.value(Child::Surname).toString();
        QString name = record.value(Child::Name).toString();
        QString date = record.value(Child::DateBirth).toString();
        QString note = record.value(Child::Note).toString();
        ChildDlg dlg(this);
        dlg.setData(surname, name, date, note);
        if(dlg.exec()==QDialog::Accepted)
        {
            if(QString::compare(surname, dlg.getSurname())!=0)
                record.setValue(Child::Surname, dlg.getSurname());
            if(QString::compare(name, dlg.getName())!=0)
                record.setValue(Child::Name, dlg.getName());
            if(QString::compare(date, dlg.getDateBirthStr())!=0)
                record.setValue(Child::DateBirth, dlg.getDateBirthStr());
            if(QString::compare(note, dlg.getNote())!=0)
                record.setValue(Child::Note, dlg.getNote());
            childModel->setRecord(row, record);
            childModel->submitAll();
            ui->tvChild->setCurrentIndex(index);
            updateChildView();
            ui->tvChild->setFocus();
        }
    }
}

void GroupWdg::initGroupPanel()
{
    groupModel = new QSqlTableModel(this, db->database());
    groupModel->setTable(Group::TABLE);
    groupModel->setSort(Group::Name, Qt::AscendingOrder);
    groupModel->setHeaderData(Group::Name, Qt::Horizontal,tr("Name"));
    groupModel->setHeaderData(Group::Note,Qt::Horizontal, tr("Note"));
    groupModel->select();

    ui->tvGroup->setModel(groupModel);
    ui->tvGroup->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvGroup->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvGroup->setColumnHidden(Group::Id, true);
    ui->tvGroup->resizeColumnsToContents();
    ui->tvGroup->horizontalHeader()->setStretchLastSection(true);

    connect(ui->tvGroup->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(updateChildView()));
}

void GroupWdg::initChildPanel()
{
    childModel = new QSqlTableModel(this, db->database());
    childModel->setTable(Child::TABLE);
    childModel->setSort(Child::Surname, Qt::AscendingOrder);
    childModel->setHeaderData(Child::Surname, Qt::Horizontal, tr("Surname"));
    childModel->setHeaderData(Child::Name, Qt::Horizontal, tr("Name "));
    childModel->setHeaderData(Child::DateBirth, Qt::Horizontal, tr("Date birth"));
    childModel->setHeaderData(Child::Note, Qt::Horizontal, tr("Note"));

    ui->tvChild->setModel(childModel);
    ui->tvChild->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvChild->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvChild->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvChild->setColumnHidden(Child::Id, true);
    ui->tvChild->setColumnHidden(Child::GroupId, true);
    ui->tvChild->resizeColumnsToContents();
    ui->tvChild->horizontalHeader()->setStretchLastSection(true);
}

void GroupWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("GroupWdg");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter_state", ui->splitter->saveState());
    settings.endGroup();
}

void GroupWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("GroupWdg");
    restoreGeometry(settings.value("geometry", geometry()).toByteArray());
    ui->splitter->restoreState(settings.value("splitter_state",ui->splitter->saveState()).toByteArray());
    settings.endGroup();
}

