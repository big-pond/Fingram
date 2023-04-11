#include "answerwdg.h"

#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>

AnswerWdg::AnswerWdg(int h, QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* hl = new QHBoxLayout(this);
    lbAnswer = new QLabel();
    lbAnswer->setWordWrap(true);
    hl->addWidget(lbAnswer, 1);
    cbCorrect = new QCheckBox(this);
    hl->addWidget(cbCorrect, 0);
    hl->setSpacing(4);
    setLayout(hl);
    setContentsMargins(0,0,0,0);
    setFixHeight(h);
    connect(cbCorrect, SIGNAL(clicked(bool)), SLOT(checkboxClicked(bool)));
}

void AnswerWdg::setAnswer(const QString &text)
{
    lbAnswer->setText(text);
}

void AnswerWdg::setCorrect(bool val)
{
    cbCorrect->setChecked(val);
}

void AnswerWdg::setContent(int id, const QString &text, int correct)
{
    answer_id = id;
    lbAnswer->setText(text);
    cbCorrect->setChecked(correct>0);
}

int AnswerWdg::getAnswerId()
{
    return answer_id;
}

QString AnswerWdg::getAnswer()
{
    return lbAnswer->text();
}

bool AnswerWdg::isCorrect()
{
    return cbCorrect->isChecked();
}

QCheckBox *AnswerWdg::getCorrectCheckBox()
{
    return cbCorrect;
}

void AnswerWdg::checkboxClicked(bool value)
{
    emit answerClicked(value, cbCorrect);
}

void AnswerWdg::setFixHeight(int h)
{
    lbAnswer->setFixedHeight(h);
    cbCorrect->setFixedHeight(h);
    setFixedHeight(h);
}
