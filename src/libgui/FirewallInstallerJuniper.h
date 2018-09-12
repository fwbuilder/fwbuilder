/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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


#ifndef __FIREWALLINSTALLERJUNIPER_H_
#define __FIREWALLINSTALLERJUNIPER_H_



#include "FirewallInstaller.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qobject.h>
#include <QStringList>

namespace libfwbuilder
{
    class Firewall;
}

class FirewallInstallerJuniper : public FirewallInstaller
{
    Q_OBJECT;

protected:
    QStringList config_lines;

    virtual QString getDestinationDir(const QString &dir);
    
    virtual bool readManifest(const QString &conffie,
                              QMap<QString, QString> *all_files);
public:

    FirewallInstallerJuniper(instDialog *_dlg, instConf *_cnf, const QString &_p);

    virtual bool packInstallJobsList(libfwbuilder::Firewall*);
    virtual void activatePolicy(const QString &script, const QString &args);
    
};


#endif
