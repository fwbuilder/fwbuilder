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




#include "fwbuilder/TCPService.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;
using namespace std;

const char *TCPService::TYPENAME={"TCPService"};

std::map<TCPService::TCPFlag,std::string> TCPService::flags;
std::map<TCPService::TCPFlag,std::string> TCPService::flags_masks;

TCPService::TCPService()  
{
    _init_flags();

    clearAllTCPFlags();
    clearAllTCPFlagMasks();
}

TCPService::~TCPService() {}

string TCPService::getProtocolName() const   {    return "tcp";}
int    TCPService::getProtocolNumber() const {    return 6;    }

void TCPService::_init_flags()
{
    if (flags.empty())
    {
        flags[URG]="urg_flag"; 
        flags[ACK]="ack_flag"; 
        flags[PSH]="psh_flag"; 
        flags[RST]="rst_flag"; 
        flags[SYN]="syn_flag"; 
        flags[FIN]="fin_flag"; 
    }

    if (flags_masks.empty())
    {
        flags_masks[URG]="urg_flag_mask"; 
        flags_masks[ACK]="ack_flag_mask"; 
        flags_masks[PSH]="psh_flag_mask"; 
        flags_masks[RST]="rst_flag_mask"; 
        flags_masks[SYN]="syn_flag_mask"; 
        flags_masks[FIN]="fin_flag_mask"; 
    }
}

void TCPService::fromXML(xmlNodePtr root)
{
    TCPUDPService::fromXML(root);

    const char *n;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("established")));
    if(n!=nullptr)
    {
        setStr("established", n);
        XMLTools::FreeXmlBuff(n);
    }

    std::map<TCPFlag,std::string>::iterator i;
    for (i=flags.begin(); i!=flags.end(); ++i)
    {
        n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast( (i->second).c_str() )));
        if(n!=nullptr)
        {
            setStr( i->second , n);
            XMLTools::FreeXmlBuff(n);
        }
    }

    for (i=flags_masks.begin(); i!=flags_masks.end(); ++i)
    {
        n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast( (i->second).c_str() )));
        if(n!=nullptr)
        {
            setStr( i->second , n);
            XMLTools::FreeXmlBuff(n);
        }
    }

}

xmlNodePtr TCPService::toXML(xmlNodePtr xml_parent_node)
{
    xmlNodePtr me = TCPUDPService::toXML(xml_parent_node);
   
    return me;
}


bool  TCPService::getEstablished()
{
    return getBool("established");
}

void  TCPService::setEstablished(bool f)
{
    return setBool("established", f);
}

bool  TCPService::inspectFlags() const 
{ 
    return ( ! getAllTCPFlagMasks().empty() );
}


bool    TCPService::getTCPFlag(TCPFlag fl) const  
{ 
    return getBool( flags[fl] ); 
}

std::set<TCPService::TCPFlag> TCPService::getAllTCPFlags() const
{
    std::set<TCPService::TCPFlag> res;

    std::map<TCPFlag,std::string>::iterator i;
    for (i=flags.begin(); i!=flags.end(); ++i)
    {
        TCPFlag fl= i->first;
        if (getTCPFlag( fl ))  res.insert( fl );
    }
    return res;
}

void    TCPService::setTCPFlag(TCPFlag fl,bool v) 
{ 
    setBool( flags[fl] , v );    
}

bool    TCPService::getTCPFlagMask(TCPFlag fl) const  
{ 
    return getBool( flags_masks[fl] ); 
}

std::set<TCPService::TCPFlag> TCPService::getAllTCPFlagMasks() const
{
    std::set<TCPService::TCPFlag> res;

    std::map<TCPFlag,std::string>::iterator i;
    for (i=flags_masks.begin(); i!=flags_masks.end(); ++i)
    {
        TCPFlag fl= i->first;
        if (getTCPFlagMask( fl ))  res.insert( fl );
    }
    return res;
}

void    TCPService::setTCPFlagMask(TCPFlag fl,bool v) 
{ 
    setBool( flags_masks[fl] , v );    
}

void    TCPService::clearAllTCPFlags() 
{ 
    setBool( flags[URG] , false );    
    setBool( flags[ACK] , false );    
    setBool( flags[PSH] , false );    
    setBool( flags[RST] , false );    
    setBool( flags[SYN] , false );    
    setBool( flags[FIN] , false );    
}


void    TCPService::clearAllTCPFlagMasks() 
{ 
    setBool( flags_masks[URG] , false );    
    setBool( flags_masks[ACK] , false );    
    setBool( flags_masks[PSH] , false );    
    setBool( flags_masks[RST] , false );    
    setBool( flags_masks[SYN] , false );    
    setBool( flags_masks[FIN] , false );    
}

void   TCPService::setAllTCPFlagMasks()
{
    setBool( flags_masks[URG] , true );    
    setBool( flags_masks[ACK] , true );    
    setBool( flags_masks[PSH] , true );    
    setBool( flags_masks[RST] , true );    
    setBool( flags_masks[SYN] , true );    
    setBool( flags_masks[FIN] , true );    
}







