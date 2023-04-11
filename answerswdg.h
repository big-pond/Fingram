#ifndef ANSWERSWDG_H
#define ANSWERSWDG_H

#include <QWidget>

namespace Ui {
class AnswersWdg;
}

class AnswerWdg;
class QSqlTableModel;
class QVBoxLayout;
class QCheckBox;

class AnswersWdg : public QWidget
{
    Q_OBJECT

public:
    explicit AnswersWdg(QWidget *parent = 0);
    ~AnswersWdg();
    void setModel(QSqlTableModel* model);
    void updateAnswerList();

private:
    Ui::AnswersWdg *ui;
private slots:
    void selectAnswer(bool value, QCheckBox* checkbox);
signals:
    sendAnswerId(int id);
private:
    QList<AnswerWdg*> answerList;
    QSqlTableModel* model;
    QVBoxLayout* vlayout;

    void init();
    void appendPanel();
    bool setAnswerContent(int row, int id, const QString& text, int correct);
};

#endif // ANSWERSWDG_H
