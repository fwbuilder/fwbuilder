/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com

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


#include <QString>
#include <QTextStream>

class RuleRowInfo 
{
public:

    QString groupName ;
    QString color ;
    bool isFirstRow;
    bool collapsedGroup ; 
    QModelIndex* index ;

    RuleRowInfo(RuleRowInfo & r)
    {
        this->operator =(r);
    }

    RuleRowInfo(QString groupName, bool first_row, bool collapsed)
    {
        isFirstRow = first_row ;
        this->groupName = groupName;
        collapsedGroup = collapsed ;
        index = NULL;
    }
    
    RuleRowInfo& operator=(RuleRowInfo& r)
    {
        this->isFirstRow = r.isFirstRow;
        this->groupName = r.groupName;
        this->index = r.index;
        this->collapsedGroup = r.collapsedGroup ;
        return *this;
    }
};
