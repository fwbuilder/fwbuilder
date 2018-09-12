/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#include "SSHProcurve.h"

#include <QtDebug>


using namespace std;

/*
 * HP "normal" prompt looks something like this:
 *
 *   ProCurve Switch 3500-24> [24;1H[24;26H[24;1H[
 *
 * Enable password prompt also uses ansi codes:
 *   Password: [?25h[24;1H[?25h[24;11H[1;1H[?25l[24;11H[24;1H
 *
 * once in enable mode, the prompt looks like this:
 *
 *   ProCurve Switch 3500-24# ^[[24;1H^[[24;26H^[[24;1H^[[?25h^[[24;26H
 *
 * but we clear all ANSI sequences in SSHSession::readFromStdout()
 */


SSHProcurve::SSHProcurve(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) :
    SSHCisco(_par,_h,args,_p,_ep,_in)
{
    normal_prompt = "> *$";
    enable_prompt = "# *$"; // matches config prompt too
    epwd_prompt = "Password: *$";
    hp_greeting_prompt = "Press any key to continue *$";

    pwd_prompt_1 = "'s password: $";
    pwd_prompt_2 = "Password: ";

    comment_symbol = ';';

    errorsLoggedin.push_back("Unable to verify password");
}

SSHProcurve::~SSHProcurve()
{
}

// Procurve state machine needs to be able to deal with
// "reload after ... " command

void SSHProcurve::stateMachine()
{
    if (checkForErrors()) return;

    switch (state)
    {

    case NONE:
/*
 * Procurve prints a full page greeting right after it accepts user password
 * and provides prompt "Press any key to continue". Press "any key" to
 * proceed.
 */
        if (cmpPrompt(stdoutBuffer, QRegExp(hp_greeting_prompt)))
        {
            stdoutBuffer="";
            proc->write("\n");
            break;
        } else
            SSHCisco::stateMachine();

        break;

    case SCHEDULE_RELOAD_DIALOG:
        if ( cmpPrompt(stdoutBuffer,
                       QRegExp("Do you want to save current configuration [y/n]?")) )
        {
            stdoutBuffer="";
            proc->write( "no\n" );
            break;
        }
        if ( cmpPrompt(
                 stdoutBuffer,
                 QRegExp("System will be rebooted at the scheduled time .*Do you want to continue [y/n]? ")) )
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

    case EXIT_FROM_CONFIG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            /*
             * Execute post_config_commands
             */
            if (post_config_commands.size()>0)
            {
                stdoutBuffer = "";
                QString cmd = post_config_commands.front();
                post_config_commands.pop_front();
                sendCommand(cmd);
                break;
            }

            stdoutBuffer="";
            state = EXIT;
            // Use command "logout" to log out from enable mode
            // instead of exit, which exits to normal mode.
            proc->write( "logout\n");
        }
        break;

    case EXIT:
        if ( cmpPrompt(stdoutBuffer,QRegExp("Do you want to log out [y/n]?")) )
        {
            stdoutBuffer="";
            proc->write("y\n"); // accept default file name
            state = FINISH;
        }
        break;

    default:
        SSHCisco::stateMachine();
        break;
    }
}

/*
 * for some reason ssh session to a ProCurve always finishes with
 * return code 255
 */
void SSHProcurve::finished(int retcode)
{
    if (fwbdebug) qDebug("SSHProcurve::processExited   proc=%p retcode=%d",
                         proc, retcode);

    // background process has exited now, we do not need proc object anymore
    cleanUp();

    QString exitStatus = (retcode)?QObject::tr("ERROR"):QObject::tr("OK");

    emit printStdout_sign(tr("SSH session terminated, exit status: %1").
                          arg(retcode) + "\n");

    // Consider return code 0 and 255 a success, this is different from SSHSession
    // 
    sessionComplete(retcode!=0 && retcode!=255);

    if (fwbdebug) qDebug("SSHProcurve::processExited done");
}
