#ifndef DEF_H
#define DEF_H

#include <QString>
#include <QStringList>

class QStandardItemModel;

class Def
{
public:
    static const QString APP_NAME;
//    static const QString PICT_EXT;

    static const QString FORM;
    static const QString GROUP;
    static const QString IFILE;

    static QString formdir;
    static QString groupdir;
    static QString dbfilename;

    static int language_id;
    static QStringList language_list;
    static QString languageName(int i);
    static QString languageSuffix(int i);
    static QString level(const double& point_max, const double& point_sum);
    static QList<double> levelPercent(QStandardItemModel* model, const double& point_max);
    static int languageCount();
};

#endif // DEF_H
