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


#ifndef __HELPER_HH
#define __HELPER_HH

#include <fwcompiler/Compiler.h>


#include <fwbuilder/FWObject.h>
#include <fwbuilder/Address.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/FWException.h>

namespace fwcompiler {

    class Helper
    {
        fwcompiler::Compiler *compiler;

        public:

        Helper(fwcompiler::Compiler *comp) { compiler=comp; }


        /**
         * finds interface of the firewall to whose subnet object
         * 'obj' belongs to.  Returns interface ID
         */
        int  findInterfaceByAddress(const libfwbuilder::InetAddr *a,
                                    const libfwbuilder::InetAddr *nm=nullptr);
        int  findInterfaceByAddress(libfwbuilder::Address *obj);

        /**
         * finds interface of the firewall associated with the netzone
         * that object 'obj' belongs to.  Returns interface ID
         */
        int findInterfaceByNetzone(const libfwbuilder::InetAddr *a,
                                   const libfwbuilder::InetAddr *nm=nullptr);
        int findInterfaceByNetzone(libfwbuilder::Address *obj);
        std::list<int> findInterfaceByNetzoneOrAll(
            libfwbuilder::RuleElement *re);
        std::list<int> getAllInterfaceIDs();

        /**
         * recursively expands object 'o' and places all its children
         * objects in the list 'ol'.
         */
        void expand_group_recursive(libfwbuilder::FWObject *o,
                                    std::list<libfwbuilder::FWObject*> &ol);


    };

};


#endif
