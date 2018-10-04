/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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




#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;
using namespace std;


const char *TCPUDPService::TYPENAME={"TCPUDPService"};

TCPUDPService::TCPUDPService()  
{
    src_range_start = 0;
    src_range_end = 0;
    dst_range_start = 0;
    dst_range_end = 0;
}

TCPUDPService::~TCPUDPService() {}

string TCPUDPService::getProtocolName() const   {    return ""; }
int    TCPUDPService::getProtocolNumber() const {    return -1;    }

void TCPUDPService::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("src_range_start")));
    if(n!=nullptr)
    {
        src_range_start = atol(n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("src_range_end")));
    if(n!=nullptr)
    {
        src_range_end = atol(n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dst_range_start")));
    if(n!=nullptr)
    {
        dst_range_start = atol(n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dst_range_end")));
    if(n!=nullptr)
    {
        dst_range_end = atol(n);
        XMLTools::FreeXmlBuff(n);
    }

}

xmlNodePtr TCPUDPService::toXML(xmlNodePtr xml_parent_node)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    char str[128];

    snprintf(str, sizeof(str), "%d", src_range_start);
    xmlNewProp(me, XMLTools::ToXmlCast("src_range_start"), XMLTools::ToXmlCast(str));
    snprintf(str, sizeof(str), "%d", src_range_end);
    xmlNewProp(me, XMLTools::ToXmlCast("src_range_end"), XMLTools::ToXmlCast(str));

    snprintf(str, sizeof(str), "%d", dst_range_start);
    xmlNewProp(me, XMLTools::ToXmlCast("dst_range_start"), XMLTools::ToXmlCast(str));
    snprintf(str, sizeof(str), "%d", dst_range_end);
    xmlNewProp(me, XMLTools::ToXmlCast("dst_range_end"), XMLTools::ToXmlCast(str));
   
    return me;
}

FWObject& TCPUDPService::shallowDuplicate(const FWObject *obj,
                                          bool preserve_id)
{
    const TCPUDPService *other = TCPUDPService::constcast(obj);
    src_range_start = other->src_range_start;
    src_range_end = other->src_range_end;
    dst_range_start = other->dst_range_start;
    dst_range_end = other->dst_range_end;
    return FWObject::shallowDuplicate(obj, preserve_id);
}

bool TCPUDPService::cmp(const FWObject *obj, bool recursive)
{
    const TCPUDPService *other = TCPUDPService::constcast(obj);
    if (other == nullptr) return false;
    if (src_range_start != other->src_range_start ||
        src_range_end != other->src_range_end ||
        dst_range_start != other->dst_range_start ||
        dst_range_end != other->dst_range_end) return false;
    return FWObject::cmp(obj, recursive);
}



