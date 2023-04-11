#include "inputtestwdg.h"
#include "ui_inputtestwdg.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelation>
#include <QDataWidgetMapper>

#include "database.h"
#include "answerswdg.h"

#include <QDebug>

InputTestWdg::InputTestWdg(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputTestWdg)
{
    ui->setupUi(this);
    this->db = db;
    childModel = new QSqlQueryModel(this);
    childMapper = new QDataWidgetMapper(this);

    questionModel = new QSqlQueryModel(this);
    questionMapper = new QDataWidgetMapper(this);

    ui->leQuestion->setVisible(false);
    connect(questionMapper, SIGNAL(currentIndexChanged(int)), SLOT(selectPictureAndAnswers(int)));
    initAnswerPanel();
}

InputTestWdg::~InputTestWdg()
{
    delete ui;
}

void InputTestWdg::setData(int testing_id, int group_id, int form_id, const QString &tst_date,
                         const QString &group_name, const QString &form_name, const QString &note)
{
    this->testing_id = testing_id;
    this->group_id = group_id;
    this->form_id = form_id;
    ui->deDate->setDate(QDate::fromString(tst_date, Qt::ISODate));
    ui->leGroup->setText(group_name);
    ui->leForm->setText(form_name);
    ui->leNote->setText(note);
    selectChildren(group_id);
    selectQuestions(form_id);
}

void InputTestWdg::initAnswerPanel()
{
     childanswerModel = new QSqlTableModel(this, db->database());
     childanswerModel->setTable("childanswers");
     //              0      1             2         3           4
     //childanswers (id, testing_id,   child_id, question_id, answer_id);
     childanswerModel->select();
     qDebug() << "answerModel->rowCount()" << childanswerModel->rowCount();
//     qDebug() <<answerModel->record(0);

//     ui->tableView->setModel(childanswerModel);
//     ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
     QVBoxLayout *vbl = dynamic_cast<QVBoxLayout*>(ui->wgAnswersMain->layout());//new QVBoxLayout;
     wgAnswers = new AnswersWdg();
     vbl->insertWidget(-1, wgAnswers, 1);
     wgAnswers->setModel(childanswerModel);
}

void InputTestWdg::childChanged(int i)
{
    ui->tbPrevChild->setEnabled(true);
    ui->tbNextChild->setEnabled(true);
    if(i==0)
        ui->tbPrevChild->setEnabled(false);
    if(i==childModel->rowCount()-1)
        ui->tbNextChild->setEnabled(false);
    questionMapper->toFirst();
}

void InputTestWdg::questionChanged(int i)
{
    ui->pbPrevQuestion->setEnabled(true);
    ui->pbNextQuestion->setEnabled(true);
    if(i==0)
        ui->pbPrevQuestion->setEnabled(false);
    if(i==questionModel->rowCount()-1)
        ui->pbNextQuestion->setEnabled(false);

    ui->lbQuestNum->setText(
                QString("<html><head/><body><p><span style=\" font-size:10pt; font-weight:600; "
                        "color:#005500;\">%1</span></p></body></html>").arg(i+1)
                );
}

void InputTestWdg::setQuestionText(const QString &question)
{
    ui->lbQuestion->setText(
                QString("<html><head/><body><p><span style=\" font-size:10pt; font-weight:600; "
                        "color:#00007f;\">%1</span></p></body></html>").arg(question)
                );
}

void InputTestWdg::selectPictureAndAnswers(int mapper_index)
{
    QByteArray byte_array = questionModel->data(questionModel->index(mapper_index, 2)).toByteArray();
    if (!byte_array.isEmpty())
    {
        QPixmap pixmap = QPixmap();
        pixmap.loadFromData(byte_array );
        ui->lbImage->setPixmap(pixmap.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
//        ui->lbImage->setPixmap(pixmap);
    }
    else
    {
//        ui->lbImage->clear();
        ui->lbImage->setText(tr("No picture"));
    }

    //childanswers (id, testing_id,   child_id, question_id, answer_id);
    int question_id = questionModel->data(questionModel->index(mapper_index, 0)).toInt();
    int child_id = ui->leChildId->text().toInt();
    QString filter = QString("testing_id=%1 AND child_id=%2 AND question_id=%3").arg(testing_id).arg(child_id).arg(question_id);
    childanswerModel->setFilter(filter);
    wgAnswers->updateAnswerList();
}

void InputTestWdg::selectChildren(int group_id)
{
    QSqlQuery query(db->database());
    query.prepare("SELECT childs.id, childs.surname, childs.name, childs.db FROM childs "
                  "WHERE childs.group_id=?");
    query.bindValue(0, group_id);
    query.exec();
    childModel->setQuery(query);
    childMapper->setModel(childModel);
    childMapper->addMapping(ui->leChildId, 0);
    childMapper->addMapping(ui->leSurname, 1);
    childMapper->addMapping(ui->leName, 2);
    childMapper->addMapping(ui->leDateBirth, 3);
    connect(ui->tbPrevChild, SIGNAL(clicked()), childMapper, SLOT(toPrevious()));
    connect(ui->tbNextChild, SIGNAL(clicked()), childMapper, SLOT(toNext()));
    connect(childMapper, SIGNAL(currentIndexChanged(int)), SLOT(childChanged(int)));
    childMapper->toFirst();
}

void InputTestWdg::selectQuestions(int form_id)
{
    QSqlQuery query(db->database());
    query.prepare("SELECT questions.id, questions.content, questions.imagedata FROM questions "
                  "WHERE questions.form_id=?");
    query.bindValue(0, form_id);
    query.exec();
    questionModel->setQuery(query);
    questionMapper->setModel(questionModel);
    questionMapper->addMapping(ui->leQuestion, 1);
    connect(ui->pbPrevQuestion, SIGNAL(clicked()), questionMapper, SLOT(toPrevious()));
    connect(ui->pbNextQuestion, SIGNAL(clicked()), questionMapper, SLOT(toNext()));
    connect(questionMapper, SIGNAL(currentIndexChanged(int)), SLOT(questionChanged(int)));
    connect(ui->leQuestion, SIGNAL(textChanged(QString)), this, SLOT(setQuestionText(QString)));
    questionMapper->toFirst();
}
