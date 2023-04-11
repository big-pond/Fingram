#ifndef TESTPROCWDG_H
#define TESTPROCWDG_H

#include <QWidget>

namespace Ui {
class TestProcWdg;
}

class QStandardItemModel;
class Database;

class TestProcWdg : public QWidget
{
    Q_OBJECT

public:
    static const int COL_COUNT;
    static const int defaultColW[];
    enum {ChildId, Surname, Name, BirthDate, Points, Level};
    explicit TestProcWdg(int testing_id, Database* db, QWidget *parent = 0);
//id, tstdate, form_id, group_id, note
    ~TestProcWdg();
    void processingTest();

private:
    Ui::TestProcWdg *ui;
    Database* db;
    int testing_id;
    QStandardItemModel* model;
};

#endif // TESTPROCWDG_H
