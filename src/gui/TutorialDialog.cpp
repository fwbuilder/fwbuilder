#include "TutorialDialog.h"
#include "ui_TutorialDialog.h"

#include <QDebug>
#include <QFile>

TutorialDialog::TutorialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialDialog_q)
{
    ui->setupUi(this);
    animator = NULL;
    currentPage = 0;
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::ApplicationModal);

    QFile f(QString(":/Tutorial/controls.txt"));
    f.open(QFile::ReadOnly);
    foreach(QString line, QString(f.readAll()).split("\n"))
    {
        QStringList parts = line.split(" ");
        if (parts.length() < 3) continue;
        bool ok;
        int page = parts.at(0).toInt(&ok, 10);
        prevEnabled[page] = parts.at(2) == "true";
        requiresPrev[page] = parts.at(1) == "true";
        qDebug() << requiresPrev;
    }


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

QString TutorialDialog::getScenarioForPage(int page)
{
    QFile src(QString(":/Tutorial/commands/page") + QString::number(page) + ".txt");
    src.open(QFile::ReadOnly);
    return QString(src.readAll());
}

void TutorialDialog::showPage(int page)
{
    ui->demonstrate->setEnabled(true);
    QString filename = QString(":/Tutorial/html/page") + QString::number(page) + ".html";
    QFile src(filename);
    src.open(QFile::ReadOnly);
    QString text = src.readAll();
    ui->content->setText(text);
    qDebug() << "next should be enabled:" << page+1 << requiresPrev[page+1];
    bool nextPageExists = QFile::exists(QString(":/Tutorial/html/page") + QString::number(page+1) + ".html");
    ui->next->setEnabled(nextPageExists && (!requiresPrev[page+1]));
    ui->message->setVisible(nextPageExists && requiresPrev[page+1]);
    ui->prev->setEnabled(prevEnabled[page]);
}

void TutorialDialog::demonstrate()
{
    ui->demonstrate->setEnabled(false);
    if (animator != NULL) delete animator;
    animator = new TutorialAnimator(this, getScenarioForPage(currentPage));
    animator->setSpeed(ui->speed->value());
    animator->start();
    ui->next->setEnabled(QFile::exists(QString(":/Tutorial/html/page") + QString::number(currentPage+1) + ".html"));
    if (QFile::exists(QString(":/Tutorial/html/page") + QString::number(currentPage+1) + ".html"))
    {
        showPage(++currentPage);
    }
    //ui->message->setVisible(false);
}
