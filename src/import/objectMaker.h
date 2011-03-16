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

#include <QString>
#include <QMap>


namespace libfwbuilder
{
    class Library;
};


class ObjectMakerException : public std::exception
{
    QString err;
public:
    ObjectMakerException(const std::string &e) { err = e.c_str(); }
    ObjectMakerException(const QString &e) { err = e; }
    virtual ~ObjectMakerException() throw() {}
    QString toString() { return err; }
};


class ObjectSignature
{
public:
    QString type_name;

    // for address-like objects
    QString address;
    QString netmask;
    QString address_range_start;
    QString address_range_end;
    QString dns_name;
    QString address_table_name;

    // for services
    int protocol;
    bool fragments;
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

    QString toString() const;
};

class ObjectMaker
{
    libfwbuilder::Library *library;
    libfwbuilder::FWObject *last_created;

    QMap<QString, int> object_registry;
    
public:

    ObjectMaker(libfwbuilder::Library *l) { library = l; last_created = NULL; }
    virtual ~ObjectMaker() {};
    
    virtual void clear();

    libfwbuilder::FWObject* findMatchingObject(const ObjectSignature &sig);
    void registerObject(const ObjectSignature &sig, libfwbuilder::FWObject* obj);
    
    libfwbuilder::FWObject* createObject(const std::string &objType,
                                         const std::string &objName);

    libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                         const std::string &objType,
                                         const std::string &objName);

    
    void addStandardImportComment(libfwbuilder::FWObject *obj,
                                  const QString &file_name,
                                  int line_num);

    libfwbuilder::FWObject *getLastCreatedObject() { return last_created; }
    
};

#endif
