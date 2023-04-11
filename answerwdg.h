#ifndef ANSWERWDG_H
#define ANSWERWDG_H

#include <QWidget>

class QLabel;
class QCheckBox;

class AnswerWdg : public QWidget
{
    Q_OBJECT
public:
    explicit AnswerWdg(int h = 40, QWidget *parent = nullptr);
    void  setFixHeight(int h);

    void setAnswerId(int id) { answer_id = id;}
    void setAnswer(const QString& text);
    void setCorrect(bool val);
    void setContent(int id, const QString& text, int correct);

    int getAnswerId();
    QString getAnswer();
    bool isCorrect();
    QCheckBox* getCorrectCheckBox();

signals:
    void answerClicked(bool value, QCheckBox* chekbox);
public slots:
private slots:
    void checkboxClicked(bool value);
private:
    int answer_id;
    QLabel* lbAnswer;
    QCheckBox* cbCorrect;
};

#endif // ANSWERWDG_H
