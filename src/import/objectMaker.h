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

#include "objectSignature.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>


namespace libfwbuilder
{
    class AddressRange;
    class AttachedNetworks;
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
    class UserService;
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


class ObjectMakerErrorTracker
{
    QStringList errors;
    bool error_status;
    QStringList warnings;
    bool warning_status;
    
public:
    ObjectMakerErrorTracker() { error_status = false; warning_status = false; }

    void clear() { error_status = false; warning_status = false; errors.clear(); warnings.clear(); }
    
    void registerError(const QString &msg);
    void registerError(const std::string &msg);
    bool hasErrors() { return error_status; }
    QStringList getErrors() { return errors; }

    void registerWarning(const QString &msg);
    void registerWarning(const std::string &msg);
    bool hasWarnings() { return warning_status; }
    QStringList getWarnings() { return warnings; }
};


class ObjectMaker
{
protected:

    ObjectMakerErrorTracker *error_tracker;
    
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

    ObjectMaker(libfwbuilder::Library *l, ObjectMakerErrorTracker *et)
    {
        library = l;
        error_tracker = et;
        last_created = nullptr;
    }
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
