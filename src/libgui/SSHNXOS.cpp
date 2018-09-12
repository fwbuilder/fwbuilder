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

#include "SSHNXOS.h"

#include <QtDebug>


using namespace std;

SSHNXOS::SSHNXOS(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) :
    SSHCisco(_par,_h,args,_p,_ep,_in)
{
    normal_prompt=">$";
    fwb_prompt="--**--**--";
    enable_prompt="# $";
    pwd_prompt_1="'s password: $";
    pwd_prompt_2="Password: ";
    epwd_prompt="Password: ";
    ssh_pwd_prompt="'s password: ";
    ssoft_config_prompt="> ";
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

SSHNXOS::~SSHNXOS()
{
}

// NXOS state machine needs to be able to deal with
// "reload in ... " command

void SSHNXOS::stateMachine()
{
    if (checkForErrors()) return;

    // We need too delete files when doing scp with session
    if ( cmpPrompt(stdoutBuffer,
                QRegExp("Do you want to delete .* \\(yes/no/abort\\)   \\[y\\] ")) )
    {
        stdoutBuffer="";
        proc->write( "yes\n" );
    }

    switch (state)
    {

    case SCHEDULE_RELOAD_DIALOG:
        if ( cmpPrompt(stdoutBuffer,
                       QRegExp("System config.* modified\\. Save?")) )
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

    case PUSHING_CONFIG:
        if ( cmpPrompt(stdoutBuffer, QRegExp("Destination filename [.*]?")) )
        {
            stdoutBuffer="";
            proc->write("\n"); // accept default file name
        } else
            SSHCisco::stateMachine();
        break;

    default:
        SSHCisco::stateMachine();
        break;
    }
}
