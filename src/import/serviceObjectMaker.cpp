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

#include "serviceObjectMaker.h"

#include "fwbuilder/CustomService.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ObjectMirror.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

// TODO: FWBTree needs to be refactored into an independent module
#include "../libgui/FWBTree.h"

#include "QStringListOperators.h"

#include <QStringList>
#include <QtDebug>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


ServiceObjectMaker::ServiceObjectMaker(Library *l, ObjectMakerErrorTracker *et) :
    ObjectMaker(l, et)
{
    custom_service_code_tracker = 0;
    tcp_flag_names[libfwbuilder::TCPService::URG] = "u";
    tcp_flag_names[libfwbuilder::TCPService::ACK] = "a";
    tcp_flag_names[libfwbuilder::TCPService::PSH] = "p";
    tcp_flag_names[libfwbuilder::TCPService::RST] = "r";
    tcp_flag_names[libfwbuilder::TCPService::SYN] = "s";
    tcp_flag_names[libfwbuilder::TCPService::FIN] = "f";
    tcp_flag_names[98]="N";   // NONE
    tcp_flag_names[99]="A";   // ALL

    // some "built-in" objects
    ObjectSignature sig(error_tracker);
    sig.type_name = IPService::TYPENAME;
    sig.protocol = 0;
    sig.fragments = false;
    registerAnonymousObject(sig, nullptr);  // "any"
}

ServiceObjectMaker::~ServiceObjectMaker() {}

void ServiceObjectMaker::clear()
{
    ObjectMaker::clear();
}

FWObject* ServiceObjectMaker::createObject(ObjectSignature &sig)
{
    assert( ! sig.type_name.isEmpty());

    FWObject *obj = findMatchingObject(sig);

    // qDebug() << "Create object " << sig.toString() << " obj=" << obj;

    if (obj) return obj;

    if (sig.type_name == CustomService::TYPENAME)
        obj = getCustomService(sig.platform, sig.code, sig.protocol_name);

    if (sig.type_name == IPService::TYPENAME)
    {
        QString name;
        if (sig.protocol > 0) name = QString("ip-%1").arg(sig.protocol);
        else name = "ip";
        if (sig.fragments) name += "-fragm";
        obj = ObjectMaker::createObject(IPService::TYPENAME, name.toStdString());
        obj->setInt("protocol_num", sig.protocol);
        obj->setBool("fragm", sig.fragments);
        obj->setBool("short_fragm", sig.short_fragments);
        obj->setBool("any_opt", sig.any_opt);
        obj->setStr("dscp", sig.dscp.toStdString());
        obj->setStr("tos", sig.tos.toStdString());
        obj->setBool("lsrr", sig.lsrr);
        obj->setBool("ssrr", sig.ssrr);
        obj->setBool("rr", sig.rr);
        obj->setBool("ts", sig.ts);
        obj->setBool("rtralt", sig.rtralt);
        obj->setBool("rtralt_value", sig.rtralt_value);
    }

    if (sig.type_name == ICMPService::TYPENAME)
    {
        QString name = QString("icmp %1/%2").arg(sig.icmp_type).arg(sig.icmp_code);
        obj = ObjectMaker::createObject(ICMPService::TYPENAME, name.toStdString());
        obj->setInt("type", sig.icmp_type);
        obj->setInt("code", sig.icmp_code);
    }

    if (sig.type_name == TCPService::TYPENAME)
        obj = getTCPService(sig.src_port_range_start, sig.src_port_range_end,
                            sig.dst_port_range_start, sig.dst_port_range_end,
                            sig.established,
                            sig.flags_mask,
                            sig.flags_comp);

    if (sig.type_name == UDPService::TYPENAME)
        obj = getUDPService(sig.src_port_range_start, sig.src_port_range_end,
                            sig.dst_port_range_start, sig.dst_port_range_end);

    if (sig.type_name == TagService::TYPENAME)
        obj = getTagService(sig.tag);

    if ( ! sig.object_name.isEmpty())
    {
        obj->setName(sig.object_name.toUtf8().constData());
        registerNamedObject(sig, obj);
    } else
        registerAnonymousObject(sig, obj);

    return obj;
}

FWObject* ServiceObjectMaker::getCustomService(const QString &platform,
                                               const QString &code,
                                               const QString &protocol)
{
    QString custom_service_name_sig = platform + "-" + code;
    if (custom_service_codes.count(custom_service_name_sig) > 0)
        custom_service_code_tracker++;

    QStringList name;
    name << "cust-" << custom_service_code_tracker;
    if ( ! protocol.isEmpty()) name << "-" << protocol;

    CustomService *s = CustomService::cast(
        ObjectMaker::createObject(CustomService::TYPENAME, name.join("").toStdString()));
    if (!protocol.isEmpty()) s->setProtocol(protocol.toStdString());
    s->setCodeForPlatform(platform.toStdString(), code.toStdString());
    return s;
}

FWObject* ServiceObjectMaker::getTCPService(int srs, int sre,
                                            int drs, int dre,
                                            bool established,
                                            QList<int> &flags_mask,
                                            QList<int> &flags_comp)
{
    QStringList nl;

    nl << QString("tcp %1:%2 / %3:%4").arg(srs).arg(sre).arg(drs).arg(dre);
    if (established) nl << "est";
    if (!flags_mask.isEmpty() || !flags_comp.isEmpty())
    {
        QStringList f;
        foreach(int x, flags_comp) f << tcp_flag_names[x];
        f << "/";
        foreach(int x, flags_mask) f << tcp_flag_names[x];
        nl << f.join("");
    }

    TCPService* s = TCPService::cast(
        ObjectMaker::createObject(TCPService::TYPENAME, nl.join(" ").toStdString()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);


    if (!flags_mask.isEmpty() || !flags_comp.isEmpty())
    {
        // TCP flags
        foreach(int x, flags_mask)
        {
            switch (x)
            {
            case 99:    // ALL
                s->setAllTCPFlagMasks();
                break;
            case 98:    // NONE
                s->clearAllTCPFlagMasks();
                break;
            default:
                s->setTCPFlagMask( TCPService::TCPFlag(x), true);
            }
        }
        foreach(int x, flags_comp)
        {
            switch (x)
            {
            case 99:    // ALL
                 s->setTCPFlag( TCPService::URG, true);
                 s->setTCPFlag( TCPService::ACK, true);
                 s->setTCPFlag( TCPService::PSH, true);
                 s->setTCPFlag( TCPService::RST, true);
                 s->setTCPFlag( TCPService::SYN, true);
                 s->setTCPFlag( TCPService::FIN, true);
                break;
            case 98:    // NONE
                s->clearAllTCPFlags();
                break;
            default:
                 s->setTCPFlag( TCPService::TCPFlag(x), true);
            }
        }
    }

    s->setEstablished(established);
    
    return s;
}

FWObject* ServiceObjectMaker::getUDPService(int srs, int sre, int drs, int dre)
{
    QString name =  QString("udp %1:%2 / %3:%4")
        .arg(srs).arg(sre).arg(drs).arg(dre);

    UDPService* s = UDPService::cast(
        ObjectMaker::createObject(UDPService::TYPENAME, name.toStdString()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);

    return s;
}

FWObject* ServiceObjectMaker::getTagService(const QString &tagcode)
{
    TagService *s = nullptr;

    QString name = QString("tag-%1").arg(tagcode);

    s = TagService::cast(
        ObjectMaker::createObject(TagService::TYPENAME, name.toStdString()));
    assert(s!=nullptr);
    s->setCode(tagcode.toStdString());

    return s;
}

FWObject* ServiceObjectMaker::getMirroredServiceObject(FWObject *obj)
{
    string new_name = obj->getName() + "-mirror";
    QString qs_new_name = QString::fromUtf8(new_name.c_str());
    FWObject *new_obj = nullptr;
    if (TCPService::isA(obj) || UDPService::isA(obj))
    {
        ObjectMirror mirror;
        new_obj = mirror.getMirroredService(Service::cast(obj));
        if (new_obj!=nullptr)
        {
            if (TCPService::isA(new_obj))
                TCPService::cast(new_obj)->setEstablished(false);

            ObjectSignature sig(error_tracker);
            new_obj->dispatch(&sig, (void*)(nullptr));
            sig.object_name = "";

            FWObject *matching_obj = findMatchingObject(sig);
            if (matching_obj)
            {
                delete new_obj;
                return matching_obj;
            }

            registerAnonymousObject(sig, new_obj);

            new_obj->setName(new_name);

            // obj may belong to the standard objects library if it was
            // deduplicated before
            FWObject *parent = obj->getParent();
            if (parent->isReadOnly())
            {
                FWBTree tree ;
                FWObject *slot = tree.getStandardSlotForObject(
                    library, new_obj->getTypeName().c_str());
                slot->add(new_obj);
            } else parent->add(new_obj);
        }
    }
    return new_obj;
}

