/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __INSTCONF_H_
#define __INSTCONF_H_

#include <qstring.h>
#include <string>

namespace libfwbuilder {
    class Firewall;
};

class instConf {

 public:
    bool      quiet;
    bool      verbose;
    int       debug;
    bool      incremental;
    bool      dry_run;
    bool      save_diff;
    bool      no_gui;
    bool      backup;
    bool      stripComments;
    bool      compressScript;
    bool      copyFWB;
    bool      saveStandby;
    bool      batchInstall;
    bool      useSCPForRouter; // use scp for cisco ios, pix and hp procurve
    bool      useNXOSSession; // use NXOS Session Manager

    QString   pgm;
    QString   wdir;
    QString   diff_pgm;
    QString   user;
    QString   activationCmd;
    QString   pwd;
    QString   epwd;
    QString   maddr;
    QString   putty_session;
    QString   sshArgs;
    QString   scpArgs;
    QString   fwdir;
    QString   fwscript;
    
    libfwbuilder::Firewall  *fwobj;

    QString   fwbfile;
    QString   script;
    QString   remote_script;
    QString   backup_file;
    QString   diff_file;

    instConf();

    QString getCmdFromResource(const QString &resource_name);
    void clear();
   
};

#endif
