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


#ifndef _SERVICE_OBJECT_MAKER_H_
#define _SERVICE_OBJECT_MAKER_H_

#include "objectMaker.h"


class ServiceObjectMaker : public ObjectMaker
{
    int custom_service_code_tracker;
    std::map<const QString, int> custom_service_codes;
    std::map<int, QString> tcp_flag_names;
    
public:
    
    ServiceObjectMaker(libfwbuilder::Library *l);
    virtual ~ServiceObjectMaker();

    virtual void clear();
    
    virtual libfwbuilder::FWObject* getCustomService(const QString &platform,
                                                     const QString &code,
                                                     const QString &protocol,
                                                     bool deduplicate=true);
    virtual libfwbuilder::FWObject* getIPService(int proto, bool fragments,
                                                 bool deduplicate=true);
    virtual libfwbuilder::FWObject* getICMPService(int type, int code,
                                                   bool deduplicate=true);

    virtual libfwbuilder::FWObject* getTCPService(int srs, int sre,
                                                  int drs, int dre,
                                                  bool established,
                                                  QList<int> &flags_mask,
                                                  QList<int> &flags_comp,
                                                  bool deduplicate=true);

    virtual libfwbuilder::FWObject* getUDPService(int srs, int sre,
                                                  int drs, int dre,
                                                  bool deduplicate=true);

    virtual libfwbuilder::FWObject* getTagService(const QString &tagcode,
                                                  bool deduplicate=true);



};

#endif
