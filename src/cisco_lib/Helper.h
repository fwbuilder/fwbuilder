/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Helper.h,v 1.1 2008/03/06 06:48:56 vkurland Exp $

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

namespace fwcompiler {

    class Helper
    {
        fwcompiler::Compiler *compiler;

        /**
         * this methods checks if object addr belongs to network or address obj
         */
        bool belongsTo(libfwbuilder::Address *obj,
                       const libfwbuilder::IPAddress &a);
        bool belongsTo(libfwbuilder::Address *obj,
                       libfwbuilder::Address *addr);

        public:

        Helper(fwcompiler::Compiler *comp) { compiler=comp; }


        /**
         * finds interface of the firewall to whose subnet object
         * 'obj' belongs to.  Returns interface ID
         */
        std::string  findInterfaceByAddress(const libfwbuilder::IPAddress &a);
        std::string  findInterfaceByAddress(libfwbuilder::Address *obj);

        /**
         * finds interface of the firewall associated with the netzone
         * that object 'obj' belongs to.  Returns interface ID
         */
        std::string findInterfaceByNetzone(const libfwbuilder::IPAddress &a)
            throw(std::string);
        std::string findInterfaceByNetzone(libfwbuilder::Address *obj);
        std::list<std::string> findInterfaceByNetzoneOrAll(
            libfwbuilder::RuleElement *re);
        std::list<std::string> getAllInterfaceIDs();

        /**
         * recursively expands object 'o' and places all its children
         * objects in the list 'ol'. Uses cache in compiler.
         */
        void expand_group_recursive(libfwbuilder::FWObject *o,
                                    std::list<libfwbuilder::FWObject*> &ol);

        /**
         * recursively expands object 'o' and places all its children
         * objects in the list 'ol'. Does not use cache in compiler,
         * therefore can be called even if compiler object has not
         * been created yet.
         */
        void expand_group_recursive_no_cache(libfwbuilder::FWObject *o,
                                             std::list<libfwbuilder::FWObject*> &ol);

    };

};

class triplet {
    public:
            libfwbuilder::Address *src;
            libfwbuilder::Address *dst;
            libfwbuilder::Service *srv;
            triplet() {src=NULL; dst=NULL; srv=NULL;}
            triplet(libfwbuilder::Address *s,
                    libfwbuilder::Address *d,
                    libfwbuilder::Service *v) {src=s; dst=d; srv=v;}

            std::string hash();
};



#endif
