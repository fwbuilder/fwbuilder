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


#include "getServByName.h"

#ifndef _WIN32
#  include <netdb.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif


QMap<QString, QMap<QString,int> > GetServByName::ports;

int GetServByName::getPortByName(const QString &name, const QString &proto)
{
    if (ports.size() == 0)
    {

        ports["tcp"]["tcpmux"] = 1;
        ports["tcp"]["echo"] = 7;
        ports["tcp"]["discard"] = 9;
        ports["tcp"]["systat"] = 11;
        ports["tcp"]["daytime"] = 13;
        ports["tcp"]["netstat"] = 15;
        ports["tcp"]["qotd"] = 17;
        ports["tcp"]["msp"] = 18;
        ports["tcp"]["chargen"] = 19;
        ports["tcp"]["ftp-data"] = 20;
        ports["tcp"]["ftp"] = 21;
        ports["tcp"]["ssh"] = 22;
        ports["tcp"]["telnet"] = 23;
        ports["tcp"]["smtp"] = 25;
        ports["tcp"]["time"] = 37;
        ports["tcp"]["nameserver"] = 42;
        ports["tcp"]["whois"] = 43;
        ports["tcp"]["tacacs"] = 49;
        ports["tcp"]["re-mail-ck"] = 50;
        ports["tcp"]["domain"] = 53;
        ports["tcp"]["mtp"] = 57;
        ports["tcp"]["tacacs-ds"] = 65;
        ports["tcp"]["bootps"] = 67;
        ports["tcp"]["bootpc"] = 68;
        ports["tcp"]["gopher"] = 70;
        ports["tcp"]["rje"] = 77;
        ports["tcp"]["finger"] = 79;
        ports["tcp"]["www"] = 80;
        ports["tcp"]["link"] = 87;
        ports["tcp"]["kerberos"] = 88;
        ports["tcp"]["supdup"] = 95;
        ports["tcp"]["hostnames"] = 101;
        ports["tcp"]["iso-tsap"] = 102;
        ports["tcp"]["acr-nema"] = 104;
        ports["tcp"]["csnet-ns"] = 105;
        ports["tcp"]["rtelnet"] = 107;
        ports["tcp"]["pop2"] = 109;
        ports["tcp"]["pop3"] = 110;
        ports["tcp"]["sunrpc"] = 111;
        ports["tcp"]["auth"] = 113;
        ports["tcp"]["sftp"] = 115;
        ports["tcp"]["uucp-path"] = 117;
        ports["tcp"]["nntp"] = 119;
        ports["tcp"]["ntp"] = 123;
        ports["tcp"]["pwdgen"] = 129;
        ports["tcp"]["loc-srv"] = 135;
        ports["tcp"]["netbios-ns"] = 137;
        ports["tcp"]["netbios-dgm"] = 138;
        ports["tcp"]["netbios-ssn"] = 139;
        ports["tcp"]["imap2"] = 143;
        ports["tcp"]["snmp"] = 161;
        ports["tcp"]["snmp-trap"] = 162;
        ports["tcp"]["cmip-man"] = 163;
        ports["tcp"]["cmip-agent"] = 164;
        ports["tcp"]["mailq"] = 174;
        ports["tcp"]["xdmcp"] = 177;
        ports["tcp"]["nextstep"] = 178;
        ports["tcp"]["bgp"] = 179;
        ports["tcp"]["prospero"] = 191;
        ports["tcp"]["irc"] = 194;
        ports["tcp"]["smux"] = 199;
        ports["tcp"]["at-rtmp"] = 201;
        ports["tcp"]["at-nbp"] = 202;
        ports["tcp"]["at-echo"] = 204;
        ports["tcp"]["at-zis"] = 206;
        ports["tcp"]["qmtp"] = 209;
        ports["tcp"]["z3950"] = 210;
        ports["tcp"]["ipx"] = 213;
        ports["tcp"]["imap3"] = 220;
        ports["tcp"]["pawserv"] = 345;
        ports["tcp"]["zserv"] = 346;
        ports["tcp"]["fatserv"] = 347;
        ports["tcp"]["rpc2portmap"] = 369;
        ports["tcp"]["codaauth2"] = 370;
        ports["tcp"]["clearcase"] = 371;
        ports["tcp"]["ulistserv"] = 372;
        ports["tcp"]["ldap"] = 389;
        ports["tcp"]["imsp"] = 406;
        ports["tcp"]["https"] = 443;
        ports["tcp"]["snpp"] = 444;
        ports["tcp"]["microsoft-ds"] = 445;
        ports["tcp"]["kpasswd"] = 464;
        ports["tcp"]["saft"] = 487;
        ports["tcp"]["isakmp"] = 500;
        ports["tcp"]["rtsp"] = 554;
        ports["tcp"]["nqs"] = 607;
        ports["tcp"]["npmp-local"] = 610;
        ports["tcp"]["npmp-gui"] = 611;
        ports["tcp"]["hmmp-ind"] = 612;
        ports["tcp"]["qmqp"] = 628;
        ports["tcp"]["ipp"] = 631;
        ports["tcp"]["exec"] = 512;
        ports["tcp"]["login"] = 513;
        ports["tcp"]["shell"] = 514;
        ports["tcp"]["printer"] = 515;
        ports["tcp"]["tempo"] = 526;
        ports["tcp"]["courier"] = 530;
        ports["tcp"]["conference"] = 531;
        ports["tcp"]["netnews"] = 532;
        ports["tcp"]["gdomap"] = 538;
        ports["tcp"]["uucp"] = 540;
        ports["tcp"]["klogin"] = 543;
        ports["tcp"]["kshell"] = 544;
        ports["tcp"]["afpovertcp"] = 548;
        ports["tcp"]["remotefs"] = 556;
        ports["tcp"]["nntps"] = 563;
        ports["tcp"]["submission"] = 587;
        ports["tcp"]["ldaps"] = 636;
        ports["tcp"]["tinc"] = 655;
        ports["tcp"]["silc"] = 706;
        ports["tcp"]["kerberos-adm"] = 749;
        ports["tcp"]["webster"] = 765;
        ports["tcp"]["rsync"] = 873;
        ports["tcp"]["ftps-data"] = 989;
        ports["tcp"]["ftps"] = 990;
        ports["tcp"]["telnets"] = 992;
        ports["tcp"]["imaps"] = 993;
        ports["tcp"]["ircs"] = 994;
        ports["tcp"]["pop3s"] = 995;
        ports["tcp"]["socks"] = 1080;
        ports["tcp"]["proofd"] = 1093;
        ports["tcp"]["rootd"] = 1094;
        ports["tcp"]["openvpn"] = 1194;
        ports["tcp"]["rmiregistry"] = 1099;
        ports["tcp"]["kazaa"] = 1214;
        ports["tcp"]["nessus"] = 1241;
        ports["tcp"]["lotusnote"] = 1352;
        ports["tcp"]["ms-sql-s"] = 1433;
        ports["tcp"]["ms-sql-m"] = 1434;
        ports["tcp"]["ingreslock"] = 1524;
        ports["tcp"]["prospero-np"] = 1525;
        ports["tcp"]["datametrics"] = 1645;
        ports["tcp"]["sa-msg-port"] = 1646;
        ports["tcp"]["kermit"] = 1649;
        ports["tcp"]["l2f"] = 1701;
        ports["tcp"]["radius"] = 1812;
        ports["tcp"]["radius-acct"] = 1813;
        ports["tcp"]["msnp"] = 1863;
        ports["tcp"]["unix-status"] = 1957;
        ports["tcp"]["log-server"] = 1958;
        ports["tcp"]["remoteping"] = 1959;
        ports["tcp"]["cisco-sccp"] = 2000;
        ports["tcp"]["search"] = 2010;
        ports["tcp"]["pipe_server"] = 2010;
        ports["tcp"]["nfs"] = 2049;
        ports["tcp"]["gnunet"] = 2086;
        ports["tcp"]["rtcm-sc104"] = 2101;
        ports["tcp"]["cvspserver"] = 2401;
        ports["tcp"]["venus"] = 2430;
        ports["tcp"]["venus-se"] = 2431;
        ports["tcp"]["codasrv"] = 2432;
        ports["tcp"]["codasrv-se"] = 2433;
        ports["tcp"]["mon"] = 2583;
        ports["tcp"]["dict"] = 2628;
        ports["tcp"]["gpsd"] = 2947;
        ports["tcp"]["gds_db"] = 3050;
        ports["tcp"]["icpv2"] = 3130;
        ports["tcp"]["mysql"] = 3306;
        ports["tcp"]["nut"] = 3493;
        ports["tcp"]["distcc"] = 3632;
        ports["tcp"]["daap"] = 3689;
        ports["tcp"]["svn"] = 3690;
        ports["tcp"]["suucp"] = 4031;
        ports["tcp"]["sysrqd"] = 4094;
        ports["tcp"]["remctl"] = 4373;
        ports["tcp"]["iax"] = 4569;
        ports["tcp"]["radmin-port"] = 4899;
        ports["tcp"]["rfe"] = 5002;
        ports["tcp"]["mmcc"] = 5050;
        ports["tcp"]["sip"] = 5060;
        ports["tcp"]["sip-tls"] = 5061;
        ports["tcp"]["aol"] = 5190;
        ports["tcp"]["xmpp-client"] = 5222;
        ports["tcp"]["xmpp-server"] = 5269;
        ports["tcp"]["cfengine"] = 5308;
        ports["tcp"]["mdns"] = 5353;
        ports["tcp"]["postgresql"] = 5432;
        ports["tcp"]["freeciv"] = 5556;
        ports["tcp"]["ggz"] = 5688;
        ports["tcp"]["x11"] = 6000;
        ports["tcp"]["x11-1"] = 6001;
        ports["tcp"]["x11-2"] = 6002;
        ports["tcp"]["x11-3"] = 6003;
        ports["tcp"]["x11-4"] = 6004;
        ports["tcp"]["x11-5"] = 6005;
        ports["tcp"]["x11-6"] = 6006;
        ports["tcp"]["x11-7"] = 6007;
        ports["tcp"]["gnutella-svc"] = 6346;
        ports["tcp"]["gnutella-rtr"] = 6347;
        ports["tcp"]["sge_qmaster"] = 6444;
        ports["tcp"]["sge_execd"] = 6445;
        ports["tcp"]["afs3-fileserver"] = 7000;
        ports["tcp"]["afs3-callback"] = 7001;
        ports["tcp"]["afs3-prserver"] = 7002;
        ports["tcp"]["afs3-vlserver"] = 7003;
        ports["tcp"]["afs3-kaserver"] = 7004;
        ports["tcp"]["afs3-volser"] = 7005;
        ports["tcp"]["afs3-errors"] = 7006;
        ports["tcp"]["afs3-bos"] = 7007;
        ports["tcp"]["afs3-update"] = 7008;
        ports["tcp"]["afs3-rmtsys"] = 7009;
        ports["tcp"]["font-service"] = 7100;
        ports["tcp"]["http-alt"] = 8080;
        ports["tcp"]["bacula-dir"] = 9101;
        ports["tcp"]["bacula-fd"] = 9102;
        ports["tcp"]["bacula-sd"] = 9103;
        ports["tcp"]["amanda"] = 10080;
        ports["tcp"]["hkp"] = 11371;
        ports["tcp"]["bprd"] = 13720;
        ports["tcp"]["bpdbm"] = 13721;
        ports["tcp"]["bpjava-msvc"] = 13722;
        ports["tcp"]["vnetd"] = 13724;
        ports["tcp"]["bpcd"] = 13782;
        ports["tcp"]["vopied"] = 13783;
        ports["tcp"]["wnn6"] = 22273;
        ports["tcp"]["kerberos4"] = 750;
        ports["tcp"]["kerberos_master"] = 751;
        ports["tcp"]["krb_prop"] = 754;
        ports["tcp"]["krbupdate"] = 760;
        ports["tcp"]["swat"] = 901;
        ports["tcp"]["kpop"] = 1109;
        ports["tcp"]["knetd"] = 2053;
        ports["tcp"]["eklogin"] = 2105;
        ports["tcp"]["kx"] = 2111;
        ports["tcp"]["iprop"] = 2121;
        ports["tcp"]["supfilesrv"] = 871;
        ports["tcp"]["supfiledbg"] = 1127;
        ports["tcp"]["linuxconf"] = 98;
        ports["tcp"]["poppassd"] = 106;
        ports["tcp"]["ssmtp"] = 465;
        ports["tcp"]["moira_db"] = 775;
        ports["tcp"]["moira_update"] = 777;
        ports["tcp"]["spamd"] = 783;
        ports["tcp"]["omirr"] = 808;
        ports["tcp"]["customs"] = 1001;
        ports["tcp"]["skkserv"] = 1178;
        ports["tcp"]["rmtcfg"] = 1236;
        ports["tcp"]["wipld"] = 1300;
        ports["tcp"]["xtel"] = 1313;
        ports["tcp"]["xtelw"] = 1314;
        ports["tcp"]["support"] = 1529;
        ports["tcp"]["cfinger"] = 2003;
        ports["tcp"]["frox"] = 2121;
        ports["tcp"]["ninstall"] = 2150;
        ports["tcp"]["zebrasrv"] = 2600;
        ports["tcp"]["zebra"] = 2601;
        ports["tcp"]["ripd"] = 2602;
        ports["tcp"]["ripngd"] = 2603;
        ports["tcp"]["ospfd"] = 2604;
        ports["tcp"]["bgpd"] = 2605;
        ports["tcp"]["ospf6d"] = 2606;
        ports["tcp"]["ospfapi"] = 2607;
        ports["tcp"]["isisd"] = 2608;
        ports["tcp"]["afbackup"] = 2988;
        ports["tcp"]["afmbackup"] = 2989;
        ports["tcp"]["xtell"] = 4224;
        ports["tcp"]["fax"] = 4557;
        ports["tcp"]["hylafax"] = 4559;
        ports["tcp"]["distmp3"] = 4600;
        ports["tcp"]["munin"] = 4949;
        ports["tcp"]["enbd-cstatd"] = 5051;
        ports["tcp"]["enbd-sstatd"] = 5052;
        ports["tcp"]["pcrd"] = 5151;
        ports["tcp"]["noclog"] = 5354;
        ports["tcp"]["hostmon"] = 5355;
        ports["tcp"]["nsca"] = 5667;
        ports["tcp"]["mrtd"] = 5674;
        ports["tcp"]["bgpsim"] = 5675;
        ports["tcp"]["canna"] = 5680;
        ports["tcp"]["sane-port"] = 6566;
        ports["tcp"]["ircd"] = 6667;
        ports["tcp"]["zope-ftp"] = 8021;
        ports["tcp"]["tproxy"] = 8081;
        ports["tcp"]["omniorb"] = 8088;
        ports["tcp"]["clc-build-daemon"] = 8990;
        ports["tcp"]["xinetd"] = 9098;
        ports["tcp"]["git"] = 9418;
        ports["tcp"]["zope"] = 9673;
        ports["tcp"]["webmin"] = 10000;
        ports["tcp"]["kamanda"] = 10081;
        ports["tcp"]["amandaidx"] = 10082;
        ports["tcp"]["amidxtape"] = 10083;
        ports["tcp"]["smsqp"] = 11201;
        ports["tcp"]["xpilot"] = 15345;
        ports["tcp"]["sgi-cad"] = 17004;
        ports["tcp"]["isdnlog"] = 20011;
        ports["tcp"]["vboxd"] = 20012;
        ports["tcp"]["binkp"] = 24554;
        ports["tcp"]["asp"] = 27374;
        ports["tcp"]["csync2"] = 30865;
        ports["tcp"]["dircproxy"] = 57000;
        ports["tcp"]["tfido"] = 60177;
        ports["tcp"]["fido"] = 60179;

        ports["udp"]["echo"] = 7;
        ports["udp"]["discard"] = 9;
        ports["udp"]["daytime"] = 13;
        ports["udp"]["msp"] = 18;
        ports["udp"]["chargen"] = 19;
        ports["udp"]["fsp"] = 21;
        ports["udp"]["ssh"] = 22;
        ports["udp"]["time"] = 37;
        ports["udp"]["rlp"] = 39;
        ports["udp"]["tacacs"] = 49;
        ports["udp"]["re-mail-ck"] = 50;
        ports["udp"]["domain"] = 53;
        ports["udp"]["tacacs-ds"] = 65;
        ports["udp"]["bootps"] = 67;
        ports["udp"]["bootpc"] = 68;
        ports["udp"]["tftp"] = 69;
        ports["udp"]["gopher"] = 70;
        ports["udp"]["www"] = 80;
        ports["udp"]["kerberos"] = 88;
        ports["udp"]["acr-nema"] = 104;
        ports["udp"]["csnet-ns"] = 105;
        ports["udp"]["rtelnet"] = 107;
        ports["udp"]["pop2"] = 109;
        ports["udp"]["pop3"] = 110;
        ports["udp"]["sunrpc"] = 111;
        ports["udp"]["ntp"] = 123;
        ports["udp"]["pwdgen"] = 129;
        ports["udp"]["loc-srv"] = 135;
        ports["udp"]["netbios-ns"] = 137;
        ports["udp"]["netbios-dgm"] = 138;
        ports["udp"]["netbios-ssn"] = 139;
        ports["udp"]["imap2"] = 143;
        ports["udp"]["snmp"] = 161;
        ports["udp"]["snmp-trap"] = 162;
        ports["udp"]["cmip-man"] = 163;
        ports["udp"]["cmip-agent"] = 164;
        ports["udp"]["mailq"] = 174;
        ports["udp"]["xdmcp"] = 177;
        ports["udp"]["nextstep"] = 178;
        ports["udp"]["bgp"] = 179;
        ports["udp"]["prospero"] = 191;
        ports["udp"]["irc"] = 194;
        ports["udp"]["smux"] = 199;
        ports["udp"]["at-rtmp"] = 201;
        ports["udp"]["at-nbp"] = 202;
        ports["udp"]["at-echo"] = 204;
        ports["udp"]["at-zis"] = 206;
        ports["udp"]["qmtp"] = 209;
        ports["udp"]["z3950"] = 210;
        ports["udp"]["ipx"] = 213;
        ports["udp"]["imap3"] = 220;
        ports["udp"]["pawserv"] = 345;
        ports["udp"]["zserv"] = 346;
        ports["udp"]["fatserv"] = 347;
        ports["udp"]["rpc2portmap"] = 369;
        ports["udp"]["codaauth2"] = 370;
        ports["udp"]["clearcase"] = 371;
        ports["udp"]["ulistserv"] = 372;
        ports["udp"]["ldap"] = 389;
        ports["udp"]["imsp"] = 406;
        ports["udp"]["https"] = 443;
        ports["udp"]["snpp"] = 444;
        ports["udp"]["microsoft-ds"] = 445;
        ports["udp"]["kpasswd"] = 464;
        ports["udp"]["saft"] = 487;
        ports["udp"]["isakmp"] = 500;
        ports["udp"]["rtsp"] = 554;
        ports["udp"]["nqs"] = 607;
        ports["udp"]["npmp-local"] = 610;
        ports["udp"]["npmp-gui"] = 611;
        ports["udp"]["hmmp-ind"] = 612;
        ports["udp"]["qmqp"] = 628;
        ports["udp"]["ipp"] = 631;
        ports["udp"]["biff"] = 512;
        ports["udp"]["who"] = 513;
        ports["udp"]["syslog"] = 514;
        ports["udp"]["talk"] = 517;
        ports["udp"]["ntalk"] = 518;
        ports["udp"]["route"] = 520;
        ports["udp"]["timed"] = 525;
        ports["udp"]["netwall"] = 533;
        ports["udp"]["gdomap"] = 538;
        ports["udp"]["afpovertcp"] = 548;
        ports["udp"]["nntps"] = 563;
        ports["udp"]["submission"] = 587;
        ports["udp"]["ldaps"] = 636;
        ports["udp"]["tinc"] = 655;
        ports["udp"]["silc"] = 706;
        ports["udp"]["webster"] = 765;
        ports["udp"]["rsync"] = 873;
        ports["udp"]["telnets"] = 992;
        ports["udp"]["imaps"] = 993;
        ports["udp"]["ircs"] = 994;
        ports["udp"]["pop3s"] = 995;
        ports["udp"]["socks"] = 1080;
        ports["udp"]["proofd"] = 1093;
        ports["udp"]["rootd"] = 1094;
        ports["udp"]["openvpn"] = 1194;
        ports["udp"]["rmiregistry"] = 1099;
        ports["udp"]["kazaa"] = 1214;
        ports["udp"]["nessus"] = 1241;
        ports["udp"]["lotusnote"] = 1352;
        ports["udp"]["ms-sql-s"] = 1433;
        ports["udp"]["ms-sql-m"] = 1434;
        ports["udp"]["ingreslock"] = 1524;
        ports["udp"]["prospero-np"] = 1525;
        ports["udp"]["datametrics"] = 1645;
        ports["udp"]["sa-msg-port"] = 1646;
        ports["udp"]["kermit"] = 1649;
        ports["udp"]["l2f"] = 1701;
        ports["udp"]["radius"] = 1812;
        ports["udp"]["radius-acct"] = 1813;
        ports["udp"]["msnp"] = 1863;
        ports["udp"]["cisco-sccp"] = 2000;
        ports["udp"]["nfs"] = 2049;
        ports["udp"]["gnunet"] = 2086;
        ports["udp"]["rtcm-sc104"] = 2101;
        ports["udp"]["cvspserver"] = 2401;
        ports["udp"]["venus"] = 2430;
        ports["udp"]["venus-se"] = 2431;
        ports["udp"]["codasrv"] = 2432;
        ports["udp"]["codasrv-se"] = 2433;
        ports["udp"]["mon"] = 2583;
        ports["udp"]["dict"] = 2628;
        ports["udp"]["gpsd"] = 2947;
        ports["udp"]["gds_db"] = 3050;
        ports["udp"]["icpv2"] = 3130;
        ports["udp"]["mysql"] = 3306;
        ports["udp"]["nut"] = 3493;
        ports["udp"]["distcc"] = 3632;
        ports["udp"]["daap"] = 3689;
        ports["udp"]["svn"] = 3690;
        ports["udp"]["suucp"] = 4031;
        ports["udp"]["sysrqd"] = 4094;
        ports["udp"]["remctl"] = 4373;
        ports["udp"]["iax"] = 4569;
        ports["udp"]["radmin-port"] = 4899;
        ports["udp"]["rfe"] = 5002;
        ports["udp"]["mmcc"] = 5050;
        ports["udp"]["sip"] = 5060;
        ports["udp"]["sip-tls"] = 5061;
        ports["udp"]["aol"] = 5190;
        ports["udp"]["xmpp-client"] = 5222;
        ports["udp"]["xmpp-server"] = 5269;
        ports["udp"]["cfengine"] = 5308;
        ports["udp"]["mdns"] = 5353;
        ports["udp"]["postgresql"] = 5432;
        ports["udp"]["freeciv"] = 5556;
        ports["udp"]["ggz"] = 5688;
        ports["udp"]["x11"] = 6000;
        ports["udp"]["x11-1"] = 6001;
        ports["udp"]["x11-2"] = 6002;
        ports["udp"]["x11-3"] = 6003;
        ports["udp"]["x11-4"] = 6004;
        ports["udp"]["x11-5"] = 6005;
        ports["udp"]["x11-6"] = 6006;
        ports["udp"]["x11-7"] = 6007;
        ports["udp"]["gnutella-svc"] = 6346;
        ports["udp"]["gnutella-rtr"] = 6347;
        ports["udp"]["sge_qmaster"] = 6444;
        ports["udp"]["sge_execd"] = 6445;
        ports["udp"]["afs3-fileserver"] = 7000;
        ports["udp"]["afs3-callback"] = 7001;
        ports["udp"]["afs3-prserver"] = 7002;
        ports["udp"]["afs3-vlserver"] = 7003;
        ports["udp"]["afs3-kaserver"] = 7004;
        ports["udp"]["afs3-volser"] = 7005;
        ports["udp"]["afs3-errors"] = 7006;
        ports["udp"]["afs3-bos"] = 7007;
        ports["udp"]["afs3-update"] = 7008;
        ports["udp"]["afs3-rmtsys"] = 7009;
        ports["udp"]["font-service"] = 7100;
        ports["udp"]["http-alt"] = 8080;
        ports["udp"]["bacula-dir"] = 9101;
        ports["udp"]["bacula-fd"] = 9102;
        ports["udp"]["bacula-sd"] = 9103;
        ports["udp"]["amanda"] = 10080;
        ports["udp"]["hkp"] = 11371;
        ports["udp"]["bprd"] = 13720;
        ports["udp"]["bpdbm"] = 13721;
        ports["udp"]["bpjava-msvc"] = 13722;
        ports["udp"]["vnetd"] = 13724;
        ports["udp"]["bpcd"] = 13782;
        ports["udp"]["vopied"] = 13783;
        ports["udp"]["wnn6"] = 22273;
        ports["udp"]["kerberos4"] = 750;
        ports["udp"]["kerberos_master"] = 751;
        ports["udp"]["passwd_server"] = 752;
        ports["udp"]["zephyr-srv"] = 2102;
        ports["udp"]["zephyr-clt"] = 2103;
        ports["udp"]["zephyr-hm"] = 2104;
        ports["udp"]["poppassd"] = 106;
        ports["udp"]["moira_ureg"] = 779;
        ports["udp"]["omirr"] = 808;
        ports["udp"]["customs"] = 1001;
        ports["udp"]["predict"] = 1210;
        ports["udp"]["ninstall"] = 2150;
        ports["udp"]["afbackup"] = 2988;
        ports["udp"]["afmbackup"] = 2989;
        ports["udp"]["noclog"] = 5354;
        ports["udp"]["hostmon"] = 5355;
        ports["udp"]["rplay"] = 5555;
        ports["udp"]["omniorb"] = 8088;
        ports["udp"]["mandelspawn"] = 9359;
        ports["udp"]["kamanda"] = 10081;
        ports["udp"]["smsqp"] = 11201;
        ports["udp"]["xpilot"] = 15345;
        ports["udp"]["sgi-cmsd"] = 17001;
        ports["udp"]["sgi-crsd"] = 17002;
        ports["udp"]["sgi-gcd"] = 17003;
        ports["udp"]["isdnlog"] = 20011;
        ports["udp"]["vboxd"] = 20012;
        ports["udp"]["asp"] = 27374;

// these are found in Cisco configs. Some of these names duplicate
// protocols listed above but a few are extras.
// http://www.cisco.com/en/US/docs/security/asa/asa80/configuration/guide/ports.html
//
// this is a mix of port names from PIX/ASA and IOS

        ports["tcp"]["aol"] = 5190;         // America Online
        ports["tcp"]["bgp"] = 179;          // Border Gateway Protocol, RFC 1163
        ports["tcp"]["chargen"] = 19;       // Character Generator
// about the port value for cifs:
// http://serverfault.com/questions/71582/list-of-cisco-asa-pre-defined-services
        ports["tcp"]["cifs"] = 3020;
        ports["tcp"]["citrix-ica"] = 1494;  // Citrix Independent Computing
                                            // Architecture (ICA) protocol
        ports["tcp"]["cmd"] = 514;          // Similar to exec except that cmd
                                            //  has automatic authentication
        ports["tcp"]["ctiqbe"] = 2748;      // Computer Telephony Interface
                                            //  Quick Buffer Encoding
        ports["tcp"]["daytime"] = 13;       // Day time, RFC 867
        ports["tcp"]["discard"] = 9;        // Discard
        ports["tcp"]["domain"] = 53;        // DNS
        ports["tcp"]["echo"] = 7;           // Echo
        ports["tcp"]["exec"] = 512;         // Remote process execution
        ports["tcp"]["finger"] = 79;        // Finger
        ports["tcp"]["ftp"] = 21;           // File Transfer Protocol (control port)
        ports["tcp"]["ftp-data"] = 20;      // File Transfer Protocol (data port)
        ports["tcp"]["gopher"] = 70;        // Gopher
        ports["tcp"]["h323"] = 1720;        // H.323 call signalling
        ports["tcp"]["hostname"] = 101;     // NIC Host Name Server
        ports["tcp"]["https"] = 443;        // HTTP over SSL
        ports["tcp"]["ident"] = 113;        // Ident authentication service
        ports["tcp"]["imap4"] = 143;        // Internet Message Access Protocol,
                                            //  version 4
        ports["tcp"]["irc"] = 194;          // Internet Relay Chat protocol
        ports["tcp"]["kerberos"] = 750;     // Kerberos
        ports["tcp"]["klogin"] = 543;       // KLOGIN
        ports["tcp"]["kshell"] = 544;       // Korn Shell
        ports["tcp"]["ldap"] = 389;         // Lightweight Directory Access
                                            // Protocol
        ports["tcp"]["ldaps"] = 636;        // Lightweight Directory Access
                                            //  Protocol (SSL)
        ports["tcp"]["login"] = 513;        // Remote login
        ports["tcp"]["lotusnotes"] = 1352;  // IBM Lotus Notes
        ports["tcp"]["lpd"] = 515;          // Line Printer Daemon - printer spooler
        ports["tcp"]["netbios-ssn"] = 139;  // NetBIOS Session Service
        ports["tcp"]["nfs"] = 2049;
        ports["tcp"]["nntp"] = 119;         // Network News Transfer Protocol
        ports["tcp"]["pcanywhere-data"] = 5631; // pcAnywhere data
        ports["tcp"]["pim-auto-rp"] = 496;  // Protocol Independent Multicast,
                                            //  reverse path flooding, dense mode
        ports["tcp"]["pop2"] = 109;         // Post Office Protocol - Version 2
        ports["tcp"]["pop3"] = 110;         // Post Office Protocol - Version 3
        ports["tcp"]["pptp"] = 1723;        // Point-to-Point Tunneling Protocol
        ports["tcp"]["rsh"] = 514;
        ports["tcp"]["rtsp"] = 554;
        ports["tcp"]["smtp"] = 25;          // Simple Mail Transport Protocol
        ports["tcp"]["sqlnet"] = 1521;      // Structured Query Language Network
        ports["tcp"]["ssh"] = 22;           // Secure Shell
        ports["tcp"]["sip"] = 5060;
        ports["tcp"]["sunrpc"] = 111;       // 111 Sun Remote Procedure Call
        ports["tcp"]["syslog"] = 514;
        ports["tcp"]["tacacs"] = 49;        // Terminal Access Controller
                                            //  Access Control System Plus
        ports["tcp"]["tacacs-ds"] = 63;     // ???
        ports["tcp"]["talk"] = 517;         // Talk
        ports["tcp"]["telnet"] = 23;        // RFC 854 Telnet
        ports["tcp"]["time"] = 37;          // ???
        ports["tcp"]["uucp"] = 540;         // UNIX-to-UNIX Copy Program
        ports["tcp"]["whois"] = 43;         // Who Is
        ports["tcp"]["http"] = 80;
        ports["tcp"]["www"] = 80;           // World Wide Web



        ports["udp"]["biff"] = 512;         // Used by mail system to notify
                                            //  users that new mail is received
        ports["udp"]["bootpc"] = 68;        // Bootstrap Protocol Client
        ports["udp"]["bootps"] = 67;        // Bootstrap Protocol Server
        ports["udp"]["cifs"] = 3020;
        ports["udp"]["discard"] = 9;        // Discard
        ports["udp"]["dnsix"] = 195;        // DNSIX Session Management
                                            //  Module Audit Redirector
        ports["udp"]["domain"] = 53;        // DNS
        ports["udp"]["echo"] = 7;
        ports["udp"]["isakmp"] = 500;       // Internet Security Association
                                            //  and Key Management Protocol
        ports["udp"]["kerberos"] = 750;     // Kerberos
        ports["udp"]["mobile-ip"] = 434;    // MobileIP-Agent
        ports["udp"]["nameserver"] = 42;    // Host Name Server
        ports["udp"]["netbios-dgm"] = 138;  // NetBIOS Datagram Service
        ports["udp"]["netbios-ns"] = 137;   // NetBIOS Name Service
        ports["udp"]["netbios-ss"] = 139;
        ports["udp"]["nfs"] = 2049;
        ports["udp"]["ntp"] = 123;          // Network Time Protocol
        ports["udp"]["pcanywhere-status"] = 5632; // pcAnywhere status
        ports["udp"]["pim-auto-rp"] = 496;  // Protocol Independent Multicast,
                                            //  reverse path flooding, dense mode
        ports["udp"]["radius"] = 1645;      // Remote Authentication Dial-In
                                            //  User Service
        ports["udp"]["radius-acct"] = 1646; // Remote Authentication Dial-In
                                            //  User Service (accounting)
        ports["udp"]["rip"] = 520;          // Routing Information Protocol
        ports["udp"]["rtsp"] = 554;
        ports["udp"]["secureid-udp"] = 5510; // SecureID over 
        ports["udp"]["sip"] = 5060;
        ports["udp"]["snmp"] = 161;         // Simple Network Management Protocol
        ports["udp"]["snmptrap"] = 162;     // Simple Network Management Protocol
                                            //  - Trap
        ports["udp"]["sunrpc"] = 111;       // 111 Sun Remote Procedure Call
        ports["udp"]["syslog"] = 514;       // System Log
        ports["udp"]["tacacs"] = 49;        // Terminal Access Controller
                                            //  Access Control System Plus
        ports["udp"]["talk"] = 517;         // Talk
        ports["udp"]["tftp"] = 69;          // Trivial File Transfer Protocol
        ports["udp"]["time"] = 37;          // Time
        ports["udp"]["who"] = 513;          // Who
        ports["udp"]["xdmcp"] = 177;        // X Display Manager Control Protocol


    }

    bool ok = false;
    int port = name.toInt(&ok);
    if (ok) return port;

    if (ports.contains(proto) && ports[proto].contains(name))
        return ports[proto][name];

    // I guess this can be considered a hack. For some reason ASA
    // converts all UDP ports in "show run" to the same names as if
    // they were tcp.
    if ((proto == "udp" || proto == "tcp-udp") && ports["tcp"].contains(name))
        return ports["tcp"][name];

    struct servent *se = getservbyname(name.toLatin1().constData(),
                                       proto.toLatin1().constData());

    if (se!=nullptr)
    {
        int port = ntohs(se->s_port);
        //free(se);
        return port;
    }

    return -1;
}

