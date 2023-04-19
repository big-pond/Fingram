#include "questioneddlg.h"
#include "ui_questioneddlg.h"

#include <QSqlRecord>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "def.h"
#include "database.h"

QuestionEdDlg::QuestionEdDlg(int form_id, const QSqlRecord &record, Database *db, QWidget *parent) :
    QDialog(parent), ui(new Ui::QuestionEdDlg)
{
    ui->setupUi(this);
    this->form_id = form_id;
    this->db = db;
    question_id = record.value("id").toInt();
    question_record = new QSqlRecord(record);

    ui->textEdit->setPlainText(question_record->value("content").toString());

    imagedata = db->getQuestionImage(question_id);
    if (!imagedata.isEmpty())
    {
        QPixmap pixmap = QPixmap();
        pixmap.loadFromData(imagedata );
        ui->lbImage->setPixmap(pixmap.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
    }
    else
    {
        ui->lbImage->setText(tr("No picture"));
    }

    connect(ui->tbSelectPicture, SIGNAL(clicked()), SLOT(loadImage()));
    connect(ui->tbDeletePicture, SIGNAL(clicked()), SLOT(deleteImage()));

    connect(ui->tbAdd, SIGNAL(clicked()), SLOT(addAnswer()));
    connect(ui->tbDel, SIGNAL(clicked()), SLOT(deleteAnswer()));
    connect(ui->tbSort, SIGNAL(clicked()), SLOT(sortAnswer()));

    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(answerDoubleClicked(QModelIndex)));
    model->setColumnCount(5);
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Answer")));
    model->setHorizontalHeaderItem(4, new QStandardItem(tr("Сorrect\nanswer")));
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(3, true);
    ui->tableView->setColumnWidth(2, 240);
    ui->tableView->setColumnWidth(4, 80);
    db->getAnswers(question_id, model);
    readSettings();
}

QuestionEdDlg::~QuestionEdDlg()
{
    writeSettings();
    delete ui;
}

void QuestionEdDlg::accept()
{
    question_record->setValue("content", ui->textEdit->toPlainText());
    db->setAnswers(form_id, question_record, imagedata, model);
    QDialog::accept();
}

void QuestionEdDlg::answerDoubleClicked(const QModelIndex &index)
{
    if(index.column()==4)
    {
        for(int row=0; row<model->rowCount(); row++)
        {
            if (index.row()==row)
            {
                model->setData(model->index(row, 4), tr("Yes"));
                model->setData(model->index(row, 3), 1);
            }
            else
            {
                model->setData(model->index(row, 4), tr("No"));
                model->setData(model->index(row, 3), 0);
            }
        }
    }
    else if (index.column()==2)
    {
        bool ok = false;
        QString content = QInputDialog::getText(this, tr("Answer"), "", QLineEdit::Normal, model->data(index).toString(),&ok);
        if (ok)
            model->setData(index, content);
    }
}

void QuestionEdDlg::addAnswer()
{
    model->appendRow(*createStandartItemRow(model->columnCount()));
    int row = model->rowCount() - 1;
    model->setData(model->index(row, 0), 0);
    model->setData(model->index(row, 1), question_id);
    model->setData(model->index(row, 2), "");
    model->setData(model->index(row, 3), 0);
    model->setData(model->index(row, 4), tr("No"));
}

void QuestionEdDlg::deleteAnswer()
{
    QModelIndex index = ui->tableView->currentIndex();  //CHECK REFERENCES to childanswers table!!!!!
    if (index.isValid())
    {
        int id = model->data(index.sibling(index.row(),0), 0).toInt();
        int child_answer_count = db->getChildAnswerCount(id);
        if (child_answer_count>0)
        {
            QMessageBox::information(this, "", tr("The answer cannot be deleted "
                                                  "because it has already been used in testing."));
            //Ответ не может быть удален так как он уже использовался в тестировании.
            return;
        }

        if (QMessageBox::question(this, "", tr("Delete answer?"))==QMessageBox::Yes)
            model->removeRow(index.row());
    }
    else
        QMessageBox::information(this, "", tr("Line not selected."));
}

void QuestionEdDlg::sortAnswer()
{
    model->sort(2);
}

void QuestionEdDlg::loadImage()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Load image"), Def::formdir, tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fname.isEmpty())
    {
        QFile file(fname);
        if (file.open(QIODevice::ReadOnly))
        {
            imagedata = file.readAll();
            file.close();
            QPixmap pixmap = QPixmap();
            pixmap.loadFromData(imagedata );
            ui->lbImage->setPixmap(pixmap.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
        }
    }
}

void QuestionEdDlg::deleteImage()
{
    imagedata.clear();
    ui->lbImage->setText(tr("No picture"));
}

QList<QStandardItem*> *QuestionEdDlg::createStandartItemRow(int col_count)
{
    QList<QStandardItem*>* items = new QList<QStandardItem*>();
    for (int col=0; col<col_count; col++)
        items->append(new QStandardItem());
    return items;
}

void QuestionEdDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("QuestionEdDlg");
    settings.setValue("geometry", saveGeometry());
    int col_width = ui->tableView->columnWidth(2);
    if(col_width==0)
        col_width = 240;
    settings.setValue("answer_col", col_width);
    col_width = ui->tableView->columnWidth(4);
    if(col_width==0)
        col_width = 80;
    settings.setValue("correct_col", col_width);
    settings.endGroup();
}

void QuestionEdDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("QuestionEdDlg");
    restoreGeometry(settings.value("geometry", geometry()).toByteArray());
    ui->tableView->setColumnWidth(2, settings.value("answer_col", 240).toInt());
    ui->tableView->setColumnWidth(4, settings.value("correct_col", 80).toInt());
    settings.endGroup();
}

