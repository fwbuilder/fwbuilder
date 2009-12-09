#include "LibraryManipulator.h"

using namespace std;
using namespace libfwbuilder;

LibraryManipulator::LibraryManipulator()
{
    systemGroupPaths[Library::TYPENAME]       = "";

    systemGroupPaths[IPv4::TYPENAME]          = "Objects/Addresses";
    systemGroupPaths[IPv6::TYPENAME]          = "Objects/Addresses";
    systemGroupPaths[DNSName::TYPENAME]       = "Objects/DNS Names";
    systemGroupPaths[AddressTable::TYPENAME]  = "Objects/Address Tables";
    systemGroupPaths[AddressRange::TYPENAME]  = "Objects/Address Ranges";
    systemGroupPaths[ObjectGroup::TYPENAME]   = "Objects/Groups";
    systemGroupPaths[Host::TYPENAME]          = "Objects/Hosts";
    systemGroupPaths[Network::TYPENAME]       = "Objects/Networks";
    systemGroupPaths[NetworkIPv6::TYPENAME]   = "Objects/Networks";

    systemGroupPaths[ServiceGroup::TYPENAME]  = "Services/Groups";
    systemGroupPaths[CustomService::TYPENAME] = "Services/Custom";
    systemGroupPaths[IPService::TYPENAME]     = "Services/IP";
    systemGroupPaths[ICMPService::TYPENAME]   = "Services/ICMP";
    systemGroupPaths[ICMP6Service::TYPENAME]  = "Services/ICMP";
    systemGroupPaths[TCPService::TYPENAME]    = "Services/TCP";
    systemGroupPaths[UDPService::TYPENAME]    = "Services/UDP";
    systemGroupPaths[UserService::TYPENAME]   = "Services/Users";

    systemGroupPaths[TagService::TYPENAME]    = "Services/TagServices";

    systemGroupPaths[Firewall::TYPENAME]      = "Firewalls";
    systemGroupPaths[Cluster::TYPENAME]       = "Clusters";

    systemGroupPaths[Interval::TYPENAME]      = "Time";
}

FWObject* LibraryManipulator::createNewLibrary(FWObjectDatabase *db)
{

    FWObject *nlib = db->create(Library::TYPENAME);
    db->add(nlib);
    nlib->setName( string(QObject::tr("New Library").toUtf8()) );

    FWObject *o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Objects");
    nlib->add(o1);

    FWObject *o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Addresses");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("DNS Names");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Address Tables");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Groups");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Hosts");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Networks");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Address Ranges");
    o1->add(o2);

    o1 = db->create(ServiceGroup::TYPENAME);
    o1->setName("Services");
    nlib->add(o1);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Groups");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("ICMP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("IP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("TCP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("UDP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Users");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Custom");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("TagServices");
    o1->add(o2);

    o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Firewalls");
    nlib->add(o1);

    o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Clusters");
    nlib->add(o1);

    o1 = db->create(IntervalGroup::TYPENAME);
    o1->setName("Time");
    nlib->add(o1);

    return nlib;
}

FWObject* LibraryManipulator::getStandardSlotForObject(FWObject* lib,const QString &objType)
{
    QString path = systemGroupPaths[objType];

    if (path.isEmpty()) return lib;

    QString level1 = path.section('/',0,0);
    QString level2 = path.section('/',1,1);

    FWObject::iterator i=std::find_if(lib->begin(),lib->end(),
        FWObjectNameEQPredicate(static_cast<const char*>(level1.toAscii())));
    if (i==lib->end()) return NULL;
    FWObject *l1obj = *i;
    if (level2.isEmpty()) return l1obj;

    i=std::find_if(l1obj->begin(),l1obj->end(),
        FWObjectNameEQPredicate(static_cast<const char*>(level2.toAscii())));
    if (i==l1obj->end()) return NULL;
    return (*i);
}
