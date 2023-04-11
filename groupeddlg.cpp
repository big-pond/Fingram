#include "groupeddlg.h"
#include "ui_groupeddlg.h"

#include <QSettings>

GroupEdDlg::GroupEdDlg(QWidget *parent) :
    QDialog(parent), ui(new Ui::GroupEdDlg)
{
    ui->setupUi(this);
    readSettings();
}

GroupEdDlg::~GroupEdDlg()
{
    writeSettings();
    delete ui;
}

void GroupEdDlg::setData(const QString &name, const QString &note)
{
    ui->leName->setText(name);
    ui->leNote->setText(note);
}

QString GroupEdDlg::getName()
{
    return ui->leName->text();
}

QString GroupEdDlg::getNote()
{
    return ui->leNote->text();
}

void GroupEdDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("GroupEdDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void GroupEdDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("GroupEdDlg");
    restoreGeometry(settings.value("geometry", this->geometry()).toByteArray());
    settings.endGroup();
}
