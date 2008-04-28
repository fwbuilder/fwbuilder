/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: RuleRowInfo.h,v 1.4 2007/01/06 22:03:25 vkurland Exp $

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
#include <qtableview.h>

class RuleRowInfo 
{
public:
    RuleRowInfo (RuleRowInfo & r)
    {
        this->operator =(r);
    }
    RuleRowInfo (QString groupName, bool begin,bool hide)
    {
        isBeginRow = begin ;
        this->groupName=groupName;
        index=NULL;
        isHide = hide ;
    }
    QString groupName ;
    QString color ;
    bool isBeginRow;
    bool isHide ; 
    QModelIndex * index ;
    RuleRowInfo & operator = (RuleRowInfo & r)
    {
        this->isBeginRow = r.isBeginRow;
        this->groupName = r.groupName;
        this->index = r.index;
        this->isHide = r.isHide ;
        return *this;
    }

};
