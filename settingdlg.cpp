#include "settingdlg.h"
#include "ui_settingdlg.h"

#include <QFileDialog>

#include "def.h"

SettingDlg::SettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDlg)
{
    ui->setupUi(this);
    connect(ui->tbSelectFormDir, SIGNAL(clicked(bool)),SLOT(selectFormDir()));
    connect(ui->tbSelectDbFileName, SIGNAL(clicked(bool)), SLOT(selectDbFileName()));
    ui->leFormDir->setText(Def::formdir);
    ui->leDbFileName->setText(Def::dbfilename);
}

SettingDlg::~SettingDlg()
{
    delete ui;
}

void SettingDlg::accept()
{
    Def::formdir = ui->leFormDir->text();
    QDialog::accept();
}

void SettingDlg::selectFormDir()
{
    QString sdir = QFileDialog::getExistingDirectory(this,"", Def::formdir);
    if(!sdir.isEmpty())
        ui->leFormDir->setText(sdir);
}

void SettingDlg::selectDbFileName()
{
    QString fname = QFileDialog::getOpenFileName(this,"", Def::dbfilename);
    if(!fname.isEmpty())
        ui->leDbFileName->setText(fname);
}
