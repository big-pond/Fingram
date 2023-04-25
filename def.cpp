#include "def.h"
#include <QObject>
#include <QStandardItemModel>
//#include <QDebug>

const QString Def::APP_NAME = "fingram";
//const QString Def::PICT_EXT = "*.png *.jpg";//"Image files (*.txt)"

const QString Def::FORM = "QUESTIONNAIRE:";
const QString Def::GROUP = "GROUP:";
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
    const double mi_hi = 0.666667;
    QString level = QObject::tr("low");
    double ratio = point_sum/point_max;

    if (ratio>mi_hi)
        level = QObject::tr("high");
    else if ((ratio>=lo_mi)&&(ratio<=mi_hi))
        level = QObject::tr("middle");
    return level;
}

QList<double> Def::levelPercent(QStandardItemModel *model, const double &point_max)
{
    const double lo_mi = 0.333333;
    const double mi_hi = 0.666667;
    QList<double> plst = QList<double>() << 0.0 << 0.0 << 0.0;
    for(int i=0; i<model->rowCount(); i++)
    {
        double point_sum = model->data(model->index(i, 4)).toInt();
        double ratio = point_sum/point_max;
        if (ratio>mi_hi)
            plst[0]++;
        else if ((ratio>=lo_mi)&&(ratio<=mi_hi))
            plst[1]++;
        else
            plst[2]++;
    }
    for(int i=0; i<plst.count(); i++)
        plst[i] /= model->rowCount();
    return plst;
}
