#ifndef CHILDDLG_H
#define CHILDDLG_H

#include <QDialog>

namespace Ui {
class ChildDlg;
}

class ChildDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ChildDlg(QWidget *parent = 0);
    ~ChildDlg();
    void setData(const QString& surename, const QString& name, const QString& datebirth, const QString& note);
    QString getSurname();
    QString getName();
    QString getDateBirthStr();
    QString getNote();
public slots:

private slots:

private:
    Ui::ChildDlg *ui;
    void writeSettings();
    void readSettings();
};

#endif // CHILDDLG_H
