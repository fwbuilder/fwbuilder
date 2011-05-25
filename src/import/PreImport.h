/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef _PRE_IMPORTER_H_
#define _PRE_IMPORTER_H_


#include <QString>
#include <QStringList>


class matchPFDirection
{
public:
    virtual bool operator()(const QString&) {return false;}
};

/*
 * This class scans firewall configuration and tries to guess platform
 * and some other parameters
 */
class PreImport
{
    const QStringList *buffer;

public:

    enum Platforms { UNKNOWN, IPTABLES, IPTABLES_WITH_COUNTERS,
                     PF, PF_REVERSE, IOSACL, PIX, FWSM } ;

private:    

    enum Platforms platform;

public:
    
    PreImport(const QStringList *buf) { buffer = buf; platform = UNKNOWN; }
    void scan();
    enum Platforms getPlatform() { return platform; }
    QString getPlatformAsString();
    bool isReversePFConfigurationStyle(matchPFDirection &dir_op);
};

#endif
