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


#include "inspectionProtocol.h"

std::map<std::string,InspectionProtocol*> InspectionProtocol::protocols;

/*
 *  Default ports are defined here jsut like they are filled in the
 *  options by the GUI. If the GUI allows for port range, we specify
 *  port range here, and vice versa. Some of the cases seem to differ
 *  from what Cisco doc specify in the table of the default ports here
 *  http://www.cisco.com/en/US/products/sw/secursw/ps2120/products_upgrade_guides09186a0080369ee2.html
 *  I suppose this is ok since we always can use port range map with
 *  "match" command even if they did not intend it to be like that by
 *  default. However if the GUI returned port numbers that match those
 *  defined in protocolDefinitions, we do not generate 'match' commands
 *  at all and put everything in the "inspection_default" class-map
 *
 *  Here is how this works: constructor of the class InspectionProtocols 
 *  adds object to map 'protocols'. Every initialization of an object
 *  of this class in array protocolDefinitions calls constructor and 
 *  therefore creates an entry in the map 'protocols'. It is done this
 *  way because we can statically initialize an array but cant initialize
 *  std::map  (at least I do not know how)
 *
 *  Note: in PIX 7.0 inspector that corresponds to fixup 'smtp' is
 *  called 'esmtp'
 */
InspectionProtocol protocolDefinitions[] = 
{ 
    InspectionProtocol("ctiqbe",    "ctiqbe",    "tcp",   2748,  0     ),
    InspectionProtocol("dns",       "dns",       "udp",   53,    0     ),
    InspectionProtocol("ftp",       "ftp",       "tcp",   21,    0     ),
    InspectionProtocol("gtp",       "gtp",       "udp",   2123,  3386  ),
    InspectionProtocol("h323_h225", "h323 h225", "tcp",   1720,  1720  ),
    InspectionProtocol("h323_ras",  "h323 ras",  "udp",   1718,  1719  ),
    InspectionProtocol("http",      "http",      "tcp",   80,    80    ),
    InspectionProtocol("icmp_error","icmp",      "icmp",  0,     0     ),
    InspectionProtocol("ils",       "ils",       "tcp",   389,   389   ),
    InspectionProtocol("mgcp",      "mgcp",      "udp",   2427,  2727  ),
    InspectionProtocol("netbios",   "netbios",   "udp",   137,   138   ),
    InspectionProtocol("rpc",       "rpc",       "udp",   111,   0     ),
    InspectionProtocol("rsh",       "rsh",       "tcp",   514,   0     ),
    InspectionProtocol("rtsp",      "rtsp",      "tcp",   554,   0     ),
    InspectionProtocol("sip",       "sip",       "tcp",   5060,  5060  ),
    InspectionProtocol("sip_udp",   "sip",       "udp",   5060,  0     ),
    InspectionProtocol("skinny",    "skinny",    "tcp",   2000,  2000  ),
    InspectionProtocol("smtp",      "esmtp",     "tcp",   25,    25    ),
    InspectionProtocol("sqlnet",    "sqlnet",    "tcp",   1521,  1521  ),
    InspectionProtocol("tftp",      "tftp",      "udp",   69,    0     ),
    InspectionProtocol("xdmcp",     "xdmcp",     "udp",   177,   0     ),
    InspectionProtocol("ip_options_eool", "eool","",      0,     0     ),
    InspectionProtocol("ip_options_nop", "nop",  "",      0,     0     ),
    InspectionProtocol("ip_options_rtralt", "router-alert", "", 0,     0     ),
};
