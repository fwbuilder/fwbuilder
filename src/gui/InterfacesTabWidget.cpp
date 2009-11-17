/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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
    clear();
    newInterface = new QToolButton();
    delInterface = new QToolButton();
    connect(newInterface, SIGNAL(clicked()), this, SLOT(addNewInterface()));
    connect(delInterface, SIGNAL(clicked()), this, SLOT(closeTab()));
    newInterface->setIcon(QIcon(":/Icons/add.png"));
    delInterface->setIcon(QIcon(":/Icons/del.png"));
    newInterface->setToolTip(tr("Add new interface"));
    delInterface->setToolTip(tr("Delete current interface"));
    this->setCornerWidget(delInterface, Qt::TopRightCorner);
    this->setCornerWidget(newInterface, Qt::TopLeftCorner);
    this->cornerWidget(Qt::TopRightCorner)->show();
    this->cornerWidget(Qt::TopLeftCorner)->show();
}

InterfacesTabWidget::~InterfacesTabWidget()
{
    delete m_ui;
    delete newInterface;
    delete delInterface;
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
        if ( dynamic_cast<InterfaceEditorWidget*>(this->widget(i))->getInterface() == NULL)
            continue;
        InterfaceEditorWidget *intEditor = dynamic_cast<InterfaceEditorWidget*>(this->widget(i));
        res[intEditor->getInterface()] = intEditor->getInterfaceData();
    }
    return res;
}

QList<EditedInterfaceData> InterfacesTabWidget::getNewData()
{
    QList<EditedInterfaceData> res;
    for ( int i = 0; i < this->count(); i++ )
    {
        InterfaceEditorWidget *w = dynamic_cast<InterfaceEditorWidget*>(this->widget(i));
        if ( w->getInterface() == NULL)
            res.append(w->getInterfaceData());
    }
    return res;
}

QList<Interface*> InterfacesTabWidget::getDeletedInterfaces()
{
    return deleted;
}

void InterfacesTabWidget::addInterface(Interface *interface)
{
    this->addTab(new InterfaceEditorWidget(this, interface), interface->getName().c_str());
}

void InterfacesTabWidget::addNewInterface()
{
    addTab(new InterfaceEditorWidget(this), tr("New interface"));
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
    if ( this->count() == 1 ) return;
    int idx = this->currentIndex();
    QWidget *w = this->widget(idx);
    Interface *interface = dynamic_cast<InterfaceEditorWidget*>(w)->getInterface() ;
    if ( interface != NULL ) deleted.append( interface );
    this->removeTab(idx);
    delete w;
}


bool InterfacesTabWidget::isValid()
{
    for (int i = 0; i< this->count(); i++)
    {
        if (!dynamic_cast<InterfaceEditorWidget*>(this->widget(i))->isValid())
            return false;
    }
    return true;
}

void InterfacesTabWidget::setCornerWidgetsVisible(bool st)
{
    this->cornerWidget(Qt::TopRightCorner)->setVisible(st);
    this->cornerWidget(Qt::TopLeftCorner)->setVisible(st);
}

void InterfacesTabWidget::addInterface(InterfaceData* idata)
{
    addTab(new InterfaceEditorWidget(this, idata), idata->name.c_str());
}
