/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef __NEWFIREWALLDIALOG_H_
#define __NEWFIREWALLDIALOG_H_

#include <ui_newfirewalldialog_q.h>

#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/InterfaceData.h"

#include "fakeWizard.h"

#include <map>

namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
    class Firewall;
    class Interface;
    class Logger;
    class SNMPQuery;
};

class QTreeWidgetItem;
class QTimer;
class QTextEdit;

class newFirewallDialog : public QDialog, public FakeWizard
{
    Q_OBJECT;
    
    Ui::newFirewallDialog_q *m_dialog;

    libfwbuilder::FWObject *currentTemplate;
    libfwbuilder::Firewall *nfw;
    bool snmpPollCompleted;
    libfwbuilder::Logger *logger;
    libfwbuilder::SNMPQuery *q;
    QTimer *timer;
    libfwbuilder::FWObjectDatabase *db_orig;
    libfwbuilder::FWObjectDatabase *tmpldb;
    libfwbuilder::FWObjectDatabase *db_copy;
    libfwbuilder::FWObject *parent;
    std::map<QListWidgetItem*, libfwbuilder::FWObject*> templates;
    bool unloadTemplatesLib;
    bool getInterfacesBusy;
    QString discovered_platform;
    QString discovered_host_os;
    QString discovered_version;
    
    std::list<std::string>              possible_inside_interface_labels;
    std::list<std::string>              possible_outside_interface_labels;
    std::list<std::string>              possible_dmz_interface_labels;

    void fillInterfaceData(libfwbuilder::Interface *intf, QTextBrowser *qte);

    void fillInterfaceSLList();
    void fillInterfaceNZList();

    void getInterfaceDataFromInterfaceEditor(EditedInterfaceData &edata,
                                             libfwbuilder::InterfaceData &idata);
    
    void createFirewallFromTemplate();
    void changedAddressesInNewFirewall();

    void replaceInterfaceAttributes(libfwbuilder::Firewall *fw,
                                    libfwbuilder::Interface *intf,
                                    EditedInterfaceData *new_data);
    libfwbuilder::Address* replaceInterfaceAddressData(
        libfwbuilder::Firewall *fw,
        libfwbuilder::Interface *intf,
        libfwbuilder::Address *addr_obj,
        const QString &address,
        const QString &netmask,
        bool ipv4);
    void replaceReferencesToNetworks(libfwbuilder::Firewall *fw,
                                     libfwbuilder::Interface *intf,
                                     libfwbuilder::InetAddrMask old_net,
                                     libfwbuilder::InetAddrMask new_net);
    void replaceReferencesToObject(libfwbuilder::Firewall *fw,
                                   libfwbuilder::FWObject *old_obj,
                                   libfwbuilder::FWObject *new_obj);

public:
    newFirewallDialog(QWidget *parentw, libfwbuilder::FWObject *parent);
    virtual ~newFirewallDialog();

    libfwbuilder::Firewall* getNewFirewall() { return nfw; };

    virtual bool appropriate(const int page) const;
    
    void showPage(const int page); //it was "selected(QString)"
    bool validateAddressAndMask(const QString &addr,const QString &netm);
    void getIPAddressOfFirewallByName();
    
public slots:
    virtual void changed();
    virtual void getInterfacesViaSNMP();    
    virtual void monitor();
    virtual void templateSelected(QListWidgetItem *itm);
    void browseTemplate();
    void useStandardTemplate();
    void updateTemplatePanel();

protected slots:
    
    void finishClicked();
    void cancelClicked();
    void nextClicked();
    void backClicked();

    void cleanup();    
};

#endif // __NEWFIREWALLDIALOG_H
