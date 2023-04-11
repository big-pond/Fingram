#ifndef FORMSWDG_H
#define FORMSWDG_H

#include <QWidget>

namespace Ui {
class FormsWdg;
}

class Database;
class QSqlTableModel;

class FormsWdg : public QWidget
{
    Q_OBJECT

public:
    explicit FormsWdg(Database* db, QWidget *parent = 0);
    ~FormsWdg();
private slots:
    void addForm();
    void deleteForm();
    void editForm();

private:
    Ui::FormsWdg *ui;
    Database* db;
    QSqlTableModel* formModel;
    void initPanel();
    int findRow(int id);

    void writeSettings();
    void readSettings();
};

#endif // FORMSWDG_H
