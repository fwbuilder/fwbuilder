/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "global.h"
#include "events.h"
#include "FWWindow.h"
#include "ProjectPanel.h"

#include "CreateObjectsPage.h"

#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QStringList>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


CreateObjectsPage::CreateObjectsPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::CreateObjectsPage_q;
    m_dialog->setupUi(this);

}

void CreateObjectsPage::initializePage()
{
    if (fwbdebug) qDebug() << "CreateObjectsPage::initializePage()";

    int lib_index = field("libIndex").toInt();
    QStringList libraries = field("libraries").toStringList();
    QStringList objects = field("objectsToUse").toStringList();

    if (fwbdebug)
    {
        qDebug() << "libraries=" << libraries;
        qDebug() << "objects=" << objects;
    }

    m_dialog->progressBar->setFormat("%v / %m");
    m_dialog->progressBar->setMaximum(objects.size() / 2);

    FWObject *last_object = nullptr;
    QString name;
    QString addr;
    int counter = 1;
    while (objects.size() > 0)
    {
        name = objects.front(); objects.pop_front();
        addr = objects.front(); objects.pop_front();

        QString type;
        try
        {
            InetAddr(AF_INET6, addr.toLatin1().constData() );
            type = IPv6::TYPENAME;
        } catch (FWException &ex)
        {
        }

        if (type.isEmpty())
        {
            try
            {
                InetAddr(AF_INET, addr.toLatin1().constData() );
                type = IPv4::TYPENAME;
            } catch (FWException &ex)
            {
            }
        }

        if (! type.isEmpty())
        {
            Address *obj = Address::cast(mw->createObject(type, name));
            assert(obj!=nullptr);
            obj->setName(name.toUtf8().constData());
            obj->setAddress(InetAddr(addr.toStdString()));
            obj->setNetmask(InetAddr(InetAddr::getAllOnes()));
            mw->moveObject(libraries[lib_index], obj);
            last_object = obj;
        }

        m_dialog->progressBar->setValue(counter);
        qApp->processEvents();

        counter++;
    }

    ProjectPanel *pp = mw->activeProject();
    QString filename = pp->getFileName();
    QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));
    QCoreApplication::postEvent(
        mw->activeProject(), new openLibraryForObjectEvent(
            filename, last_object->getId()));
}

