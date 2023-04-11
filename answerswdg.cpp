#include "answerswdg.h"
#include "ui_answerswdg.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>

#include "answerwdg.h"

#include <QDebug>

AnswersWdg::AnswersWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnswersWdg)
{
    ui->setupUi(this);
    vlayout = dynamic_cast<QVBoxLayout*>(layout());
    init();
}

AnswersWdg::~AnswersWdg()
{
    while(!answerList.isEmpty())
    {
        AnswerWdg* w = answerList.takeLast();
        vlayout->removeWidget(w);
        delete w;
    }
    delete ui;
}

void AnswersWdg::setModel(QSqlTableModel *model)
{
    this->model = model;
}

void AnswersWdg::updateAnswerList()
{
    int rowCount = model->rowCount();
    qDebug() << rowCount;
    int answerCount = 0;
    if(rowCount > 0)
    {
        QSqlRecord record = model->record(0);
        int question_id = record.value("question_id").toInt();
        int answer_id = record.value("answer_id").toInt();
        QSqlQuery query(model->database());
        query.prepare("SELECT count() FROM answers WHERE answers.question_id=?");
        query.bindValue(0, question_id);
        query.exec();
        if(query.next())
            answerCount = query.value(0).toInt();
        for(int i = answerList.count(); i<answerCount; i++)
            appendPanel();

        query.prepare("SELECT answers.id, answers.content FROM answers "
                      "WHERE answers.question_id=?");
        query.bindValue(0, question_id);
        query.exec();
        int i = 0;
        while(query.next())
        {
            int correct = 0;
            if (answer_id == query.value("id").toInt())
                correct = 1;
            setAnswerContent(i,
                             query.value("id").toInt(),
                             query.value("content").toString(),
                             correct
                             );
            i++;
        }
    }
    for(int i = 0; i<answerList.count(); i++)
    {
        answerList[i]->setVisible(i<answerCount);
    }
}

void AnswersWdg::init()
{
    //Минимум 2 ответа правильный и неправильный
    for (int i=0; i<2; i++)
    {
        appendPanel();
    }
}

void AnswersWdg::appendPanel()
{
    answerList << new AnswerWdg();
    answerList.last()->setContent(2, "А. Это банковские карты.", 0);
    connect(answerList.last(), SIGNAL(answerClicked(bool, QCheckBox*)), SLOT(selectAnswer(bool, QCheckBox*)));
    vlayout->insertWidget(-1, answerList.last(), 0);
}

void AnswersWdg::selectAnswer(bool value, QCheckBox *checkbox)
{
    int answer_id = 0;
    for(int i=0; i<answerList.count(); i++ )
    {

        if(answerList[i]->getCorrectCheckBox()==checkbox)
        {
            answerList[i]->setCorrect(value);
            if (value)
                answer_id = answerList[i]->getAnswerId();
        }
        else
            answerList[i]->setCorrect(false);
    }
    if(model->rowCount() > 0) //Передается одна строка. Проверяем что передана
    {
        QSqlRecord record = model->record(0);
        record.setValue("answer_id",answer_id);
        model->setRecord(0, record);
    }
}

bool AnswersWdg::setAnswerContent(int row, int id, const QString &text, int correct)
{
    if (row>(answerList.count()-1))
        appendPanel();
    answerList[row]->setContent(id, text, correct);
}
