#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QByteArray>

class QStandardItemModel;
class QStandardItem;

struct Group {
public:
    enum {Id, Name, Note};

    static const QString TABLE;
};

struct Child {
public:
    enum {Id, GroupId, Surname, Name, DateBirth, Note};
    static const QString TABLE;
    static const QString FOREIGN_TABLE1;
    static const QString FOREIGN_KEY1;
    static const QString FOREIGN_FIELD1;
};

struct Form {
public:
    enum {Id, Name, Note};
    static const QString TABLE;
};

struct Question {
public:
    enum {Id, FormId, Content, ImageData};
    static const QString TABLE;
};

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    bool connectToDatabase(const QString& filename);
    void closeDatabase();
    QSqlDatabase& database(){return db;}

    void loadFromFromFile(const QString& filename);
    void loadGroupFromFile(const QString& filename);

    void createFormsForGroup(const QString& filename, int form_id, int group_id, const QDate &tstdate, const QString &note);

    int testingsCount(int form_id, int group_id);
    int getMaxId(const QString& table_name);
    int getChildCount(int group_id);

    QByteArray getQuestionImage(int id);
    int isQuestionHasAnswers(int id);
    void deleteQuestionWithAnswers(int id);
    int getQuestiondCount(int form_id);

    void getAnswers(int id, QStandardItemModel* model);
    void setAnswers(int form_id, QSqlRecord* question_record, const QByteArray& imgdata, QStandardItemModel* model);

    int getFormIdFromTesting(int id);
    int getMaxPointQuestionnaire(int form_id);//Максимально возможное количество баллов по анкете
    double processingTest(int testing_id, QStandardItemModel* model);


signals:

public slots:

private:
    QString DATABASE_HOSTNAME;
    QSqlDatabase db;
    bool createTables();
    bool restoreDatabase(const QString &filename);
    bool openDatabase(const QString &filename);

    bool createGroupFormTables(QSqlDatabase& fc_db);
    bool fillingGroupFormTables(QSqlDatabase& fc_db, int form_id, int child_id);

    int appendFormFromStr(QString str);
    void appendQuestionFromStr(const QString& str, const QString &path, int id);
    void readPictureStr(QString str, const QString &path, QByteArray& byteArr);


    int appendGroupFromStr(QString str);
    void appendChildFromStr(const QString& str, int id);

    QList<QStandardItem*>* createStandartItemRow(int col_count);

};

#endif // DATABASE_H
