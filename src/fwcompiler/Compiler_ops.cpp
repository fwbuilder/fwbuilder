/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "Compiler.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"

#include <iostream>

using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;



/*
 *  Interface has both address and a netmask, however what we need here is only
 *  its address. If we were to use both, we would get the same behaviour as if
 *  it was network, not just an interface of a single host. 
 */
vector<FWObject*> fwcompiler::_find_obj_intersection(Address *op1, Address *op2)
{
    vector<FWObject*>  res;

    const InetAddr *addr1 = op1->getAddressPtr();
    const InetAddr *netm1 = op1->getNetmaskPtr();
    const InetAddr *addr2 = op2->getAddressPtr();
    const InetAddr *netm2 = op2->getNetmaskPtr();

    if (addr1==NULL || addr2==NULL) return res;

    const InetAddrMask n1( *addr1,
        (Interface::cast(op1)) ? InetAddr(InetAddr::getAllOnes()) : (*netm1) );
    const InetAddrMask n2( *addr2,
        (Interface::cast(op2)) ? InetAddr(InetAddr::getAllOnes()) : (*netm2) );

    vector<InetAddrMask> intersection = libfwbuilder::getOverlap(n1,n2);

    for (vector<InetAddrMask>::iterator i=intersection.begin(); i!=intersection.end(); i++)
    {
	InetAddrMask *n= &(*i);
	if (n->getNetmaskPtr()->isHostMask())
        {
            IPv4 *h = new IPv4();
            h->setAddress(*(n->getAddressPtr()));
	    h->setName("h-"+n->getAddressPtr()->toString());
            op1->getRoot()->add(h,false);
	    res.push_back(h);
	} else {
	    Network *net = new Network();
	    net->setAddress(*(n->getAddressPtr()));
	    net->setNetmask(*(n->getNetmaskPtr()));
	    net->setName("n-"+n->getAddressPtr()->toString());
            op1->getRoot()->add(net,false);
	    res.push_back(net);
	}
    }

    return res;
}

bool fwcompiler::_find_portrange_intersection(int rs1,int re1,
					    int rs2,int re2,
					    int &rsr,int &rer)
{
    if (re2<rs1) return false;
    if (rs2<rs1 && re2>rs1 && re2<re1) { rsr=rs1; rer=re2; return true; }
    if (rs2>rs1 && re2<re1) { rsr=rs2; rer=re2; return true; }
    if (rs2>rs1 && rs2<re1 && re2>re1) { rsr=rs2; rer=re1; return true; }
    if (rs2>re1) return false;
    if (rs2<rs1 && re2>re1) { rsr=rs1; rer=re1; return true; }
    if (rs1==rs2 && re1==re2) { rsr=rs1; rer=re1; return true; }
    return false;
}

vector<FWObject*> fwcompiler::_find_srv_intersection(Service *op1, Service *op2)
{
    vector<FWObject*> res;

    if (op1->getTypeName()==op2->getTypeName()) 
    {
	if ( IPService::cast(op1)) 
        {
/* TODO: check ip flags */
	    if (op1->getStr("protocol_num")==op2->getStr("protocol_num")) 
            {
		res.push_back(op1);
	    }
	    return res;
	}
	if ( ICMPService::cast(op1)) 
        {
	    int t1=op1->getInt("type");
	    int t2=op2->getInt("type");

	    if (t1!=-1 && t2!=-1 && t1==t2) { res.push_back(op1); return res; }
	    if (t1!=-1 && t2==-1) { res.push_back(op1); return res; }
	    if (t1==-1 && t2!=-1) { res.push_back(op2); return res; }
	    return res;
	}
	if ( TCPService::cast(op1) || UDPService::cast(op1) ) 
        {
	    int srs1=static_cast<const TCPUDPService*>(op1)->getSrcRangeStart();
	    int sre1=static_cast<const TCPUDPService*>(op1)->getSrcRangeEnd();
	    int drs1=static_cast<const TCPUDPService*>(op1)->getDstRangeStart();
	    int dre1=static_cast<const TCPUDPService*>(op1)->getDstRangeEnd();

	    int srs2=static_cast<const TCPUDPService*>(op2)->getSrcRangeStart();
	    int sre2=static_cast<const TCPUDPService*>(op2)->getSrcRangeEnd();
	    int drs2=static_cast<const TCPUDPService*>(op2)->getDstRangeStart();
	    int dre2=static_cast<const TCPUDPService*>(op2)->getDstRangeEnd();

	    int srsR,sreR,drsR,dreR;

	    if ( ! _find_portrange_intersection(srs1,sre1,srs2,sre2,srsR,sreR) ) return res;
	    if ( ! _find_portrange_intersection(drs1,dre1,drs2,dre2,drsR,dreR) ) return res;
		    
	    FWObject *nserv;
	    if ( TCPService::cast(op1) ) 
            {
		nserv=new TCPService();
                op1->getRoot()->add(nserv,false);
	    }
	    if ( UDPService::cast(op1) ) 
            {
		nserv=new UDPService();
                op1->getRoot()->add(nserv,false);
	    }

	    nserv->setName(op1->getName()+"-"+op2->getName());
            TCPUDPService::cast(nserv)->setSrcRangeStart(srsR);
            TCPUDPService::cast(nserv)->setSrcRangeEnd(sreR);
            TCPUDPService::cast(nserv)->setDstRangeStart(drsR);
            TCPUDPService::cast(nserv)->setDstRangeEnd(dreR);

	    res.push_back(nserv);
	    return res;
	}
    }
/*
 * if objects are of the different types, then the only case allowed is
 * when one object is IPService
 */
    if ( ! IPService::cast(op1) && IPService::cast(op2)) res.push_back(op1);
    if ( ! IPService::cast(op2) && IPService::cast(op1)) res.push_back(op2);
    return res;
}


bool fwcompiler::checkForShadowing(const Service &o1,const Service &o2)
{
    if (o1.getId()==o2.getId()) return true;
    if (o1.isAny() && o2.isAny()) return false;
    if ( ! o1.isAny() && o2.isAny()) return true;
    if ( o1.isAny() && !o2.isAny()) return false;

    if (o1.getTypeName()==o2.getTypeName()) 
    {
	if ( IPService::constcast(&o1)) 
        {
/* 
 * Both objects are IPService
 *
 * can't use Service::getProtocolNumber member because it is not
 * defined as 'const'. Can't redefine it either since that will break
 * binary compatibility (as of v2.0.4)
 */
	    if (o1.getStr("fragm")!=o2.getStr("fragm") ||
                o1.getStr("short_fragm")!=o2.getStr("short_fragm") ||
                o1.getStr("lsrr")!=o2.getStr("lsrr") ||
                o1.getStr("ssrr")!=o2.getStr("ssrr") ||
                o1.getStr("rr")!=o2.getStr("rr") ||
                o1.getStr("ts")!=o2.getStr("ts") ) return false;

	    if (o1.getInt("protocol_num")==o2.getInt("protocol_num")) return true;
	    if (o1.getInt("protocol_num")!=0 && o2.getInt("protocol_num")==0) return true;
            return false;
	}
	if ( ICMPService::constcast(&o1)) 
        {
	    if (o1.getInt("type")!=-1 && o2.getInt("type")==-1) return true;
	    else return false;
	}
	if ( TCPService::constcast(&o1) ) 
        {
            const TCPService *t1=TCPService::constcast(&o1);
            const TCPService *t2=TCPService::constcast(&o2);

/* it seems STL does not define operator!= for class std::set */
            if ( !( t1->getAllTCPFlags()     == t2->getAllTCPFlags() &&
                    t1->getAllTCPFlagMasks() == t2->getAllTCPFlagMasks() ) )
                return false;
        }
	if ( TCPService::constcast(&o1) || UDPService::constcast(&o1) ) 
        {
	    int srs1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeStart();
	    int sre1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeEnd();
	    int drs1=static_cast<const TCPUDPService*>(&o1)->getDstRangeStart();
	    int dre1=static_cast<const TCPUDPService*>(&o1)->getDstRangeEnd();

	    int srs2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeStart();
	    int sre2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeEnd();
	    int drs2=static_cast<const TCPUDPService*>(&o2)->getDstRangeStart();
	    int dre2=static_cast<const TCPUDPService*>(&o2)->getDstRangeEnd();

	    return (srs1>=srs2 && sre1<=sre2 && drs1>=drs2 && dre1<=dre2);
	}
	if ( TagService::constcast(&o1)) 
        {
            string tagvalue1 = TagService::constcast(&o1)->getCode();
            string tagvalue2 = TagService::constcast(&o2)->getCode();
            return (tagvalue1 == tagvalue2);
	}
	if ( UserService::constcast(&o1)) 
        {
            string uid1 = UserService::constcast(&o1)->getUserId();
            string uid2 = UserService::constcast(&o2)->getUserId();
            return (uid1 == uid2);
	}

        return false;
    }
/*
 * if objects are of the different types, then the only case allowed is
 * when one object is IPService
 */

    if (IPService::constcast(&o2) && ! IPService::constcast(&o1) )
    {
        /* if o2 is IP Service and o1 is not, then o2 only shades o1
         * when all flags are cleared in o2
         */
        return ( o2.getInt("protocol_num")==0 &&
                 o2.getStr("fragm")!="True" &&
                 o2.getStr("short_fragm")!="True" &&
                 o2.getStr("lsrr")!="True" &&
                 o2.getStr("ssrr")!="True" &&
                 o2.getStr("rr")!="True" &&
                 o2.getStr("ts")!="True");
    }

    return false;

    throw FWException("trying to compare objects of incompatible types: \
o1: "+o1.getName()+" ("+o1.getTypeName()+") o2: "+o2.getName()+" ("+o2.getTypeName()+")");
}

bool fwcompiler::checkForShadowing(const Address &o1,const Address &o2)
{
    if (o1.getId()==o2.getId()) return true;

    if (Interface::isA(&o1))
    {
        const Interface *intf=Interface::constcast(&o1);
        if (!intf->isRegular()) return false;
    }

    if (Interface::isA(&o2))
    {
        const Interface *intf=Interface::constcast(&o2);
        if (!intf->isRegular()) return false;
    }

    if (physAddress::isA(&o1) && physAddress::isA(&o2))
    {
        const physAddress *o1_pa =physAddress::constcast(&o1);
        const physAddress *o2_pa =physAddress::constcast(&o2);
        return o1_pa->getPhysAddress()==o2_pa->getPhysAddress();
    }
    
    const InetAddr *o1b;
    const InetAddr *o1e;
    const InetAddr *o2b;
    const InetAddr *o2e;

    if (AddressRange::isA(&o1))
    {
        o1b = &(AddressRange::constcast(&o1)->getRangeStart());
        o1e = &(AddressRange::constcast(&o1)->getRangeEnd());
    } else
    {
        if (Network::isA(&o1))
        {
            o1b = o1.getAddressPtr();
            o1e = o1.getBroadcastAddressPtr();
        } else
        {
            o1b = o1.getAddressPtr();
            o1e = o1.getAddressPtr();
        }
    }

    if (AddressRange::isA(&o2))
    {
        o2b = &(AddressRange::constcast(&o2)->getRangeStart());
        o2e = &(AddressRange::constcast(&o2)->getRangeEnd());
    } else
    {
        if (Network::isA(&o2))
        {
            o2b = o2.getAddressPtr();
            o2e = o2.getBroadcastAddressPtr();
        } else
        {
            o2b = o2.getAddressPtr();
            o2e = o2.getAddressPtr();
        }
    }

#if 0
    cerr << "# o1=" << o1.getName() << " [" << o1.toString() << "] "
         << o1b->toString() << "-" << o1e->toString()
         << "(" << o1.dimension() << ")"
         << " o2=" << o2.getName() << " [" << o2.toString() << "] "
         << o2b->toString() << "-" << o2e->toString()
         << "(" << o2.dimension() << ")"
         << " " << int( ((*o1b)>(*o2b) || (*o1b)==(*o2b)) &&
                        ((*o1e)<(*o2e) || (*o1e)==(*o2e)) )
         << endl;
#endif

    // if any of the objects has no ip address, then we can not
    // check for shadowing and return false. examples of objects with
    // no real ip address: physAddress, Interface with no child IPv4/IPv6
    // object. High level compilers should make sure they process rules
    // to the point where no such objects are left before they call
    // this method.
    if (o1b==NULL || o2b==NULL || o1e==NULL || o2e==NULL) return false;

    if (o1.isAny() && o2.isAny())  return true;
    if (o1.isAny() && !o2.isAny()) return false;
    if (!o1.isAny() && o2.isAny()) return true;

    return ( ((*o2b) < (*o1b) || (*o1b) == (*o2b)) &&
             ((*o1e) < (*o2e) || (*o1e) == (*o2e)) );
}

/*************************************************************************/




bool fwcompiler::operator==(const Service  &o1,const Service  &o2)
{
    if (o1.getId()==o2.getId()) return true;

    if (o1.getTypeName()==o2.getTypeName()) {
	if ( IPService::constcast(&o1)) {
	    return (o1.getInt("protocol_num")==o2.getInt("protocol_num") &&
                    o1.getStr("fragm")==o2.getStr("fragm") &&
                    o1.getStr("short_fragm")==o2.getStr("short_fragm") &&
                    o1.getStr("lsrr")==o2.getStr("lsrr") &&
                    o1.getStr("ssrr")==o2.getStr("ssrr") &&
                    o1.getStr("rr")==o2.getStr("rr") &&
                    o1.getStr("ts")==o2.getStr("ts") );
	}
	if ( ICMPService::constcast(&o1)) {
	    return (o1.getInt("type")==o2.getInt("type")) &&
		(o1.getInt("code")==o2.getInt("code"));
	}
	if ( UDPService::constcast(&o1) ) {
	    int srs1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeStart();
	    int sre1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeEnd();
	    int drs1=static_cast<const TCPUDPService*>(&o1)->getDstRangeStart();
	    int dre1=static_cast<const TCPUDPService*>(&o1)->getDstRangeEnd();

	    int srs2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeStart();
	    int sre2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeEnd();
	    int drs2=static_cast<const TCPUDPService*>(&o2)->getDstRangeStart();
	    int dre2=static_cast<const TCPUDPService*>(&o2)->getDstRangeEnd();

            return (srs1==srs2 && sre1==sre2 && drs1==drs2 && dre1==dre2);
	}

	if ( TCPService::constcast(&o1))
        {
	    int srs1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeStart();
	    int sre1=static_cast<const TCPUDPService*>(&o1)->getSrcRangeEnd();
	    int drs1=static_cast<const TCPUDPService*>(&o1)->getDstRangeStart();
	    int dre1=static_cast<const TCPUDPService*>(&o1)->getDstRangeEnd();

	    int srs2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeStart();
	    int sre2=static_cast<const TCPUDPService*>(&o2)->getSrcRangeEnd();
	    int drs2=static_cast<const TCPUDPService*>(&o2)->getDstRangeStart();
	    int dre2=static_cast<const TCPUDPService*>(&o2)->getDstRangeEnd();

            const TCPService *tcp1=TCPService::constcast(&o1);
            const TCPService *tcp2=TCPService::constcast(&o2);

            return ( tcp1->getAllTCPFlags()==tcp2->getAllTCPFlags() &&
                     tcp1->getAllTCPFlagMasks()==tcp2->getAllTCPFlagMasks() &&
                     srs1==srs2 && sre1==sre2 && drs1==drs2 && dre1==dre2
            );
	}
    }
    return false;
}

bool fwcompiler::operator==(const Address &o1,const Address &o2)
{
    if (o1.getId()==o2.getId()) return true;

    const InetAddr* o1b;
    const InetAddr* o1e;
    const InetAddr* o2b;
    const InetAddr* o2e;

    if (Interface::isA(&o1) && Interface::isA(&o2))
    {
        const Interface *i1=Interface::constcast(&o1);
        const Interface *i2=Interface::constcast(&o2);
        if ( !i1->isRegular() && !i2->isRegular() )
        {
            return i1->getName()==i2->getName();
        }
    }

    if (physAddress::isA(&o1) && physAddress::isA(&o2))
    {
        const physAddress *o1_pa =physAddress::constcast(&o1);
        const physAddress *o2_pa =physAddress::constcast(&o2);
        return o1_pa->getPhysAddress()==o2_pa->getPhysAddress();
    }
    
    if (AddressRange::isA(&o1))
    {
        o1b = &(AddressRange::constcast(&o1)->getRangeStart());  
        o1e = &(AddressRange::constcast(&o1)->getRangeEnd());
    } else
        if (Network::isA(&o1))
        {
            o1b = o1.getAddressPtr();
            o1e = o1.getBroadcastAddressPtr();
        } else
        {
            o1b = o1.getAddressPtr();
            o1e = o1.getAddressPtr();
        }

    if (AddressRange::isA(&o2))
    {
        o2b = &(AddressRange::constcast(&o2)->getRangeStart());  
        o2e = &(AddressRange::constcast(&o2)->getRangeEnd());
    } else
        if (Network::isA(&o2))
        {
            o2b = o2.getAddressPtr();
            o2e = o2.getBroadcastAddressPtr();
        } else
        {
            o2b = o2.getAddressPtr();
            o2e = o2.getAddressPtr();
        }

    if (o1b==NULL || o2b==NULL || o1e==NULL || o2e==NULL) return false;
    return ((*o1b) == (*o2b) && (*o1e) == (*o2e));
}

bool fwcompiler::operator==(const Interval  &o1,const Interval  &o2)
{
    if (o1.getId()==o2.getId()) return true;

    int smin1, shour1, sday1, smonth1, syear1, sdayofweek1;
    int emin1, ehour1, eday1, emonth1, eyear1, edayofweek1;

    int smin2, shour2, sday2, smonth2, syear2, sdayofweek2;
    int emin2, ehour2, eday2, emonth2, eyear2, edayofweek2;

    o1.getStartTime( &smin1, &shour1, &sday1, &smonth1, &syear1, &sdayofweek1);
    o1.getEndTime(   &emin1, &ehour1, &eday1, &emonth1, &eyear1, &edayofweek1);

    o2.getStartTime( &smin2, &shour2, &sday2, &smonth2, &syear2, &sdayofweek2);
    o2.getEndTime(   &emin2, &ehour2, &eday2, &emonth2, &eyear2, &edayofweek2);

    return (smin1==smin2             && emin1==emin2             &&
            shour1==shour2           && ehour1==ehour2           &&
            sday1==sday2             && eday1==eday2             &&
            smonth1==smonth2         && emonth1==emonth2         &&
            syear1==syear2           && eyear1==eyear2           &&
            sdayofweek1==sdayofweek2 && edayofweek1==edayofweek2);
}


/*************************************************************************

bool fwcompiler::operator<=(const Address &o1,const Address &o2)
{
    return (o1 == o2 || o1 < o2);
}

bool fwcompiler::operator<=(const Service  &o1,const Service  &o2)
{
    return (o1 == o2 || o1 < o2);
}

*/

/*************************************************************************/

bool Compiler::intersect(PolicyRule &r1, PolicyRule &r2)
{
    string act1=r1.getActionAsString();
    string act2=r2.getActionAsString();

    bool res_act;
    res_act=(act1=="Continue" || act2=="Continue" || act1==act2);
    if (res_act==false) return false;

    int iface1 = r1.getInterfaceId();
    int iface2 = r2.getInterfaceId();

    bool res_iface;
    res_iface=(iface1==-1 || iface2==-1 || iface1==iface2);
    if (res_iface==false) return false;

    vector<FWObject*> v1=_find_obj_intersection( getFirstSrc(&r1) , getFirstSrc(&r2) );
    if (v1.empty()) return false;

    vector<FWObject*> v2=_find_obj_intersection( getFirstDst(&r1) , getFirstDst(&r2) );
    if (v2.empty()) return false;

    vector<FWObject*> v3=_find_srv_intersection( getFirstSrv(&r1) , getFirstSrv(&r2) );
    if (v3.empty()) return false;

    return true;
}


void Compiler::getIntersection(PolicyRule &r1, PolicyRule &r2, PolicyRule &res)
{
    string act1=r1.getActionAsString();
    string act2=r2.getActionAsString();

/*
 * "CONTINUE" is "broad" action, so chose another one, whatever it is
 */
    if (act1=="Continue") res.setAction(act2);
    if (act2=="Continue") res.setAction(act1);

    string any_id;
//    FWObject *any;

    RuleElementSrc *nsrc=res.getSrc();
    nsrc->clearChildren();

    RuleElementDst *ndst=res.getDst();
    ndst->clearChildren();

    RuleElementSrv *nsrv=res.getSrv();
    nsrv->clearChildren();

    if (r1.getInterfaceId()!=r2.getInterfaceId()) return ;

    string lbl="'"+r1.getLabel()+"' & '"+r2.getLabel()+"'";
    res.setLabel(lbl);

    vector<FWObject*> v1=_find_obj_intersection( getFirstSrc(&r1) , getFirstSrc(&r2) );
    vector<FWObject*> v2=_find_obj_intersection( getFirstDst(&r1) , getFirstDst(&r2) );
    vector<FWObject*> v3=_find_srv_intersection( getFirstSrv(&r1) , getFirstSrv(&r2) );

    for (vector<FWObject*>::iterator i1=v1.begin(); i1!=v1.end(); ++i1)
	nsrc->addRef(*i1);

    for (vector<FWObject*>::iterator i2=v2.begin(); i2!=v2.end(); ++i2)
	ndst->addRef(*i2);

    for (vector<FWObject*>::iterator i3=v3.begin(); i3!=v3.end(); ++i3)
	nsrv->addRef(*i3);

}
    
