/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/UDPService.h>
#include <fwbuilder/XMLTools.h>

using namespace libfwbuilder;
using namespace std;

const char *UDPService::TYPENAME={"UDPService"};

UDPService::UDPService()  
{
    setInt("src_range_start", 0);
    setInt("src_range_end",   0);
    setInt("dst_range_start", 0);
    setInt("dst_range_end",   0);
}

UDPService::UDPService(const FWObject *root,bool prepopulate) : Service(root,prepopulate) 
{
    setInt("src_range_start", 0);
    setInt("src_range_end",   0);
    setInt("dst_range_start", 0);
    setInt("dst_range_end",   0);
}

UDPService::~UDPService() {}

string UDPService::getProtocolName()   {    return "udp"; }
int    UDPService::getProtocolNumber() {    return 17;    }

void UDPService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("src_range_start")));
    if(n!=NULL)
    {
        setStr("src_range_start", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("src_range_end")));
    if(n!=NULL)
    {
        setStr("src_range_end", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dst_range_start")));
    if(n!=NULL)
    {
        setStr("dst_range_start", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dst_range_end")));
    if(n!=NULL)
    {
        setStr("dst_range_end", n);
        FREEXMLBUFF(n);
    }

}









