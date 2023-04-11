#ifndef GROUPWDG_H
#define GROUPWDG_H

#include <QWidget>

namespace Ui {
class GroupWdg;
}

class Database;
class QSqlTableModel;

class GroupWdg : public QWidget
{
    Q_OBJECT

public:
    explicit GroupWdg(Database* db, QWidget *parent = 0);
    ~GroupWdg();

private slots:
    void updateChildView();
    void addGroup();
    void editGroup();
    void deleteGroup();
    void addChild();
    void deleteChild();
    void editChild();
private:
    Ui::GroupWdg *ui;
    Database* db;
    QSqlTableModel* groupModel;
    QSqlTableModel* childModel;

    void inidChildPanel();
    void initGroupPanel();

    void writeSettings();
    void readSettings();
};

#endif // GROUPWDG_H
