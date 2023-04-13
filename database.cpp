#include "database.h"
#include <QFileInfo>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
#include <QStandardItemModel>
#include <QProgressDialog>
#include <QApplication>

#include "def.h"

#include <QDebug>

//--------------------------------------------------------------
const QString Group::TABLE = "groups";
//--------------------------------------------------------------
const QString Child::TABLE = "childs";
const QString Child::FOREIGN_TABLE1 = "groups";
const QString Child::FOREIGN_KEY1 = "id";
const QString Child::FOREIGN_FIELD1 = "name";
//--------------------------------------------------------------
const QString Form::TABLE = "forms";
//--------------------------------------------------------------
const QString Question::TABLE = "questions";
//--------------------------------------------------------------

Database::Database(QObject *parent) : QObject(parent)
{
}

bool Database::connectToDatabase(const QString &filename)
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление (пустые таблицы)*/
    bool result = false;
    if (!QFileInfo::exists(filename))
        result = restoreDatabase(filename);
    else
        result = openDatabase(filename);
//    if (result) { initModels();  }
    return result;
}

bool Database::restoreDatabase(const QString &filename)
{
    bool result = true;
    if(openDatabase(filename))
        result = createTables();
    else
    {
        qDebug() << "Не удалось восстановить базу данных";
        result = false;
    }
    return result;
}

bool Database::openDatabase(const QString &filename)
{
    /* База данных открывается по заданному пути и имени базы данных, если она существует */
    db = QSqlDatabase::addDatabase("QSQLITE", "MAIN");
    db.setHostName(DATABASE_HOSTNAME);
    db.setDatabaseName(filename);
    bool result = db.open();
    return result;
}

void Database::closeDatabase()
{
    db.close();
    qDebug() << "close database";
}

void Database::loadFromFromFile(const QString &filename)
{
    QFile file(filename);
    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        QString str="";
        bool is_first_line = true;
        while(!file.atEnd())
        {
            if (is_first_line)
            {
                str = file.readLine();
                if(str.left(Def::FORM.length())==Def::FORM)
                {
                  is_first_line = false;
                }
            }
            if (is_first_line)
                break;
            str += file.readLine();
        }
        file.close();
        if(is_first_line)
           return;

        QStringList quest_lst = str.split(QRegExp("\n\r?\n"));


        QProgressDialog progress;
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle(QObject::tr("FinGram"));
        progress.setLabelText(QObject::tr("Adding a questionnaire..."));
        progress.setMinimum(0);
        progress.setMaximum(quest_lst.count() + 1);
        progress.setValue(1);
        QApplication::processEvents();

        int id = 0;
        for(int i=0; i< quest_lst.count(); i++)
        {
            if(i==0)
            {
                id = appendFormFromStr(quest_lst[i]);
            }
            else
            {
                if (id>0)
                    appendQuestionFromStr(quest_lst[i], filename, id);
            }
            progress.setValue(i+1);
            QApplication::processEvents();
        }

        progress.setValue(progress.maximum());
    }
}

int Database::appendFormFromStr(QString str)
{
    int id = 0;
    if (str.contains(Def::FORM))
    {
        QString name = str.remove(Def::FORM).trimmed();
        if(!name.isEmpty())
        {
            QSqlQuery qu_forms(db);
            qu_forms.prepare("INSERT INTO forms (name) VALUES (:Name)");
            qu_forms.bindValue(":Name", name);
            bool success = qu_forms.exec();
            if(success)
            {
                qu_forms.exec("SELECT max(id) FROM forms");
                if(qu_forms.next())
                {
                    id = qu_forms.value(0).toInt();
                }
            }
        }
    }
    return id;
}

void Database::appendQuestionFromStr(const QString &str, const QString &path, int id)
{
    QStringList str_lst = str.split(QRegExp("\r?\n"));
    if (str_lst.count()>1)
    {
        QByteArray byteArray;
        if(str_lst[0].contains(Def::IFILE))
        {
            readPictureStr(str_lst[0], path, byteArray);
        }
        QSqlQuery qu_questions(db);
        QString quest_content = str_lst[1].trimmed();
        if (byteArray.isEmpty())
        {
            qu_questions.prepare("INSERT INTO questions (form_id, content) VALUES (:FormID, :Content)");
            qu_questions.bindValue(":FormID", id);
            qu_questions.bindValue(":Content", quest_content);
        }
        else
        {
            qu_questions.prepare("INSERT INTO questions (form_id, content, imagedata) VALUES (:FormID, :Content, :ImageData)");
            qu_questions.bindValue(":FormID", id);
            qu_questions.bindValue(":Content", quest_content);
            qu_questions.bindValue(":ImageData", byteArray);
        }
        if (qu_questions.exec())
        {
            qu_questions.exec("SELECT max(id) FROM questions");
            if(qu_questions.next())
            {
                int new_question_id = qu_questions.value(0).toInt();
                QSqlQuery qu_answers(db);
                for(int j=2; j<str_lst.count(); j++)
                {
                    QString answ_content = str_lst[j].trimmed();
                    if (!answ_content.isEmpty())
                    {
                        int correct = 0;
                        if(answ_content.right(1)=="#")
                        {
                            answ_content.chop(1);
                            correct = 1;
                        }
                        qu_answers.exec(QString("INSERT INTO answers (question_id, content, correct) VALUES (%1, '%2', %3)").arg(new_question_id).arg(answ_content).arg(correct));
                    }
                }
            }
        }
    }
}

void Database::readPictureStr(QString str, const QString &path, QByteArray &byteArr)
{
    QString fname = str.remove(Def::IFILE).trimmed();
    if (!fname.isEmpty())
    {
        QFileInfo fi(path);
        QString imgpath = fi.path()+"/"+fname;
//        qDebug() << imgpath;
        QFile file(imgpath);
        if (file.open(QIODevice::ReadOnly))
        {
            byteArr = file.readAll();
            file.close();
        }
    }
}

void Database::loadGroupFromFile(const QString &filename)
{
    QFile file(filename);
    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        QString str="";
        bool is_first_line = true;
        while(!file.atEnd())
        {
            if (is_first_line)
            {
                str = file.readLine();
                if(str.left(Def::GROUP.length())==Def::GROUP)
                {
                    is_first_line = false;
                }
            }
            if (is_first_line)
                break;
            str += file.readLine();
        }
        file.close();
        if(is_first_line)
            return;

        QStringList child_lst = str.split(QRegExp("\r?\n"));

        QProgressDialog progress;
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle(QObject::tr("FinGram"));
        progress.setLabelText(QObject::tr("Adding a group..."));
        progress.setMinimum(0);
        progress.setMaximum(child_lst.count() + 1);
        progress.setValue(0);
        QApplication::processEvents();

        int id = 0;
        for(int i=0; i< child_lst.count(); i++)
        {
            if(i==0)
            {
                id = appendGroupFromStr(child_lst[i]);
            }
            else
            {
                if (id>0&&(!child_lst[i].trimmed().isEmpty()))
                    appendChildFromStr(child_lst[i], id);
            }
            progress.setValue(i+1);
            QApplication::processEvents();
        }
        progress.setValue(progress.maximum());
    }
}

int Database::appendGroupFromStr(QString str)
{
    int id = 0;
    if (str.contains(Def::GROUP))
    {
        QString name = str.remove(Def::GROUP).trimmed();
        if(!name.isEmpty())
        {
            QSqlQuery qu_groups(db);
            qu_groups.prepare("INSERT INTO groups (name) VALUES (:Name)");
            qu_groups.bindValue(":Name", name);
            bool success = qu_groups.exec();
            if(success)
            {
                qu_groups.exec("SELECT max(id) FROM groups");
                if(qu_groups.next())
                {
                    id = qu_groups.value(0).toInt();
                }
            }
        }
    }
    return id;
}

void Database::appendChildFromStr(const QString &str, int id)
{
    QStringList str_lst = str.split(QRegExp("\\s+"));
    QString surname, name, date;
    if (str_lst.count()>0)
        surname = str_lst[0];
    if (str_lst.count()>1)
        name = str_lst[1];
    if (str_lst.count()>2)
        date = QDate::fromString(str_lst[2].trimmed(), "dd.MM.yyyy").toString(Qt::ISODate);
     QSqlQuery qu_child(db);
     qu_child.prepare("INSERT INTO childs (group_id, surname, name, db) VALUES (:GroupID, :Surname, :Name, :DateBirth)");
     qu_child.bindValue(":GroupID", id);
     qu_child.bindValue(":Surname", surname);
     qu_child.bindValue(":Name", name);
     qu_child.bindValue(":DateBirth", date);
     qu_child.exec();
}

bool Database::createTables()
{
    bool result = true;
    QFile file(":/resources/createdb.sql");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QStringList querylist;
        while(!stream.atEnd())
            querylist << stream.readLine();
        file.close();

        QSqlQuery query(db);
        for(int i=0; i<querylist.count(); i++)
        {
            if(!querylist[i].isEmpty())
                if(!query.exec(querylist[i]))
                {
                    result = false;
                    break;
                }
        }
    }
    return result;
}

int Database::testingsCount(int form_id, int group_id)
{
    QSqlQuery qu(QString("SELECT count() FROM testing WHERE form_id = %1 AND group_id = %2").arg(form_id).arg(group_id), db);
    int count = 0;
    if(qu.next())
        count = qu.value(0).toInt();
    return count;
}

int Database::getMaxId(const QString &table_name)
{
    int max_id = 0;
    QSqlQuery query(db);
    query.exec(QString("SELECT max(id) FROM '%1'").arg(table_name));
    if (query.next())
        max_id = query.value(0).toInt();
    return max_id;
}

int Database::getChildCount(int group_id)
{
    QSqlQuery query(db);
    query.exec(QString("SELECT count() FROM childs WHERE group_id = %1").arg(group_id));
    int count = 0;
    if(query.next())
        count = query.value(0).toInt();
    return count;
}

QByteArray Database::getQuestionImage(int id)
{
    QByteArray img;
    QSqlQuery query(db);
    query.exec(QString("SELECT imagedata FROM questions WHERE id = %1").arg(id));
    if (query.next())
        img = query.value(0).toByteArray();
    return img;
}

int Database::isQuestionHasAnswers(int id)
{
    QSqlQuery query(db);
    query.exec(QString("SELECT count() FROM answers WHERE question_id = %1").arg(id));
    int count = 0;
    if(query.next())
        count = query.value(0).toInt();
    return count;
}

void Database::deleteQuestionWithAnswers(int id)
{
    QSqlQuery query(db);
    db.transaction();
    query.exec(QString("DELETE FROM answers WHERE question_id = %1").arg(id));
    query.exec(QString("DELETE FROM questions WHERE id = %1").arg(id));
    db.commit();
}

int Database::getQuestiondCount(int form_id)
{
    QSqlQuery query(db);
    query.exec(QString("SELECT count() FROM questions WHERE form_id = %1").arg(form_id));
    int count = 0;
    if(query.next())
        count = query.value(0).toInt();
    return count;
}

void Database::getAnswers(int id, QStandardItemModel *model)
{
    QSqlQuery query(db);
    //  answers (id, question_id, content, correct);
    query.exec(QString("SELECT id, question_id, content, correct FROM answers WHERE question_id = %1").arg(id));
    while (query.next())
    {
        int row = model->rowCount();
        model->insertRow(row, *createStandartItemRow(model->columnCount()));
        model->setData(model->index(row, 0), query.value("id").toInt());
        model->setData(model->index(row, 1), query.value("question_id").toInt());
        model->setData(model->index(row, 2), query.value("content").toString());
        int correct = query.value("correct").toInt();
        model->setData(model->index(row, 3), correct);
        if (correct)
            model->setData(model->index(row, 4), tr("Yes"));
        else

            model->setData(model->index(row, 4), tr("No"));
    }
}

void Database::setAnswers(int form_id, QSqlRecord *question_record, const QByteArray &imgdata, QStandardItemModel *model)
{
    QSqlQuery query(db);
    int question_id = question_record->value("id").toInt();
    QString content = question_record->value("content").toString();
    if (question_id==0) //Новый вопрос
    {
        bool success = query.exec(QString("INSERT INTO questions (form_id, content) "
                           "VALUES(%1, '%2')").arg(form_id).arg(content));
        if (success)
        {
            question_id = getMaxId("questions");
            if (!imgdata.isEmpty())
            {
                query.prepare("UPDATE questions SET imagedata=:ImageData WHERE id=:QuestionId");
                query.bindValue(":QuestionId", question_id);
                query.bindValue(":ImageData", imgdata);
                query.exec();
            }
        }
    }
    else if (question_id>0) //Отредактированный вопрос
    {
        query.prepare("UPDATE questions SET content=:Content, imagedata=:ImageData WHERE id=:QuestionId");
        query.bindValue(":QuestionId", question_id);
        query.bindValue(":Content", content);
        query.bindValue(":ImageData", imgdata);
        query.exec();
    }
    for (int row=0; row<model->rowCount(); row++)
    {
        int id = model->data(model->index(row, 0)).toInt();
        QString content = model->data(model->index(row, 2)).toString();
        int correct = model->data(model->index(row, 3)).toInt();
        if (id==0) //не было записи
        {
            query.prepare("INSERT INTO answers (question_id, content, correct) VALUES (:QuestionId, :Content, :Correct)");
            query.bindValue(":QuestionId", question_id);
            query.bindValue(":Content", content);
            query.bindValue(":Correct", correct);
            query.exec();

        }
        else if (id>0)
        {
            query.prepare("UPDATE answers SET content=:Content, correct=:Correct WHERE id=:Id");
            query.bindValue(":Content", content);
            query.bindValue(":Correct", correct);
            query.bindValue(":Id", id);
            query.exec();
        }
    }
}

//Методы создания анкет для группы (внутри базы данных)
void Database::createFormsForGroup(const QString &filename, int form_id, int group_id, const QDate& tstdate, const QString& note)
{
//Проверка наличия уже подготовленной или отработанной анкеты для группы
    if(testingsCount(form_id, group_id)>0)
        return;
    QSqlQuery query(db);
    query.prepare("INSERT INTO testing (group_id, form_id, tstdate, note) VALUES (:GroupID, :FormId, :TstDate, :Note)");
    query.bindValue(":GroupID", group_id);
    query.bindValue(":FormId", form_id);
    query.bindValue(":TstDate", tstdate.toString(Qt::ISODate));
    query.bindValue(":Note", note);
    query.exec();

    int testing_id = getMaxId("testing");

    QSqlQuery qu_childs(db);
    QSqlQuery qu_questions(db);
    QSqlQuery qu_childanswer(db);

    int child_count = getChildCount(group_id);
    QProgressDialog progress;
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(QObject::tr("FinGram"));
    progress.setLabelText(QObject::tr("Creating questionnaires for the group..."));
    progress.setMinimum(0);
    progress.setMaximum(child_count);
    int progress_index = 0;
    progress.setValue(progress_index);
    QApplication::processEvents();

    qu_childs.prepare("SELECT id FROM childs WHERE group_id = :GroupID");
    qu_childs.bindValue(":GroupID", group_id);
    qu_childs.exec();
    qu_questions.prepare("SELECT id FROM questions WHERE form_id = :FormId");
    qu_questions.bindValue(":FormId", form_id);
    qu_questions.exec();
    while (qu_childs.next())
    {
        int child_id = qu_childs.value(0).toInt();
        if (qu_questions.first())
        {
            do
            {
                int question_id = qu_questions.value(0).toInt();
                qu_childanswer.prepare("INSERT INTO childanswers (testing_id, child_id, question_id, answer_id) "
                                       "VALUES (:TestingId, :ChildId, :QuestionId, 0)");
                qu_childanswer.bindValue(":TestingId", testing_id);
                qu_childanswer.bindValue(":ChildId", child_id);
                qu_childanswer.bindValue(":QuestionId", question_id);
                qu_childanswer.exec();
            }
            while (qu_questions.next());
        }
        progress.setValue(++progress_index);
        QApplication::processEvents();
    }
    progress.setValue(progress.maximum());
}

int Database::getFormIdFromTesting(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT form_id FROM testing WHERE(id = ?)");
    query.bindValue(0, id);
    query.exec();
    int form_id = 0;
    if(query.next())
        form_id = query.value("form_id").toInt();
    return form_id;
}

int Database::getMaxPointQuestionnaire(int form_id)    //Максимально возможное количество баллов по анкете
{
    int point_max = 0;
    QSqlQuery query(db);
    query.prepare("SELECT sum(answers.correct) FROM answers, questions WHERE answers.question_id = questions.id AND questions.form_id = ?");
    query.bindValue(0, form_id);
    query.exec();
    if(query.next())
    {
        point_max = query.value(0).toInt();
    }
    return point_max;
}

double Database::processingTest(int testing_id, QStandardItemModel *model)
{
    QSqlQuery query(db);
    query.prepare("SELECT group_id, form_id, tstdate, note FROM testing WHERE(id = ?)");
    query.bindValue(0, testing_id);
    query.exec();
    int group_id = 0;
    int form_id = 0;
    if(query.next())
    {
        group_id = query.value("group_id").toInt();
        form_id = query.value("form_id").toInt();
    }
    if (group_id==0 || form_id==0)
        return 0;

    int point_max = getMaxPointQuestionnaire(form_id);//Максимально возможное количество баллов по анкете
//    //Максимально возможное количество баллов по анкете
//    int point_max = 0;
//    query.prepare("SELECT sum(answers.correct) FROM answers, questions WHERE answers.question_id = questions.id AND questions.form_id = ?");
//    query.bindValue(0, form_id);
//    query.exec();
//    if(query.next())
//    {
//        point_max = query.value(0).toInt();
//    }
    qDebug() << "point_max" << point_max;
//    double lo_mi = 0.333333;
//    double mi_hi = 0.666667;//низкий средний высокий

    QSqlQuery quChild(db);
    quChild.prepare("SELECT id, surname, name, db, note FROM childs WHERE group_id = ?");
    quChild.bindValue(0, group_id);
    quChild.exec();
    int group_point_sum = 0;
    while(quChild.next())
    {
        int row = model->rowCount();
        model->insertRow(row, *createStandartItemRow(model->columnCount()));
        int child_id = quChild.value("id").toInt();
        model->setData(model->index(row, 0), child_id);
        model->setData(model->index(row, 1), quChild.value("surname").toString());
        model->setData(model->index(row, 2), quChild.value("name").toString());
        model->setData(model->index(row, 3), quChild.value("db").toString());
//        model->setData(model->index(row, 4), 0); //количество баллов
        int point_sum = 0;
        QSqlQuery quChildAnswer(db);
        quChildAnswer.prepare("SELECT id, question_id, answer_id FROM childanswers WHERE testing_id = :TestingId AND child_id = :ChildID");
        quChildAnswer.bindValue(":TestingId", testing_id);
        quChildAnswer.bindValue(":ChildID", child_id);
        quChildAnswer.exec();
        while(quChildAnswer.next())
        {
//            qDebug() << i << quChildAnswer.value("id").toInt() << quChildAnswer.value("question_id").toInt() << quChildAnswer.value("answer_id").toInt();
            int answer_id = quChildAnswer.value("answer_id").toInt();
            int question_id = quChildAnswer.value("question_id").toInt();
            QSqlQuery quAnswer(db);
            quAnswer.prepare("SELECT correct FROM answers WHERE question_id = :QuestionId AND id = :AnswerId");
            quAnswer.bindValue(":QuestionId", question_id);
            quAnswer.bindValue(":AnswerId", answer_id);
            quAnswer.exec();
            if(quAnswer.next())
                point_sum += quAnswer.value("correct").toInt();
        }
        model->setData(model->index(row, 4), point_sum); //количество баллов
        group_point_sum += point_sum;
        QString level = Def::level(point_max, point_sum);
//        QString level = tr("low");
//        double ratio = (double)(point_sum)/point_max;

//        if (ratio>mi_hi)
//            level = tr("high");
//        else if ((ratio>=lo_mi)&&(ratio<=mi_hi))
//            level = tr("middle");
        model->setData(model->index(row, 5), level); //уровень
    }
    int childCount = getChildCount(group_id);
    double average_score = (double) group_point_sum / (double) childCount;
    return average_score;
}

QList<QStandardItem*>* Database::createStandartItemRow(int col_count)
{
    QList<QStandardItem*>* items = new QList<QStandardItem*>();
    for (int col=0; col<col_count; col++)
        items->append(new QStandardItem());
    return items;
}
