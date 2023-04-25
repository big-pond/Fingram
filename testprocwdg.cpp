#include "testprocwdg.h"
#include "ui_testprocwdg.h"

#include <QStandardItemModel>
#include <QSettings>
#include <QDebug>

#include "def.h"
#include "database.h"

const int TestProcWdg::COL_COUNT = 6;
const int TestProcWdg::defaultColW[] = {25, 80, 80, 60, 60, 80};

TestProcWdg::TestProcWdg(int testing_id, Database *db, QWidget *parent) :
    QWidget(parent), ui(new Ui::TestProcWdg)
{
    ui->setupUi(this);
    this->db = db;
    this->testing_id = testing_id;
    model = new QStandardItemModel(this);
    model->setColumnCount(COL_COUNT);
    model->setHorizontalHeaderItem(ChildId, new QStandardItem(tr("Child ID")));
    model->setHorizontalHeaderItem(Surname, new QStandardItem(tr("Surname")));
    model->setHorizontalHeaderItem(Name, new QStandardItem(tr("Name")));
    model->setHorizontalHeaderItem(BirthDate, new QStandardItem(tr("Date of birth")));
    model->setHorizontalHeaderItem(Points, new QStandardItem(tr("Number of\n points")));
    model->setHorizontalHeaderItem(Level, new QStandardItem(tr("Level")));
    ui->tableView->setModel(model);
    readSettings();
}

TestProcWdg::~TestProcWdg()
{
    writeSettings();
    delete ui;
}

void TestProcWdg::processingTest()
{
    double average_score = db->processingTest(testing_id, model);
    ui->lbAverageScore->setText(QString::number(average_score, 'f', 2));
    int form_id = db->getFormIdFromTesting(testing_id);
    int point_max = db->getMaxPointQuestionnaire(form_id);
    ui->lbMaxPoints->setText(QString("%1").arg(point_max));
    ui->lbGroupLevel->setText(Def::level(point_max, average_score));
    QList<double> plst = Def::levelPercent(model, point_max);

    ui->lbHighPercent->setText(QString("%1 \%").arg(plst[0]*100, 0, 'f', 1));
    ui->lbMiddlePercent->setText(QString("%1 \%").arg(plst[1]*100, 0, 'f', 1));
    ui->lbLowPercent->setText(QString("%1 \%").arg(plst[2]*100, 0, 'f', 1));
}

void TestProcWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("TestProcWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void TestProcWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("TestProcWdg");
    restoreGeometry(settings.value("geometry", geometry()).toByteArray());
    settings.endGroup();
}
