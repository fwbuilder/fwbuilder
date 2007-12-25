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

#include "config.h"
#include "global.h"
#include "utils.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"

#include "FWWindow.h"
#include "FWBSettings.h"
#include "upgradePredicate.h"
#include "listOfLibraries.h"

#include "qobject.h"
#include <qdir.h>
#include <qfile.h>

#include <iostream>
#include <algorithm>

using namespace std;
using namespace libfwbuilder;

listOfLibraries::listOfLibraries()
{
// build list of available libraries

/* first read user's preferences. User may want to load some libraries
 * that we usually find but do not load by default (e.g. templates)
 */

    add(sysfname.c_str(),true);
    add(tempfname.c_str());

#ifdef _WIN32
    string ts = appRootDir + "/lib";
    QString dir = ts.c_str();
#else
    QString dir = QString(getenv("HOME")) + "/.fwbuilder/lib";
#endif

    QDir d(dir, "*.fwb" );
    for (unsigned int i=0; i<d.count(); ++i)
        add( (dir + "/" + d[i]).toLatin1().constData() );

    load();
}

void listOfLibraries::load()
{
    int N = st->getInt("Libraries/num");
    for (int i=0; i<N; ++i)
    {
        QString lp;
        QString s;
        bool    l;

        lp = QString("Libraries/lib%1_path").arg(i);
        s = st->getStr( lp );

        lp = QString("Libraries/lib%1_load").arg(i);
        l = st->getBool( lp );

        add( s.toLatin1().constData() , l );
    }
}

void listOfLibraries::save()
{
    st->setInt("Libraries/num", size() );

    int n = 0;
    for (list<libData>::iterator i=begin(); i!=end(); ++i,++n)
    {
        QString lp;

        lp = QString("Libraries/lib%1_path").arg(n);
        st->setStr( lp , i->path );

        lp = QString("Libraries/lib%1_load").arg(n);
        st->setBool( lp , i->load );
    }
}

class findPathPredicate {
    QString p;
    public:
    findPathPredicate(const QString &_p):p(_p){}
    bool operator()(const libData &ld) { return ld.path==p; }
};

class findNamePredicate {
    QString p;
    public:
    findNamePredicate(const QString &_p):p(_p){}
    bool operator()(const libData &ld) { return ld.name==p; }
};

class findIdPredicate {
    QString p;
    public:
    findIdPredicate(const QString &_p):p(_p){}
    bool operator()(const libData &ld) { return ld.id==p; }
};



class MessageBoxNeverUpgradePredicate: public libfwbuilder::XMLTools::UpgradePredicate
{
 public:
    MessageBoxNeverUpgradePredicate() {}
    
    virtual bool operator()(const std::string &msg) const 
    { 
        QMessageBox::information( NULL , "Firewall Builder", 
                                  QObject::tr(
"The library file you are trying to open\n\
has been saved in an older version of\n\
Firewall Builder and needs to be upgraded.\n\
To upgrade it, just load it in the Firewall\n\
Builder GUI and save back to file again."
                                  ),
                                  QObject::tr("&Continue"),
                                  QString::null,
                                  QString::null,
                                  0, 1 );
        return false;
    }
};

list<libData>::iterator listOfLibraries::add(const QString &path, bool load)
{
    QString name;
    QString id;

    if ( ! QFile::exists(path) ) return end();

    MessageBoxNeverUpgradePredicate dont_upgrade_predicate;

    try
    {
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(path.toAscii().constData(),  &dont_upgrade_predicate,  librespath);
        list<FWObject*> libs = ndb->getByType(Library::TYPENAME);
        for (list<FWObject*>::iterator i=libs.begin(); i!=libs.end(); i++)
        {
            name = (*i)->getName().c_str();
            id   = (*i)->getId().c_str();
            if ((*i)->getId() == STANDARD_LIB) continue;
            if ((*i)->getId() == DELETED_LIB)  continue;
            if ((*i)->getId() == TEMPLATE_LIB) continue;
            break;
        }

        delete ndb;

    } catch(FWException &ex)
    {
        QMessageBox::warning(
            NULL,"Firewall Builder", 
            QObject::tr("Error loading file %1:\n%2").
                 arg(path).arg(ex.toString().c_str()),
            QObject::tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return end();
    }

    if (id.isEmpty()) return end();
    if (name.isEmpty()) return end();

#if 0
    // commented out for bug #1620284
    //
/*
 *  mw is NULL at this point if this method is called to preload
 *  libraries on startup
 */
    list<FWObject*> currentLibs;
    if (mw)  currentLibs= mw->db()->getByType(Library::TYPENAME);

    if ( std::find_if(begin(),end(),findIdPredicate(id))!=end() ||
         (!currentLibs.empty() &&
          std::find_if(currentLibs.begin(),currentLibs.end(),
                       findFWObjectIDPredicate(id.ascii()))!=currentLibs.end() )
    )
    {
        QMessageBox::warning(
            NULL,"Firewall Builder", 
            QObject::tr("Duplicate library '%1'").arg(QString::fromUtf8(name)),
            QObject::tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return end();
    }
#endif

    if (id == STANDARD_LIB) return end();
    if (id == DELETED_LIB)  return end();
    if (id == TEMPLATE_LIB) return end();

    list<libData>::iterator i1=insert(end(),libData( id, name, path, false) );
    i1->load=load;

    return i1;
}

void listOfLibraries::setLoad(const QString &libPath, bool f)
{
    list<libData>::iterator it;
    if ( (it=std::find_if(begin(),end(),findPathPredicate(libPath)))!=end())
        it->load=f;
}

bool listOfLibraries::getLoad(const QString &libPath)
{
    list<libData>::iterator it;
    if ( (it=std::find_if(begin(),end(),findPathPredicate(libPath)))!=end())
        return it->load;

    return false;
}

bool listOfLibraries::isLoaded(const QString &libName)
{
    list<libData>::iterator it;
    if ( (it=std::find_if(begin(),end(),findNamePredicate(libName)))!=end())
        return it->load;

    return false;
}

bool listOfLibraries::isKnown(const QString &id)
{
    list<libData>::iterator it;
    if ( (it=std::find_if(begin(),end(),findIdPredicate(id)))!=end())
        return it->load;

    return false;
}

