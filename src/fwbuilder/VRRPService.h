/*
 * VRRPService.h - VRRPService class header file
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef __VRRPSERVICE_H_
#define __VRRPSERVICE_H_

#include <fwbuilder/Service.h>

namespace libfwbuilder
{

    class VRRPService : public Service
    {

    public:

        VRRPService();
        VRRPService(const FWObjectDatabase *root, bool prepopulate);
        virtual ~VRRPService();

        virtual void fromXML(xmlNodePtr parent) throw(FWException);
        virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node) throw(FWException);

        DECLARE_FWOBJECT_SUBTYPE(VRRPService);

        virtual std::string getProtocolName();
        virtual int getProtocolNumber();
    };

}

#endif

