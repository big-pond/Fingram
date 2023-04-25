#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QTranslator;
class Database;
class QSqlQueryModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
private slots:
    void settings();
    void loadFormFromFile();
    void loadGroupFromFile();
    void editGroups();
    void editForms();
    void createFormsForGroup();
    void inputTestData();
    void testProcessing();
    void deleteTestResults();
    void help();
    void about();
    void aboutQt();

private:
    Ui::MainWindow *ui;
    Database* db;
    QSqlQueryModel* testingModel;
    QTranslator* translator;
    QTranslator* qt_translator;
    void writeSettings();
    void readSettings();
    void initTestingModel();
    void updateActions();
    void setCurretFile(const QString &fname);
    int findRow(int id);
};

#endif // MAINWINDOW_H
