/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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


#ifndef __MANAGEMENT_HH_FLAG__
#define __MANAGEMENT_HH_FLAG__

#include "fwbuilder/InetAddr.h"
#include "fwbuilder/FWObject.h"

namespace libfwbuilder
{

    class PolicyInstallScript : public FWObject 
    {
        public:

        PolicyInstallScript();

        DECLARE_FWOBJECT_SUBTYPE(PolicyInstallScript);

        DECLARE_DISPATCH_METHODS(PolicyInstallScript);
        
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);
        virtual FWObject& shallowDuplicate(
            const FWObject *obj, bool preserve_id = true);
        virtual bool cmp(const FWObject *obj, bool recursive=false);

        const std::string& getCommand () const;
        void          setCommand (const std::string& );

        const std::string& getArguments() const;
        void          setArguments(const std::string& );

        bool isEmpty() const;
        bool isEnabled() const;

        void setEnabled(bool v);

        private:
        
        bool   enabled;
        std::string command;
        std::string arguments;
    };

    class SNMPManagement : public FWObject 
    {
        public:

        SNMPManagement();

        DECLARE_FWOBJECT_SUBTYPE(SNMPManagement);

        DECLARE_DISPATCH_METHODS(SNMPManagement);
        
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);
        virtual FWObject& shallowDuplicate(
            const FWObject *obj, bool preserve_id = true);
        virtual bool cmp(const FWObject *obj, bool recursive=false);

        const std::string& getReadCommunity () const;
        void          setReadCommunity (const std::string& );

        const std::string& getWriteCommunity() const;
        void          setWriteCommunity(const std::string& );

        bool isEmpty() const;
        bool isEnabled() const;

        void setEnabled(bool v);

        private:
        
        bool   enabled;
        std::string read_community;
        std::string write_community;
    };

    class FWBDManagement : public FWObject 
    {
        public:

        FWBDManagement();
        virtual ~FWBDManagement();

        DECLARE_FWOBJECT_SUBTYPE(FWBDManagement);

        DECLARE_DISPATCH_METHODS(FWBDManagement);
        
        virtual FWObject& shallowDuplicate(
            const FWObject *obj, bool preserve_id = true);
        virtual bool cmp(const FWObject *obj, bool recursive=false);

        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);

        int           getPort()       const;
        void          setPort(int);

        const std::string &getIdentityId() const;
        void          setIdentityId(const std::string &);

        bool isEmpty() const;
        bool isEnabled() const;

        void setEnabled(bool v);
    
        private:

        bool   enabled     ;
        int    port        ;
        std::string identity_id ;
    };

    class Management : public FWObject 
    {
        public:

        Management();

        DECLARE_FWOBJECT_SUBTYPE(Management);

        DECLARE_DISPATCH_METHODS(Management);
        
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);
        virtual FWObject& shallowDuplicate(
            const FWObject *obj, bool preserve_id = true);
        virtual bool  cmp(const FWObject *obj, bool recursive=false);
	virtual bool  validateChild(FWObject *o);

        const InetAddr& getAddress() const { return addr; }
        void setAddress(const InetAddr& a) { addr = a; }

        PolicyInstallScript *getPolicyInstallScript();
        SNMPManagement      *getSNMPManagement();
        FWBDManagement      *getFWBDManagement();

        bool isEmpty() const;

        private:

        InetAddr addr;
    };
}

#endif //__MANAGEMENT_HH_FLAG__
