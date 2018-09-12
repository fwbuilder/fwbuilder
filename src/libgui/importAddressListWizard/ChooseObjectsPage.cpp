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

#include "ChooseObjectsPage.h"
#include "HostsFile.h"

#include "ObjectDescriptor.h"

#include <QMessageBox>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ChooseObjectsPage::ChooseObjectsPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ChooseObjectsPage_q;
    m_dialog->setupUi(this);

    registerField("objectsToUse*", m_dialog->objectSelector,
                  "objectsToUse", SIGNAL(selectionChanged()));
}

void ChooseObjectsPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ChooseObjectsPage::initializePage()"
                 << "file name" << field("fileName").toString();

    try
    {
        HostsFile importer(field("fileName").toString());
        importer.parse();
        map<InetAddr, QStringList> imported_hosts_info = importer.getAll();

        QList<ObjectDescriptor> objects;
        map<InetAddr, QStringList>::iterator i;
        for (i=imported_hosts_info.begin(); i!=imported_hosts_info.end(); ++i)
        {
            ObjectDescriptor od;
            od.addr = i->first;
            od.sysname = i->second.front().toUtf8().constData();
            objects.push_back(od);
        }

        m_dialog->objectSelector->init(objects);

    } catch (FWException &ex)
    {
        QMessageBox::critical( nullptr , "Firewall Builder",
                               ex.toString().c_str(),
                               QString::null,QString::null);
    }
}

bool ChooseObjectsPage::validatePage()
{
    if (fwbdebug) qDebug() << "ChooseObjectsPage::validatePage()";
    return true;
}

bool ChooseObjectsPage::isComplete() const
{
    if (fwbdebug) qDebug() << "ChooseObjectsPage::isComplete()";
    return (m_dialog->objectSelector->count() > 0);
}

