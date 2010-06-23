#include "FirewallCodeViewer.h"
#include "ui_FirewallCodeViewer.h"
#include <QFile>

FirewallCodeViewer::FirewallCodeViewer(QStringList files, QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirewallCodeViewer_q)
{
    ui->setupUi(this);
    this->files = files;
    foreach(QString file, files)
        ui->fileSelector->addItem(file.split("/").last());
    fileSelected(0);
    ui->path->setText(name);
}

FirewallCodeViewer::~FirewallCodeViewer()
{
    delete ui;
}

void FirewallCodeViewer::changeEvent(QEvent *e)
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

void FirewallCodeViewer::fileSelected(int idx)
{
    QFile f(this->files.at(idx));
    f.open(QFile::ReadOnly);
    ui->text->setPlainText(f.readAll());
}
