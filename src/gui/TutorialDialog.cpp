#include "TutorialDialog.h"
#include "ui_TutorialDialog.h"

#include <QDebug>
#include <QFile>

TutorialDialog::TutorialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialDialog_q)
{
    ui->setupUi(this);
    currentPage = 0;
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    //this->setWindowModality(Qt::ApplicationModal);
    showPage(currentPage);
}

void TutorialDialog::resizeEvent(QResizeEvent *)
{
//    ui->content->setMaximumWidth(ui->scrollArea->viewport()->width());
}

TutorialDialog::~TutorialDialog()
{
    delete ui;
}

void TutorialDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TutorialDialog::next()
{
    currentPage++;
    showPage(currentPage);
}

void TutorialDialog::previous()
{
    currentPage--;
    showPage(currentPage);
}

void TutorialDialog::reset()
{
    currentPage = 0;
    showPage(currentPage);
}

void TutorialDialog::showPage(int page)
{
    QString filename = QString(":/Tutorial/html/page") + QString::number(page) + ".html";
    QFile src(filename);
    src.open(QFile::ReadOnly);
    QString text = src.readAll();
    ui->content->setText(text);
    bool nextPageExists = QFile::exists(QString(":/Tutorial/html/page") + QString::number(page+1) + ".html");
    bool prevPageExists = QFile::exists(QString(":/Tutorial/html/page") + QString::number(page-1) + ".html");
    ui->next->setEnabled(nextPageExists);
    ui->prev->setEnabled(prevPageExists);
}
