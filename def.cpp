#include "def.h"

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
