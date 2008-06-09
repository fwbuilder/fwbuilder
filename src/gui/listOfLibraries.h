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

#ifndef __LISTOFLIBRARIES_H_
#define __LISTOFLIBRARIES_H_

#include "config.h"

#include <list>
#include <qstring.h>

/*
 * name     - name of the library object
 * path     - a full path for the file
 *
 */
class libData
{
 public:
    int id;
    QString name;
    QString path;
    bool    mandatory;
    bool    load;

    libData(int i, const QString &n, const QString &p, bool f)
        { id=i; name=n; path=p; mandatory=f; load=false; }
};

class listOfLibraries : public std::list<libData>
{
// I could use map<string> (with a full path to a library file being a
// key) but I do not want this list to be sorted by path. The list is
// likely to be short so simple linear search in isn't going to be a
// problem.

        
 public:
    listOfLibraries();

    /**
     * this method adds a library from the file 'path' to the list and
     * returns iterator that points at the new object in the list avLibs
     */
    std::list<libData>::iterator add(const QString &path, bool load=false);
    
    void setLoad(const QString &libPath, bool f);
    bool getLoad(const QString &libPath);

    bool isLoaded(const QString &libName);
    bool isKnown(int id);

    void load();
    void save();
};

#endif
