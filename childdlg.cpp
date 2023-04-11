#include "childdlg.h"
#include "ui_childdlg.h"

#include <QSettings>

ChildDlg::ChildDlg(QWidget *parent) : QDialog(parent), ui(new Ui::ChildDlg)
{
    ui->setupUi(this);

    ui->lbSurName->setBuddy(ui->leSurName);
    ui->lbName->setBuddy(ui->lbName);
    ui->lbDateBirth->setBuddy(ui->deDateBirth);
    ui->lbNote->setBuddy(ui->leNote);
    ui->leSurName->setFocus();
    ui->deDateBirth->setDate(QDate::currentDate());
    setWindowTitle(tr("Edit data about children")); //Редактировать данные о детях
    readSettings();
}

ChildDlg::~ChildDlg()
{
    writeSettings();
    delete ui;
}

void ChildDlg::setData(const QString &surename, const QString &name, const QString &datebirth, const QString &note)
{
    ui->leSurName->setText(surename);
    ui->leName->setText(name);
    ui->deDateBirth->setDate(QDate::fromString(datebirth, Qt::ISODate));
    ui->leNote->setText(note);
}

QString ChildDlg::getSurname()
{
    return ui->leSurName->text();
}

QString ChildDlg::getName()
{
    return ui->leName->text();
}

QString ChildDlg::getDateBirthStr()
{
    return ui->deDateBirth->date().toString(Qt::ISODate);
}

QString ChildDlg::getNote()
{
    return ui->leNote->text();
}

void ChildDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("ChildDlg");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("date", ui->deDateBirth->date());
    settings.endGroup();
}

void ChildDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("ChildDlg");
    restoreGeometry(settings.value("geometry", this->geometry()).toByteArray());
    ui->deDateBirth->setDate(settings.value("date", ui->deDateBirth->date()).toDate());
    settings.endGroup();
}
