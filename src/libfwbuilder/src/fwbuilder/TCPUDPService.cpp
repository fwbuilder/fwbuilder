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


#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


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

void TCPUDPService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("src_range_start")));
    if(n!=NULL)
    {
        src_range_start = atol(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("src_range_end")));
    if(n!=NULL)
    {
        src_range_end = atol(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dst_range_start")));
    if(n!=NULL)
    {
        dst_range_start = atol(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dst_range_end")));
    if(n!=NULL)
    {
        dst_range_end = atol(n);
        FREEXMLBUFF(n);
    }

}

xmlNodePtr TCPUDPService::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    char str[128];

    snprintf(str, sizeof(str), "%d", src_range_start);
    xmlNewProp(me, TOXMLCAST("src_range_start"), TOXMLCAST(str));
    snprintf(str, sizeof(str), "%d", src_range_end);
    xmlNewProp(me, TOXMLCAST("src_range_end"), TOXMLCAST(str));

    snprintf(str, sizeof(str), "%d", dst_range_start);
    xmlNewProp(me, TOXMLCAST("dst_range_start"), TOXMLCAST(str));
    snprintf(str, sizeof(str), "%d", dst_range_end);
    xmlNewProp(me, TOXMLCAST("dst_range_end"), TOXMLCAST(str));
   
    return me;
}

FWObject& TCPUDPService::shallowDuplicate(const FWObject *obj,
                                          bool preserve_id)
    throw(FWException)
{
    const TCPUDPService *other = TCPUDPService::constcast(obj);
    src_range_start = other->src_range_start;
    src_range_end = other->src_range_end;
    dst_range_start = other->dst_range_start;
    dst_range_end = other->dst_range_end;
    return FWObject::shallowDuplicate(obj, preserve_id);
}

bool TCPUDPService::cmp(const FWObject *obj, bool recursive) throw(FWException)
{
    const TCPUDPService *other = TCPUDPService::constcast(obj);
    if (other == NULL) return false;
    if (src_range_start != other->src_range_start ||
        src_range_end != other->src_range_end ||
        dst_range_start != other->dst_range_start ||
        dst_range_end != other->dst_range_end) return false;
    return FWObject::cmp(obj, recursive);
}



