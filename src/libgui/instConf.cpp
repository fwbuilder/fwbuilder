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



#include "global.h"
#include "utils.h"

#include "instConf.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"

#include <iostream>

#include <qobject.h>
#include <qregexp.h>
#include <qsettings.h>
#include <QtDebug>

using namespace std;
using namespace libfwbuilder;

instConf::instConf()
{
    clear();
}

QString instConf::getCmdFromResource(const QString &resource_name)
{
    if (fwbdebug)
        qDebug() << QString("instConf::getCmdFromResource resource_name=%1")
            .arg(resource_name);

    string optpath_root = "activation/reg_user/run/";
    string optpath = optpath_root + resource_name.toStdString();
    QString cmd = Resources::getTargetOptionStr(fwobj->getStr("host_OS"),
                                                optpath).c_str();
    cmd = cmd.trimmed();
    if (fwbdebug)
        qDebug() << "instConf::getCmdFromResource  cmd=" << cmd;

    return cmd;
}

void instConf::clear()
{
    quiet = false;
    verbose = false;
    debug = 0;
    incremental = false;
    dry_run = false;
    saveStandby = false;
    save_diff = false;
    diff_pgm = "";
    no_gui = false;
    backup = false;
    backup_file = "";
    wdir = "./";
    fwobj = nullptr;
    maddr = "";
    user = "";
    batchInstall = false;
    sshArgs = "";
    scpArgs = "";
    putty_session = "";
    fwscript = "";
}

