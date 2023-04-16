#ifndef INPUTTESTWDG_H
#define INPUTTESTWDG_H

#include <QWidget>

namespace Ui {
class InputTestWdg;
}

class Database;
class QSqlQueryModel;
class QSqlTableModel;
class QDataWidgetMapper;

class AnswersWdg;

class InputTestWdg : public QWidget
{
    Q_OBJECT

public:
    explicit InputTestWdg(Database* db, QWidget *parent = 0);
    ~InputTestWdg();
    void setData(int testing_id, int group_id, int form_id, const QString& tst_date, const QString& group_name, const QString& form_name, const QString& note);
private slots:
    void childChanged(int i);
    void questionChanged(int i);
    void setQuestionText(const QString &question);
    void selectPictureAndAnswers(int mapper_index);
private:
    Ui::InputTestWdg *ui;
    Database* db;
    QSqlQueryModel* childModel;
    QDataWidgetMapper* childMapper;

    QSqlQueryModel* questionModel;
    QDataWidgetMapper* questionMapper;

    QSqlTableModel* childanswerModel;
    AnswersWdg *wgAnswers;
    int testing_id;
    int group_id;
    int form_id;

    void initAnswerPanel();

    void selectChildren(int group_id);
    void selectQuestions(int form_id);

    void writeSettings();
    void readSettings();
};

#endif // INPUTTESTWDG_H
