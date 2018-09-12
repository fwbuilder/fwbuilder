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


#ifndef  __SSHCISCO_H_
#define  __SSHCISCO_H_

#include "global.h"

#include "SSHSession.h"

#include <qstring.h>
#include <qstringlist.h>

#include <list>
#include <string>

class QEventLoop;

class SSHCisco : public SSHSession {

    Q_OBJECT;

    int         nLines;
    int         ncmd;

protected:
    QEventLoop *local_event_loop;

    QStringList newAcls;
    QStringList currentAcls;
        
    QStringList newObjectGroups;
    QStringList currentObjectGroups;
    
    QStringList pre_config_commands;
    QStringList post_config_commands;
    QStringList activation_commands;

    char comment_symbol;
    
public:
    
    SSHCisco(QWidget *parent,
             const QString &host,
             const QStringList &args,
             const QString &pwd,
             const QString &epwd,
             const std::list<std::string> &in);
    virtual ~SSHCisco();

    virtual bool checkForErrors();
    virtual void stateMachine();

    QString cmd(QProcess *proc,const QString &cmd);

    void loadPreConfigCommands(const QStringList &cl);
    void loadPostConfigCommands(const QStringList &cl);
    void loadActivationCommands(const QStringList &cl);
    
};

#endif
