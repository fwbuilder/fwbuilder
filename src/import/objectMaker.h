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


#ifndef _OBJECT_MAKER_H_
#define _OBJECT_MAKER_H_

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Dispatch.h"

#include <QString>
#include <QMap>
#include <QPair>


namespace libfwbuilder
{
    class AddressRange;
    class Cluster;
    class CustomService;
    class Firewall;
    class Host;
    class ICMPService;
    class IPService;
    class IPv4;
    class IPv6;
    class Interface;
    class Library;
    class Network;
    class NetworkIPv6;
    class ObjectGroup;
    class ServiceGroup;
    class TCPService;
    class TagService;
    class UDPService;
    class physAddress;
};


class ObjectMakerException : public std::exception
{
    QString err;
public:
    ObjectMakerException(const std::string &e) { err = e.c_str(); }
    ObjectMakerException(const QString &e) { err = e; }
    virtual ~ObjectMakerException() throw() {}
    QString toString() { return err; }
    virtual const char* what() const throw() { return err.toStdString().c_str(); }
};


class ObjectSignature : public libfwbuilder::Dispatch
{
    static QMap<QString, QPair<int,int> > icmp_names;

public:
    ObjectSignature();
    ObjectSignature(const ObjectSignature &other);

    bool port_range_inclusive;
    
    QString type_name;
    QString object_name;

    // for address-like objects
    QString address;
    QString netmask;
    QString address_range_start;
    QString address_range_end;
    QString dns_name;
    QString address_table_name;

    // for IP service
    int protocol;
    bool fragments;
    bool short_fragments;
    bool any_opt;
    QString dscp;
    QString tos;
    bool lsrr;
    bool ssrr;
    bool rr;
    bool ts;
    bool rtralt;
    bool rtralt_value;
    
    // for ICMP service
    int icmp_type;
    int icmp_code;

    // for tcp and udp
    int src_port_range_start;
    int src_port_range_end;
    int dst_port_range_start;
    int dst_port_range_end;

    // tcp only
    bool established;
    QList<int> flags_mask;
    QList<int> flags_comp;

    // custom service
    QString platform;
    QString protocol_name;
    QString code;

    // tag service
    QString tag;

    QList<int> group_children_ids;

    // convenience methods that populate various attributes from
    // strings taken from imported configs
    void setAddress(const QString &s);
    void setNetmask(const QString &s, bool inverted_netmask=false);
    void setAddressRangeStart(const QString &s);
    void setAddressRangeEnd(const QString &s);
    void setProtocol(const QString &s);
    void setIcmpFromName(const QString &s);
    void setIcmpType(const QString &s);
    void setIcmpCode(const QString &s);

    int portFromString(const QString &port_spec, const QString &proto,
                       int default_port);
    
    void setSrcPortRange(const QString &range_start_spec,
                         const QString &range_end_spec,
                         const QString &proto);
    void setDstPortRange(const QString &range_start_spec,
                         const QString &range_end_spec,
                         const QString &proto);

    void setSrcPortRangeFromPortOp(const QString &port_op,
                                   const QString &port_spec,
                                   const QString &proto);
    void setDstPortRangeFromPortOp(const QString &port_op,
                                   const QString &port_spec,
                                   const QString &proto);

    QString toString() const;

    // The following methods build signature from given object taking
    // into account its type
    virtual void* dispatch(libfwbuilder::Network*, void*);
    virtual void* dispatch(libfwbuilder::NetworkIPv6*, void*);
    virtual void* dispatch(libfwbuilder::IPv4*, void*);
    virtual void* dispatch(libfwbuilder::IPv6*, void*);
    virtual void* dispatch(libfwbuilder::AddressRange*, void*);
    virtual void* dispatch(libfwbuilder::AddressTable*, void*);
    virtual void* dispatch(libfwbuilder::physAddress*, void*);
    virtual void* dispatch(libfwbuilder::IPService*, void*);
    virtual void* dispatch(libfwbuilder::ICMPService*, void*);
    virtual void* dispatch(libfwbuilder::ICMP6Service*, void*);
    virtual void* dispatch(libfwbuilder::TCPService*, void*);
    virtual void* dispatch(libfwbuilder::UDPService*, void*);
    virtual void* dispatch(libfwbuilder::CustomService*, void*);
    virtual void* dispatch(libfwbuilder::TagService*, void*);
    virtual void* dispatch(libfwbuilder::DNSName*, void*);
    virtual void* dispatch(libfwbuilder::ObjectGroup*, void*);
    virtual void* dispatch(libfwbuilder::ServiceGroup*, void*);

};

class ObjectMaker
{
protected:
    libfwbuilder::Library *library;
    libfwbuilder::FWObject *last_created;

    QMap<QString, int> named_object_registry;
    QMap<QString, int> anon_object_registry;

    libfwbuilder::FWObject* findMatchingObject(const ObjectSignature &sig);
    void registerNamedObject(const ObjectSignature &sig,
                             libfwbuilder::FWObject* obj);
    void registerAnonymousObject(const ObjectSignature &sig,
                                 libfwbuilder::FWObject* obj);
    
public:

    ObjectMaker(libfwbuilder::Library *l) { library = l; last_created = NULL; }
    virtual ~ObjectMaker() {};
    
    virtual void clear();

    virtual libfwbuilder::FWObject* createObject(ObjectSignature &sig);
    
    libfwbuilder::FWObject *getLastCreatedObject() { return last_created; }

    void prepareForDeduplication(libfwbuilder::FWObject *root);
    
    libfwbuilder::FWObject* createObject(const std::string &objType,
                                         const std::string &objName);

    libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                         const std::string &objType,
                                         const std::string &objName);

    libfwbuilder::FWObject*  promoteToNamedObject(libfwbuilder::FWObject *obj,
                                                  const std::string &objName);
};

#endif
