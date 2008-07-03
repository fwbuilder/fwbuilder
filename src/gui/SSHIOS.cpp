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



#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"

#include "SSHIOS.h"

#include <iostream>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

using namespace std;

SSHIOS::SSHIOS(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) : SSHPIX(_par,_h,args,_p,_ep,_in)
{
    normal_prompt=">$";
    fwb_prompt="--**--**--";
    enable_prompt="#$";
    pwd_prompt="'s password: $";
    epwd_prompt="Password: ";
    ssh_pwd_prompt="'s password: ";
    ssoft_config_prompt="> ";
    sudo_pwd_prompt="Password:";
    putty_pwd_prompt="Password: ";
    passphrase_prompt="Enter passphrase for key ";

    errorsInit.clear();
    errorsInit.push_back("Permission denied");
    errorsInit.push_back("Invalid password");
    errorsInit.push_back("Access denied");
    errorsInit.push_back("Unable to authenticate");
    errorsInit.push_back("Too many authentication failures");

    errorsLoggedin.clear();
    errorsLoggedin.push_back("Invalid password");
    errorsLoggedin.push_back("ERROR: ");
    errorsLoggedin.push_back("Not enough arguments");
    errorsLoggedin.push_back("cannot find");

    errorsEnabledState.clear();
    errorsEnabledState.push_back("ERROR: ");
    errorsEnabledState.push_back("Type help");
    errorsEnabledState.push_back("Not enough arguments");
    errorsEnabledState.push_back("invalid input detected");
    errorsEnabledState.push_back("Invalid");
    errorsEnabledState.push_back("cannot find");

}

SSHIOS::~SSHIOS()
{
}

// IOS state machine needs to be able to deal with
// "reload in ... " command

void SSHIOS::stateMachine()
{
    if (checkForErrors()) return;

    switch (state)
    {
    case ENABLE:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            if (pre_config_commands.size()>0)
            {
                stdoutBuffer="";

                QString cmd = pre_config_commands.front();
                pre_config_commands.pop_front();

                if (cmd.indexOf("reload in")!=-1)
                    state = SCHEDULE_RELOAD_DIALOG;

                proc->write( cmd.toAscii() );
                proc->write( "\n" );
                break;
            } else
                SSHPIX::stateMachine();
        }
        break;

    case SCHEDULE_RELOAD_DIALOG:
        if ( cmpPrompt(stdoutBuffer,QRegExp("System config.* modified\\. Save?")) )
        {
            stdoutBuffer="";
            proc->write( "no\n" );
            break;
        }
        if ( cmpPrompt(stdoutBuffer,QRegExp("Proceed with reload?")) )
        {
            stdoutBuffer="";
            proc->write( "y\n" );
            state = ENABLE;
            break;
        }
        break;

    default:
        SSHPIX::stateMachine();
        break;
    }
}
