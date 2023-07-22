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


#ifndef __UPGRADEPREDICATE_H_
#define __UPGRADEPREDICATE_H_

#include "fwbuilder/XMLTools.h"

#include "qmessagebox.h"
#include "qobject.h"
#include <QPushButton>

class MessageBoxUpgradePredicate: public libfwbuilder::XMLTools::UpgradePredicate
{
    QWidget *parent;
 public:
    MessageBoxUpgradePredicate(QWidget *p=nullptr) { parent=p; }
    
    virtual bool operator()(const std::string&) const 
    { 
        QMessageBox msgBox;
        msgBox.setWindowTitle("Firewall Builder");
        msgBox.setText(QObject::tr("The data file you are trying to open has been \
                                  saved with an older version of Firewall Builder. \
                                  Opening it in this version will cause it to be \
                                  upgraded, which may prevent older versions of \
                                  the program from reading it. Backup copy of your \
                                  file in the old format will be made in the same \
                                  directory with extension '.bak'.\n\
                                  Are you sure you want to open it?"));
        msgBox.setIcon(QMessageBox::Information);
        QPushButton *upgradeButton = msgBox.addButton(QObject::tr("&Upgrade"), QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton(QObject::tr("&Do not load the file"), QMessageBox::RejectRole);
        msgBox.setEscapeButton(cancelButton);

        msgBox.exec();

        return msgBox.clickedButton() == upgradeButton;
    }
};

#endif
