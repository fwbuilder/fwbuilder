/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id$

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "InterfacesTabWidget.h"
#include "ui_InterfacesTabWidget.h"

using namespace libfwbuilder;

InterfacesTabWidget::InterfacesTabWidget(QWidget *parent) :
    QTabWidget(parent),
    m_ui(new Ui::InterfacesTabWidget)
{
    m_ui->setupUi(this);
    clusterMode = false;
    clear();
    connect(&newInterface, SIGNAL(clicked()), this, SLOT(addNewInterface()));
    connect(&delInterface, SIGNAL(clicked()), this, SLOT(closeTab()));
    newInterface.setIcon(QIcon(":/Icons/add.png"));
    delInterface.setIcon(QIcon(":/Icons/del.png"));
    newInterface.setToolTip(tr("Add new interface"));
    delInterface.setToolTip(tr("Delete current interface"));
    this->setCornerWidget(&delInterface, Qt::TopRightCorner);
    this->setCornerWidget(&newInterface, Qt::TopLeftCorner);
    this->cornerWidget(Qt::TopRightCorner)->show();
    this->cornerWidget(Qt::TopLeftCorner)->show();
    noTabs = true;
}

InterfacesTabWidget::~InterfacesTabWidget()
{
    delete m_ui;
}

void InterfacesTabWidget::changeEvent(QEvent *e)
{
    QTabWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QMap<Interface*, EditedInterfaceData> InterfacesTabWidget::getData()
{
    QMap<Interface*, EditedInterfaceData> res;
    for ( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget *w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (w == nullptr || w->getInterface() == nullptr)
            continue;
        InterfaceEditorWidget *intEditor = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (intEditor != nullptr)
            res[intEditor->getInterface()] = intEditor->getInterfaceData();
    }
    return res;
}

QList<EditedInterfaceData> InterfacesTabWidget::getNewData()
{
    QList<EditedInterfaceData> res;
    for ( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget *w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if ( w != nullptr && w->getInterface() == nullptr)
            res.append(w->getInterfaceData());
    }
    return res;
}

QList<Interface*> InterfacesTabWidget::getDeletedInterfaces()
{
    return deleted;
}

void InterfacesTabWidget::addInterface(Interface *iface)
{
    if (noTabs)
    {
        this->removeTab(0);
        noTabs = false;
        this->delInterface.setEnabled(true);
    }
    InterfaceEditorWidget *w = new InterfaceEditorWidget(this, iface);
    w->setClusterMode(clusterMode);
    this->addTab(w, iface->getName().c_str());
}

void InterfacesTabWidget::addNewInterface()
{
    if (noTabs)
    {
        this->removeTab(0);
        noTabs = false;
        this->delInterface.setEnabled(true);
    }
    InterfaceEditorWidget *w = new InterfaceEditorWidget(this);
    w->setClusterMode(clusterMode);
    w->setHostOS(host_OS);
    addTab(w, ""); // deliberately create new interface with blank name
    setCurrentIndex(count() - 1);
}

void InterfacesTabWidget::setTemplate(FWObject* obj)
{
    currentTemplate = obj;
}

void InterfacesTabWidget::clear()
{
    while ( this->count() ) this->removeTab(0);
    this->addNewInterface();
}

void InterfacesTabWidget::closeTab()
{
    if (noTabs) return;
    int idx = this->currentIndex();
    QWidget *w = this->widget(idx);
    Interface *iface = dynamic_cast<InterfaceEditorWidget*>(w)->getInterface() ;
    if ( iface != nullptr ) deleted.append( iface );
    this->removeTab(idx);
    delete w;
    if (this->count() == 0)
    {
        noTabs = true;
        QString text;
        if (!this->clusterMode)
            text = tr("This firewall has no interfaces. Add interface using button <img src=\":/Icons/add.png\" width=15 height=15>.");
        else
            text = tr("This cluster has no interfaces. Add interface using button <img src=\":/Icons/add.png\" width=15 height=15>.");
        QLabel *label = new QLabel(text, this);
        label->setAlignment(Qt::AlignCenter);
        this->addTab(label, tr("No interfaces"));
        this->delInterface.setEnabled(false);
    }
}

bool InterfacesTabWidget::isValid()
{
    for (int i = 0; i< this->count(); i++)
    {
        InterfaceEditorWidget* w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (w == nullptr) continue;
        if (!w->isValid())
        {
            this->setCurrentWidget(w);
            w->showError();
            return false;
        }
    }
    return true;
}

void InterfacesTabWidget::setCornerWidgetsVisible(bool st)
{
    this->cornerWidget(Qt::TopRightCorner)->setVisible(st);
    this->cornerWidget(Qt::TopLeftCorner)->setVisible(st);
}

void InterfacesTabWidget::addInterfaceFromData(InterfaceData* idata)
{
    InterfaceEditorWidget *w = new InterfaceEditorWidget(this);
    w->setData(idata);
    addTab(w, idata->name.c_str());
}

void InterfacesTabWidget::addTab(QWidget* widget, const QString& title)
{
    if ( dynamic_cast<InterfaceEditorWidget*>(widget) != nullptr ||
         (noTabs && dynamic_cast<QLabel*>(widget) != nullptr))
    {
        widget->setObjectName(title+"_widget");
        if (!noTabs)
        dynamic_cast<InterfaceEditorWidget*>(widget)->setExplanation(explanation);
        QTabWidget::addTab(widget, title);
    }
}

void InterfacesTabWidget::setClusterMode(bool st)
{
    clusterMode = st;
    for ( int i = 0; i < this->count() ; i++ )
    {
        InterfaceEditorWidget *w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (w!=nullptr) w->setClusterMode(st);
    }
    newInterface.setVisible(!st);
    delInterface.setVisible(!st);
}

void InterfacesTabWidget::addClusterInterface(ClusterInterfaceData data)
{
    InterfaceEditorWidget *w = new InterfaceEditorWidget(this, data);
    w->setClusterMode(true);
    this->addTab(w, data.name);
}

void InterfacesTabWidget::setExplanation(const QString& text)
{
    explanation = text;
    for( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget* w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (w!=nullptr) w->setExplanation(text);
    }
}

void InterfacesTabWidget::setHostOS(const QString &s)
{
    host_OS = s;
    for ( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget *w = dynamic_cast<InterfaceEditorWidget*>(
            this->widget(i));
        if (w != nullptr) w->setHostOS(host_OS);
    }
}
