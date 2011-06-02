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


#ifndef _ADDRESS_OBJECT_MAKER_H_
#define _ADDRESS_OBJECT_MAKER_H_

#include "objectMaker.h"

#include <QString>


class AddressObjectMaker : public ObjectMaker
{
    bool inverted_netmasks;
    
public:
    
    AddressObjectMaker(libfwbuilder::Library *l,
                       ObjectMakerErrorTracker *et) : ObjectMaker(l, et)
    {
        inverted_netmasks = false;
    }
    virtual ~AddressObjectMaker();

    void setInvertedNetmasks(bool f) { inverted_netmasks = f; }
    bool getInvertedNetmasks() { return inverted_netmasks; }
    
    virtual libfwbuilder::FWObject* createObject(ObjectSignature &sig);

protected:
    virtual libfwbuilder::FWObject* createAddress(ObjectSignature &sig);
    virtual libfwbuilder::FWObject* createAddressRange(ObjectSignature &sig);
    virtual libfwbuilder::FWObject* createAddressTable(ObjectSignature &sig);
    virtual libfwbuilder::FWObject* createDNSName(ObjectSignature &sig);

};

#endif
