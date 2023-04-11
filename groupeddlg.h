#ifndef GROUPEDDLG_H
#define GROUPEDDLG_H

#include <QDialog>

namespace Ui {
class GroupEdDlg;
}

class GroupEdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GroupEdDlg(QWidget *parent = 0);
    ~GroupEdDlg();
    void setData(const QString& name, const QString& note);
    QString getName();
    QString getNote();
private:
    Ui::GroupEdDlg *ui;
    void writeSettings();
    void readSettings();
};

#endif // GROUPEDDLG_H
