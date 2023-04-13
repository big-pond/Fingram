#include "def.h"
#include <QObject>

const QString Def::APP_NAME = "fingram";
//const QString Def::PICT_EXT = "*.png *.jpg";//"Image files (*.txt)"

const QString Def::FORM = "АНКЕТА:";
const QString Def::GROUP = "ГРУППА:";
const QString Def::IFILE = "IFILE:";

QString Def::formdir = "./forms";
QString Def::groupdir = "./groups";
QString Def::dbfilename = "./fingram.sl3";

int Def::language_id = -1;
QStringList Def::language_list = QStringList() << "English;en_US"
                                               << "Русский;ru_RU";
QString Def::languageName(int i)
{
    return Def::language_list[i].split(";")[0];
}

QString Def::languageSuffix(int i)
{
    return Def::language_list[i].split(";")[1];
}

QString Def::level(const double &point_max, const double &point_sum)
{
    const double lo_mi = 0.333333;
    const double mi_hi = 0.666667;//низкий средний высокий
    QString level = QObject::tr("low");
    double ratio = point_sum/point_max;

    if (ratio>mi_hi)
        level = QObject::tr("high");
    else if ((ratio>=lo_mi)&&(ratio<=mi_hi))
        level = QObject::tr("middle");
    return level;
}
