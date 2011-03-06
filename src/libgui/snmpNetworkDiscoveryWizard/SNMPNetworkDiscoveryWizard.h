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

#ifndef __SNMPNETWORKDISCOVERYWIZARD_H_
#define __SNMPNETWORKDISCOVERYWIZARD_H_

#include "ObjectDescriptor.h"

#include <QWizard>


class SNMPNetworkDiscoveryWizard : public QWizard
{
    Q_OBJECT;

    ObjectDescriptorList networks;
    ObjectDescriptorList objects;
    QStringList objectsToUse;
    
public:
    SNMPNetworkDiscoveryWizard(QWidget *parent);
    virtual ~SNMPNetworkDiscoveryWizard() {}

    ObjectDescriptorList* getNetworks() { return &networks; }
    ObjectDescriptorList* getObjects() { return &objects; }
    QStringList* getObjectsToUse() { return &objectsToUse; }
    
public slots:
        
};


#endif
