#ifndef LIBRARYMANIPULATOR_H
#define LIBRARYMANIPULATOR_H


#include <map>
#include <QString>
#include <QObject>
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"

class LibraryManipulator
{
public:
    LibraryManipulator();
    std::map<QString, QString>  systemGroupPaths;
    libfwbuilder::FWObject* getStandardSlotForObject(libfwbuilder::FWObject*, const QString &objType);
    libfwbuilder::FWObject* createNewLibrary(libfwbuilder::FWObjectDatabase*);
};

#endif // LIBRARYMANIPULATOR_H
