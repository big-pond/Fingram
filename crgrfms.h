#ifndef CRGRFMS_H
#define CRGRFMS_H

#include <QDialog>

namespace Ui {
class CrGrFms;
}

class Database;

class CrGrFms : public QDialog
{
    Q_OBJECT

public:
    explicit CrGrFms(Database* db, QWidget *parent = 0);
    ~CrGrFms();

public slots:
    void accept();

private:
    Ui::CrGrFms *ui;
    Database* db;
    void writeSettings();
    void readSettings();
};

#endif // CRGRFMS_H
