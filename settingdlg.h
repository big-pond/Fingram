#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <QDialog>

namespace Ui {
class SettingDlg;
}

class SettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDlg(QWidget *parent = 0);
    ~SettingDlg();
public slots:
    void accept();
private slots:
    void selectFormDir();
    void selectDbFileName();

private:
    Ui::SettingDlg *ui;
    void writeSettings();
    void readSettings();
};

#endif // SETTINGDLG_H
