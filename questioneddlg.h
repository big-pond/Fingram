#ifndef QUESTIONEDDLG_H
#define QUESTIONEDDLG_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class QuestionEdDlg;
}

class QSqlRecord;
class Database;
class QStandardItemModel;
class QStandardItem;

class QuestionEdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionEdDlg(int form_id, const QSqlRecord& record, Database* db, QWidget *parent = 0);
    ~QuestionEdDlg();
public slots:
    void accept();
private slots:
    void answerDoubleClicked(const QModelIndex &index);
    void addAnswer();
    void deleteAnswer();
    void sortAnswer();
    void loadImage();
    void deleteImage();
private:
    Ui::QuestionEdDlg *ui;
    Database* db;
    int form_id;
    int question_id;
    QSqlRecord* question_record;
    QByteArray imagedata;
    QStandardItemModel* model;
    QList<QStandardItem*>* createStandartItemRow(int col_count);
};

#endif // QUESTIONEDDLG_H
