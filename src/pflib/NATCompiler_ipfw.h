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

#ifndef __NATCOMPILER_IPFW_HH
#define __NATCOMPILER_IPFW_HH

#include "NATCompiler_pf.h"

#include <map>

#define FTP_PORT    21
#define RCMD_PORT   514
#define H323_PORT   1720
#define RAUDIO_PORT 5050
#define ISAKMP_PORT 500

namespace fwcompiler {


    class NATCompiler_ipfw : public NATCompiler_pf {

	protected:

	virtual std::string myPlatformName();

	public:

	NATCompiler_ipfw(libfwbuilder::FWObjectDatabase *_db,
                         libfwbuilder::Firewall *fw,
                         bool ipv6_policy,
                         fwcompiler::OSConfigurator *_oscnf) :
        NATCompiler_pf(_db, fw, ipv6_policy, _oscnf) {}


	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
	
    };


}

#endif
