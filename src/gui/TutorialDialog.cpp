#include "TutorialDialog.h"
#include "ui_TutorialDialog.h"

#include <QDebug>
#include <QFile>

#define TUTORIALS_DIR "/Users/a2k/prog/fwbuilder/fwbuilder400.app/Contents/Resources/tutorials"
//../Contents/Resources/tutorials"
#define FS_SEPARATOR "/"

TutorialDialog::TutorialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialDialog_q)
{
    ui->setupUi(this);
    animator = NULL;
    currentPage = 0;
    showPage(currentPage);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
//    ui->content->setMaximumWidth(ui->scrollArea->viewport()->width());
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
    QString filename = QString(":/Tutorial/html/page") + QString::number(page) + ".html";
    qDebug() << filename;
    QFile src(filename);
    src.open(QFile::ReadOnly);
    QString text = src.readAll();
    qDebug() << text;
    ui->content->setText(text);
    ui->next->setEnabled(QFile::exists(QString(":/Tutorial/html/page") + QString::number(page+1) + ".html"));
    ui->prev->setEnabled(QFile::exists(QString(":/Tutorial/html/page") + QString::number(page-1) + ".html"));
}

void TutorialDialog::demonstrate()
{
    if (animator != NULL) delete animator;
    animator = new TutorialAnimator(this, getScenarioForPage(currentPage));
}
