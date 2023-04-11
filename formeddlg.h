#ifndef FORMEDDLG_H
#define FORMEDDLG_H

#include <QDialog>

namespace Ui {
class FormEdDlg;
}

class QSqlRecord;
class QSqlTableModel;
class QSqlQueryModel;
class QItemSelection;

class Database;

class FormEdDlg : public QDialog
{
    Q_OBJECT

public:
    static const QList<int> default_questcolwidthlst;
    static const QList<int> default_answcolwidthlst;
    explicit FormEdDlg(const QSqlRecord& record, Database* db, QWidget *parent = 0);
    ~FormEdDlg();
    QString getFormName();
    QString getFormNote();
private slots:
    void questionSelectionChanged(const QItemSelection &selected, const QItemSelection &);
    void addQuestion();
    void delQuestion();
    void editQuestion();
    void defaultQuestionColWidth();
    void defaultAnswerColWidth();
private:
    Ui::FormEdDlg *ui;
    Database* db;
    QString questQueryStr;
    QSqlQueryModel *questModel;
    QSqlTableModel *answrModel;
    int form_id;
    QList<int> answcolwidthlst;
    void setForm(const QSqlRecord& record);
    void updateQuestionModel();
    void updateAnswerModel(int question_id);

    void writeSettings();
    void readSettings();
};

#endif // FORMEDDLG_H
