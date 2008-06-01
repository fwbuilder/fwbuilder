/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: newFirewallDialog.h,v 1.8 2006/06/16 04:33:13 vkurland Exp $

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

#include "../../config.h"
#include <ui_newfirewalldialog_q.h>

#include "InterfaceData.h"
#include "fakeWizard.h"

#include <map>

namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
    class Firewall;
    class Interface;
    class Logger;
    class SNMP_interface_query;
};

class QTreeWidgetItem;
class QTimer;
class QTextEdit;

class newFirewallDialog : public QDialog, public FakeWizard
{
    Q_OBJECT
    
    Ui::newFirewallDialog_q            *m_dialog;

    libfwbuilder::Firewall             *nfw;
    bool                                snmpPollCompleted;
    libfwbuilder::Logger               *logger;
    libfwbuilder::SNMP_interface_query *q;
    QTimer                             *timer;
    libfwbuilder::FWObjectDatabase     *tmpldb;
    std::map<QListWidgetItem*, libfwbuilder::FWObject*> templates;
    bool                                unloadTemplatesLib;
    bool                                getInterfacesBusy;
    
    void adjustSL(QTreeWidgetItem *itm1);
    void fillInterfaceData(libfwbuilder::Interface *intf, QTextBrowser *qte);
    void fillInterfaceSLList();

 public:
    newFirewallDialog();
    virtual ~newFirewallDialog();

    libfwbuilder::Firewall* getNewFirewall() { return nfw; };

    virtual bool appropriate(const int page) const;
    
    void showPage(const int page); //it was "selected(QString)"
    
public slots:
    virtual void addInterface();
    virtual void updateInterface();
    virtual void deleteInterface();
    virtual void upInterface();
    virtual void downInterface();
    virtual void changed();
    virtual void selectedInterface(QTreeWidgetItem*,QTreeWidgetItem*);
    virtual void getInterfacesViaSNMP();    
    virtual void monitor();
    virtual void templateSelected(QListWidgetItem *itm);
    void browseTemplate();
    void useStandartTemplate();
    void showHideTemplatePanel();
 protected slots:
    
    void finishClicked();
    void cancelClicked();
    void nextClicked();
    void backClicked();
 
};

#endif // __NEWFIREWALLDIALOG_H
