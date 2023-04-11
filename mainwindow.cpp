#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTranslator>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include "def.h"
#include "database.h"
#include "settingdlg.h"
#include "crgrfms.h"
#include "groupwdg.h"
#include "formswdg.h"
#include "inputtestwdg.h"
#include "testprocwdg.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QApplication::setOrganizationName("Test");
    QApplication::setApplicationName(Def::APP_NAME);
    QApplication::setApplicationDisplayName(tr("FinancialGramota"));
    QApplication::setApplicationVersion("01");

    translator = new QTranslator(this);
    qt_translator = new QTranslator(this) ;

    db = new Database(this);

    setCentralWidget(ui->tableView);

    ui->mainToolBar->toggleViewAction()->setText("Панель быстрого доступа");
    ui->menuView->addAction(ui->mainToolBar->toggleViewAction());

    QAction* action = new QAction("Строка статуса");
    action->setCheckable(true);
    action->setChecked(true);
    ui->menuView->addAction(action);
    connect(action, SIGNAL(triggered(bool)), ui->statusBar, SLOT(setVisible(bool)));

    connect(ui->actionSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), SLOT(close()));

    connect(ui->actionLoadFormFromFile, SIGNAL(triggered()), SLOT(loadFormFromFile()));
    connect(ui->actionEditForms,SIGNAL(triggered()), SLOT(editForms()));

    connect(ui->actionLoadGroupFromFile, SIGNAL(triggered()), SLOT(loadGroupFromFile()));
    connect(ui->actionEditGroups, SIGNAL(triggered()), SLOT(editGroups()));

    connect(ui->actionCreateFormsForGroup, SIGNAL(triggered()), SLOT(createFormsForGroup()));

    connect(ui->actionInputTestData, SIGNAL(triggered()), SLOT(inputTestData()));
    connect(ui->actionProcessing, SIGNAL(triggered()), SLOT(testProcessing()));

    connect(ui->actionHelp, SIGNAL(triggered()), SLOT(help()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), SLOT(aboutQt()));

    readSettings();
    if(Def::language_id==-1)//Первый запуск
    {
        if (QLocale::system().name()=="ru_RU")
            Def::language_id = 1;
        else
            Def::language_id = 0;
    }
    QLocale::system().name();
    // Загружаем перевод и устанавливаем в приложение
    translator->load(Def::APP_NAME +"_" + Def::languageSuffix(Def::language_id), ".");
    qApp->installTranslator(translator);
    // Загружаем перевод кнопок и устанавливаем в приложение
    qt_translator->load("qtbase_" + Def::languageSuffix(Def::language_id), ".");
    qApp->installTranslator(qt_translator);
    if (db->connectToDatabase(Def::dbfilename))
    {
        setCurretFile(Def::dbfilename);
        initTestingModel();
        updateActions();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
//    int res = QMessageBox::question(this,"",tr("Exit the application?"),QMessageBox::Yes, QMessageBox::No);
    int res = QMessageBox::Yes;
    if(res==QMessageBox::Yes)
    {
        writeSettings();
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        QApplication::setApplicationDisplayName(tr("FinancialGramota"));
        this->setWindowTitle(Def::dbfilename);
//        actionExit->setText(tr("E&xit"));
//        actionExit->setStatusTip(tr("Exit the application"));
    }
}
void MainWindow::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowstate", saveState());
    settings.setValue("language", Def::language_id);
    settings.endGroup();

    settings.beginGroup("Files");
    settings.setValue("form_dir", Def::formdir);
    settings.setValue("group_dir", Def::groupdir);
    settings.setValue("db_file_name", Def::dbfilename);
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    bool mainwingeom = settings.contains("geometry");
    if (mainwingeom)
    {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowstate").toByteArray());
    }
    else
        setGeometry(0,20,800, 600);
    Def::language_id = settings.value("language", Def::language_id).toInt();
    settings.endGroup();

    settings.beginGroup("Files");
    Def::formdir = settings.value("form_dir", Def::formdir).toString();
    Def::groupdir = settings.value("group_dir", Def::groupdir).toString();
    Def::dbfilename = settings.value("db_file_name",Def::dbfilename).toString();
    settings.endGroup();
}

void MainWindow::settings()
{
    SettingDlg* dlg = new SettingDlg();
    dlg->exec();
    delete dlg;
}

void MainWindow::setCurretFile(const QString &fname)
{
    Def::dbfilename = fname;
    this->setWindowTitle(Def::dbfilename);
}

void MainWindow::initTestingModel()
{
    testingModel = new QSqlQueryModel(this);
    testingModel->setQuery("SELECT testing.id, testing.group_id, testing.form_id, testing.tstdate, groups.name, forms.name, testing.note "
                         "FROM testing, groups, forms WHERE "
                         "testing.group_id=groups.id AND testing.form_id=forms.id "
                         "ORDER BY groups.name, testing.tstdate",db->database());
    testingModel->setHeaderData(3, Qt::Horizontal,"Дата теста");
    testingModel->setHeaderData(4, Qt::Horizontal, "Группа");
    testingModel->setHeaderData(5 ,Qt::Horizontal, "Тест");
    testingModel->setHeaderData(6, Qt::Horizontal, "Примечание");

    ui->tableView->setModel(testingModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(2, true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

//    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &,const QModelIndex &)),
//            this, SLOT(updateChildView(const QModelIndex &,const QModelIndex &)));

}
void MainWindow::loadFormFromFile()
{
    QString fname = QFileDialog::getOpenFileName(this,"", Def::formdir, "Text file (*.txt)");
    if (!fname.isEmpty())
    {
        db->loadFromFromFile(fname);
        QFileInfo fi(fname);
        Def::formdir = fi.absolutePath();
    }
}

void MainWindow::loadGroupFromFile()
{
    QString fname = QFileDialog::getOpenFileName(this,"", Def::groupdir, tr("Text file (*.txt)"));
    if (!fname.isEmpty())
    {
        db->loadGroupFromFile(fname);
        QFileInfo fi(fname);
        Def::groupdir = fi.absolutePath();
    }
}

void MainWindow::editGroups()
{
    GroupWdg* grpwdg = new GroupWdg(db);
    grpwdg->setAttribute(Qt::WA_DeleteOnClose);
    grpwdg->show();
}

void MainWindow::editForms()
{
    FormsWdg* frmwdg = new FormsWdg(db);
    frmwdg->setAttribute(Qt::WA_DeleteOnClose);
    frmwdg->show();
}

void MainWindow::createFormsForGroup()
{
    CrGrFms* dlg = new CrGrFms(db);
    dlg->exec();
    delete dlg;
}

void MainWindow::inputTestData()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        InputTestWdg* inpwdg = new InputTestWdg(db);
        inpwdg->setAttribute(Qt::WA_DeleteOnClose);
        QSqlRecord record = testingModel->record(index.row());
        //testing_id, group_id, form_id, tst_date, group_name, form_name, note
        inpwdg->setData(record.value(0).toInt(),
                        record.value(1).toInt(),
                        record.value(2).toInt(),
                        record.value(3).toString(),
                        record.value(4).toString(),
                        record.value(5).toString(),
                        record.value(6).toString()
                        );
        inpwdg->show();

    }
    else
        QMessageBox::information(this, "", tr("The test record is not selected."));
}

void MainWindow::testProcessing()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        QSqlRecord record = testingModel->record(index.row());
        TestProcWdg* tpwdg = new TestProcWdg(record.value(0).toInt(), db);
        tpwdg->setAttribute(Qt::WA_DeleteOnClose);
        tpwdg->processingTest();
        tpwdg->show();
    }
    else
        QMessageBox::information(this, "", tr("The test record is not selected."));

}

void MainWindow::updateActions()
{

}


void MainWindow::about()
{
    QFileInfo fi(QApplication::applicationFilePath());
    QDateTime dt = fi.lastModified();
    QMessageBox::about(this, tr("About"),
        tr("<table><tr><td><img src=\":/resources/finlit64.png\"></td><td><h1 align=\"center\">%1</h1></td></tr></table>"
           "<p align=\"center\">Версия %2</p>"
           "<p align=\"center\">Сборка: %3</p>"
           "<p align=\"center\">Copyright &copy; %4 %5</p>"
           "<p align=\"center\">Сайт программы  <a href=\"http://www.????.ru\">http://www.????.ru</a></p>"
           "<p align=\"justify\">Эта программа является свободным программным обеспечением. "
           "Вы можете распространять ее и/или модифицировать соблюдая условия "
           "Открытого Лицензионного Соглашения (GNU General Public License) версии 2, "
           "опубликованного Free Software Foundation, или (на ваш выбор) любой последующей версии."
           "<br>Для разработки программы использовались IDE Qt Creator с компилятором <a href=\"https://osdn.net/projects/mingw/\">MinGW С++</a> и "
           "библиотека Qt 6  <a href=\"https://www.qt.io/\">https://qt.io</a>;"
           )
           .arg(QApplication::applicationName())
           .arg(QApplication::applicationVersion())
           .arg(dt.toString("dd.MM.yyyy hh:mm"))
           .arg(dt.date().year())
           .arg(QApplication::organizationName()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::help()
{

    //Из Бланшет,Саммерфилд
    //QUrl url(directoryOf("doc").absoluteFilePath("index.html"));
    //url.setScheme("file");
    //QDesktopServices::openUrl(url);
}

