/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: SSHPIX.h,v 1.7 2007/05/11 02:14:28 vkurland Exp $

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


#ifndef  __SSHPIX_H_
#define  __SSHPIX_H_

#include "../../config.h"
#include "global.h"

#include "SSHSession.h"

#include <qstring.h>
#include <qstringlist.h>

#include <list>
#include <string>



class SSHPIX : public SSHSession {

    Q_OBJECT

    int         nLines;
    int         ncmd;
    QStringList allConfig;

    QStringList newAcls;
    QStringList currentAcls;
        
    QStringList newObjectGroups;
    QStringList currentObjectGroups;
    
    std::ifstream   *config_file;

protected:
    QStringList      pre_config_commands;
    QStringList      post_config_commands;
    
public:
    
    SSHPIX(QWidget *parent,
           const QString &host,
           const QStringList &args,
           const QString &pwd,
           const QString &epwd,
           const std::list<std::string> &in);
    virtual ~SSHPIX();

    virtual bool checkForErrors();
    virtual void stateMachine();

    QString cmd(QProcess *proc,const QString &cmd);

    void loadPreConfigCommands(const QStringList &cl);
    void loadPostConfigCommands(const QStringList &cl);
    
public slots:
 void PIXbackup();
 void getACLs();
 void clearACLs();
 void getObjectGroups();
 void clearObjectGroups();
 void PIXincrementalInstall();
     
};

#endif
