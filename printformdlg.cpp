#include "printformdlg.h"
#include "ui_printformdlg.h"
#include <QSettings>
#include "database.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

PrintFormDlg::PrintFormDlg(int id, Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintFormDlg)
{
    ui->setupUi(this);
    this->db = db;
    ui->textEdit->setFocus();
    connect(ui->tbPrint, SIGNAL(clicked()), SLOT(print()));
    connect(ui->tbToPdf, SIGNAL(clicked()), SLOT(toPdf()));
    questionnairePreparation(id);
    readSettings();

}

PrintFormDlg::~PrintFormDlg()
{
    writeSettings();
    delete ui;
}

void PrintFormDlg::print()
{
    QPrintDialog* dlg =  new QPrintDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QPrinter* printer = dlg->printer();
        QTextDocument* doc = ui->textEdit->document();
        doc->print(printer);
    }
    delete dlg;
}

void PrintFormDlg::toPdf()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Save to PDF format file"), "", tr("PDF file (*.pdf)")); //Сохранить в файл формата PDF
    if (!fname.isEmpty())
    {
        QPrinter printer;
        printer.setOutputFileName(fname);
        printer.setOutputFormat(QPrinter::PdfFormat);
        ui->textEdit->print(&printer);
    }
}

void PrintFormDlg::questionnairePreparation(int id)
{
    QTextDocument* doc = ui->textEdit->document();
    db->getQuestionnaireText(id, doc);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);
}

void PrintFormDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("PrintFormDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void PrintFormDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("PrintFormDlg");
    restoreGeometry(settings.value("geometry", geometry()).toByteArray());
    settings.endGroup();
}
