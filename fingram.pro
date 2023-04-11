#-------------------------------------------------
#
# Project created by QtCreator 2023-03-07T14:36:55
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fingram
TEMPLATE = app
RC_FILE = fingram.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    def.cpp \
    database.cpp \
    crgrfms.cpp \
    answerswdg.cpp \
    answerwdg.cpp \
    inputtestwdg.cpp \
    groupwdg.cpp \
    childdlg.cpp \
    formswdg.cpp \
    settingdlg.cpp \
    testprocwdg.cpp \
    formeddlg.cpp \
    questioneddlg.cpp \
    groupeddlg.cpp

HEADERS += \
        mainwindow.h \
    def.h \
    database.h \
    crgrfms.h \
    answerswdg.h \
    answerwdg.h \
    inputtestwdg.h \
    groupwdg.h \
    childdlg.h \
    formswdg.h \
    settingdlg.h \
    testprocwdg.h \
    formeddlg.h \
    questioneddlg.h \
    groupeddlg.h

FORMS += \
        mainwindow.ui \
    crgrfms.ui \
    answerswdg.ui \
    inputtestwdg.ui \
    groupwdg.ui \
    childdlg.ui \
    formswdg.ui \
    settingdlg.ui \
    testprocwdg.ui \
    formeddlg.ui \
    questioneddlg.ui \
    groupeddlg.ui

RESOURCES += \
    fingram.qrc

TRANSLATIONS += fingram_ru.ts
