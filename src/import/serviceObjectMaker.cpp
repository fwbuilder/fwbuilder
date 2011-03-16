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
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

#include "QStringListOperators.h"

#include <QStringList>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


ServiceObjectMaker::ServiceObjectMaker(Library *l) : ObjectMaker(l)
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
    ObjectSignature sig;
    sig.type_name = IPService::TYPENAME;
    sig.protocol = 0;
    sig.fragments = false;
    registerObject(sig, NULL);  // "any"
}

ServiceObjectMaker::~ServiceObjectMaker() {}

void ServiceObjectMaker::clear()
{
    ObjectMaker::clear();
}

FWObject* ServiceObjectMaker::getCustomService(const QString &platform,
                                               const QString &code,
                                               const QString &protocol)
{
    ObjectSignature sig;
    sig.type_name = CustomService::TYPENAME;
    sig.platform = platform;
    sig.protocol_name = protocol;
    sig.code = code;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QString custom_service_name_sig = platform + "-" + code;
    if (custom_service_codes.count(custom_service_name_sig) > 0)
        custom_service_code_tracker++;

    QStringList name;
    name << "cust-" << custom_service_code_tracker;
    if ( ! protocol.isEmpty()) name << "-" << protocol;

    CustomService *s = CustomService::cast(
        createObject(CustomService::TYPENAME, name.join("").toStdString()));
    if (!protocol.isEmpty()) s->setProtocol(protocol.toStdString());
    s->setCodeForPlatform(platform.toStdString(), code.toStdString());

    registerObject(sig, s);

    return s;
}

FWObject* ServiceObjectMaker::getIPService(int proto, bool fragments)
{
    ObjectSignature sig;
    sig.type_name = IPService::TYPENAME;
    sig.protocol = proto;
    sig.fragments = fragments;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QString name = QString("ip-%1").arg(proto);
    if (fragments) name += "-frag";
    IPService *s = IPService::cast(
        createObject(IPService::TYPENAME, name.toStdString()));
    s->setInt("protocol_num", proto);
    s->setBool("fragm", fragments);

    registerObject(sig, s);

    return s;
}

FWObject* ServiceObjectMaker::getICMPService(int type, int code)
{
    ObjectSignature sig;
    sig.type_name = ICMPService::TYPENAME;
    sig.icmp_type = type;
    sig.icmp_code = code;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QString name = QString("icmp %1/%2").arg(type).arg(code);

    ICMPService *s = ICMPService::cast(
        createObject(ICMPService::TYPENAME, name.toStdString()));
    s->setInt("type", type);
    s->setInt("code", code);

    registerObject(sig, s);

    return s;
}

FWObject* ServiceObjectMaker::getTCPService(int srs, int sre,
                                            int drs, int dre,
                                            bool established,
                                            QList<int> &flags_mask,
                                            QList<int> &flags_comp)
{
    ObjectSignature sig;
    sig.type_name = TCPService::TYPENAME;
    sig.src_port_range_start = srs;
    sig.src_port_range_end = sre;
    sig.dst_port_range_start = drs;
    sig.dst_port_range_end = dre;
    sig.established = established;
    sig.flags_mask = flags_mask;
    sig.flags_comp = flags_comp;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QStringList nl;

    nl << QString("tcp %1:%2 / %3:%4").arg(srs).arg(sre).arg(drs).arg(dre);
    if (established) nl << "est";
    if (!flags_mask.isEmpty() && !flags_comp.isEmpty())
    {
        QStringList f;
        foreach(int x, flags_mask) f << tcp_flag_names[x];
        f << "/";
        foreach(int x, flags_comp) f << tcp_flag_names[x];
        nl << f.join("");
    }

    TCPService* s = TCPService::cast(
        createObject(TCPService::TYPENAME, nl.join(" ").toStdString()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);


    if (!flags_mask.isEmpty() && !flags_comp.isEmpty())
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
    
    registerObject(sig, s);

    return s;
}

FWObject* ServiceObjectMaker::getUDPService(int srs, int sre, int drs, int dre)
{
    ObjectSignature sig;
    sig.type_name = UDPService::TYPENAME;
    sig.src_port_range_start = srs;
    sig.src_port_range_end = sre;
    sig.dst_port_range_start = drs;
    sig.dst_port_range_end = dre;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    QString name =  QString("udp %1:%2 / %3:%4")
        .arg(srs).arg(sre).arg(drs).arg(dre);

    UDPService* s = UDPService::cast(
        createObject(UDPService::TYPENAME, name.toStdString()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);

    registerObject(sig, s);

    return s;
}

FWObject* ServiceObjectMaker::getTagService(const QString &tagcode)
{
    ObjectSignature sig;
    sig.type_name = TagService::TYPENAME;
    sig.tag = tagcode;
    FWObject *obj = findMatchingObject(sig);
    if (obj) return obj;

    TagService *s = NULL;

    QString name = QString("tag-%1").arg(tagcode);

    s = TagService::cast(createObject(TagService::TYPENAME, name.toStdString()));
    assert(s!=NULL);
    s->setCode(tagcode.toStdString());

    registerObject(sig, s);

    return s;
}

