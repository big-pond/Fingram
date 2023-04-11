#include "crgrfms.h"
#include "ui_crgrfms.h"

#include <QSqlQuery>

#include "database.h"

CrGrFms::CrGrFms(Database *db, QWidget *parent) :
    QDialog(parent), ui(new Ui::CrGrFms)
{
    ui->setupUi(this);
    this->db = db;

    QSqlQuery query(db->database());
    query.exec("SELECT id, name FROM forms");
    while(query.next())
        ui->cbForm->addItem(query.value("name").toString(), query.value("id"));

    query.exec("SELECT id, name FROM groups");
    while(query.next())
        ui->cbGroup->addItem(query.value("name").toString(), query.value("id"));

    ui->deDate->setDate(QDate::currentDate());

}

CrGrFms::~CrGrFms()
{
    delete ui;
}

void CrGrFms::accept()
{
    db->createFormsForGroup(ui->leFolder->text(),
                            ui->cbForm->currentData().toInt(),
                            ui->cbGroup->currentData().toInt(),
                            ui->deDate->date(),
                            ui->leNote->text());
    QDialog::accept();
}
