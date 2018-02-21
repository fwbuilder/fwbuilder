/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef _DIALOGFACTORY_HH_
#define _DIALOGFACTORY_HH_

#include "BaseObjectDialog.h"

#include <QWidget>

namespace libfwbuilder {
    class FWObject;
    class FWException;
    class Firewall;
    class Rule;
};


class DialogFactory {
 public:

    static BaseObjectDialog *createDialog(QWidget *parent,const QString &objType);

    static QWidget *createFWDialog(QWidget *parent,libfwbuilder::FWObject *o);
    static QWidget *createOSDialog(QWidget *parent,libfwbuilder::FWObject *o);
    static QWidget *createIfaceDialog(QWidget *parent,libfwbuilder::FWObject *o);
    static QWidget *createClusterConfDialog(QWidget *parent, libfwbuilder::FWObject *o);
    static QString getClusterGroupOptionsDialogName(libfwbuilder::FWObject *o);
    static QWidget *createClusterGroupOptionsDialog(QWidget *parent, libfwbuilder::FWObject *o);

    static std::string getActionDialogPageName(libfwbuilder::Firewall *fw,
                                               libfwbuilder::Rule *rule);

};



#endif
