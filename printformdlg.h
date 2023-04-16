#ifndef PRINTFORMDLG_H
#define PRINTFORMDLG_H

#include <QDialog>

namespace Ui {
class PrintFormDlg;
}

class Database;

class PrintFormDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PrintFormDlg(int id, Database* db, QWidget *parent = 0);
    ~PrintFormDlg();
private slots:
    void print();
    void toPdf();

private:
    Ui::PrintFormDlg *ui;
    Database *db;
    void questionnairePreparation(int id);
    void writeSettings();
    void readSettings();
};

#endif // PRINTFORMDLG_H
