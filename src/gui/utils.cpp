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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"

#include <qobject.h>
#include <qmenu.h>
#include <qcombobox.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qpalette.h>
#include <qnamespace.h>
#include <QHostInfo>
#include <qhostaddress.h>
#include <qapplication.h>
#include <qpixmapcache.h>
//Added by qt3to4:
#include <QList>
#include <QPixmap>

#include "FWBSettings.h"

#include "fwbuilder/dns.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"

#if defined(_WIN32)
#  include <stdio.h>
#  include <sys/timeb.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <time.h>
#  include <windows.h>
#else
#  include <unistd.h>
#  include <string.h>
#  include <errno.h>
#  include <pwd.h>
#endif

#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
using namespace libfwbuilder;


QAction* addPopupMenuItem(QObject *res,
                     QMenu* menu,
                     const QString &resourceIconPath,
                     const QString itemName,
                     const char* member,
                     const QKeySequence &accel)
{
    string icn;
    QPixmap pm;
    //int    itmID = -1;
    QAction *act = NULL;

    icn = Resources::global_res->getResourceStr(static_cast<const char*>(resourceIconPath.toLatin1()));
    if(icn!="")
    {
//        pm = QPixmap::fromMimeSource( icn.c_str() );
        if ( ! QPixmapCache::find( icn.c_str(), pm) )
        {
            pm.load( (":/"+icn).c_str() );//fromMimeSource( icn.c_str() );
            QPixmapCache::insert( icn.c_str(), pm);
        }
        act = menu->addAction( pm, itemName , res , member, accel ); //insertItem
    } else
        act = menu->addAction( itemName , res , member, accel); //insertItem
    return act;
}

void fillLibraries(QComboBox *libs, libfwbuilder::FWObject *obj, bool rw)
{
    bool standardObj = false;
    bool templateObj = false;
    bool deletedObj = false;
    QString lib="";

    if ( ! FWObjectDatabase::isA(obj))
    {
        FWObject *libobj = obj->getLibrary();
        assert(libobj!=NULL);
        lib = libobj->getName().c_str();
        standardObj = (libobj->getId()==FWObjectDatabase::STANDARD_LIB_ID);
        templateObj = (libobj->getId()==FWObjectDatabase::TEMPLATE_LIB_ID);
        deletedObj = (libobj->getId()==FWObjectDatabase::DELETED_OBJECTS_ID);
    }

    libs->clear();
    list<FWObject*> ll = obj->getRoot()->getByType( Library::TYPENAME );
    ll.sort(FWObjectNameCmpPredicate());
    int n=0;
    int cn=0;
    string libicn;
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        if (libicn.empty())
            libicn=(":/Icons/"+(*i)->getTypeName()+"/icon-tree").c_str();

        if ( (*i)->getId()==FWObjectDatabase::STANDARD_LIB_ID &&
             !standardObj) continue;
        if ( (*i)->getId()==FWObjectDatabase::TEMPLATE_LIB_ID &&
             !templateObj) continue;
        if ( (*i)->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  &&
             !deletedObj ) continue;

        if (rw && (*i)->isReadOnly()) continue;
        if (lib==QString((*i)->getName().c_str())) cn=n;
        QPixmap icon;
        icon.load( (libicn).c_str() );

        libs->addItem(icon,
                      QString::fromUtf8((*i)->getName().c_str()) );
        n++;
    }
    libs->setCurrentIndex(cn);
}


void fillLibraries(QListWidget *libs, libfwbuilder::FWObject *obj, bool rw)
{
    bool standardObj = false;
    bool templateObj = false;
    bool deletedObj = false;
    QString lib="";

    if ( ! FWObjectDatabase::isA(obj))
    {
        FWObject *libobj = obj->getLibrary();
        assert(libobj!=NULL);
        lib = libobj->getName().c_str();
        standardObj = (libobj->getId()==FWObjectDatabase::STANDARD_LIB_ID);
        templateObj = (libobj->getId()==FWObjectDatabase::TEMPLATE_LIB_ID);
        deletedObj = (libobj->getId()==FWObjectDatabase::DELETED_OBJECTS_ID);
    }

    libs->clear();
    list<FWObject*> ll = obj->getRoot()->getByType( Library::TYPENAME );
    ll.sort(FWObjectNameCmpPredicate());
    int n=0;
    int cn=0;
    string libicn;
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        if (libicn.empty())
            libicn=Resources::global_res->getObjResourceStr(*i,"icon-tree").c_str();

        if ( (*i)->getId()==FWObjectDatabase::STANDARD_LIB_ID && !standardObj) continue;
        if ( (*i)->getId()==FWObjectDatabase::TEMPLATE_LIB_ID && !templateObj) continue;
        if ( (*i)->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  && !deletedObj ) continue;

        if (rw && (*i)->isReadOnly()) continue;
        if (lib==QString((*i)->getName().c_str())) cn=n;

        QPixmap icon;
        icon.load( (":/"+libicn).c_str() );

        QListWidgetItem *item = new QListWidgetItem(icon, (*i)->getName().c_str());

        libs->addItem(item);

        n++;
    }
    libs->setCurrentRow(cn);
}


bool isTreeReadWrite(QWidget *parent, FWObject *obj)
{
    if (obj->isReadOnly())
    {
        QMessageBox::warning(
            parent, "Firewall Builder",
            QObject::tr("Impossible to apply changes because object is "
                        "located in read-only\npart of the tree or data "
                        "file was opened read-only"),
            QObject::tr("&Continue"), 0, 0,
            0, 2 );

        return false;
    }
    return true;
}

/*
 * compare names as QString objects to catch non-ascii names
 */
bool validateName(QWidget *parent, FWObject *obj, const QString &newname)
{
    FWObject *p = obj->getParent();
    for (FWObject::iterator i=p->begin(); i!=p->end(); ++i)
    {
        FWObject *o1= *i;
        // Another hack: we need to be able to create policy and nat
        // ruleset objects with name "ftp-proxy/*" for PF. Allow
        // objects of different type to have the same name.

        if (o1 == obj || o1->getTypeName() != obj->getTypeName()) continue;

        if (QString(o1->getName().c_str()) == newname)
        {
            QMessageBox::warning(
                parent, "Firewall Builder",
                QObject::tr("Object with name '%1' already exists, "
                            "please choose different name.").
                arg(o1->getName().c_str()),
                QObject::tr("&Continue"), NULL, NULL, 0, 2 );
            return false;
        }
    }
    return true;
}

QString quoteString(const QString &str)
{
    QString res;

    if (str.indexOf(" ")!=-1) res="\"";
    res+=str;
    if (str.indexOf(" ")!=-1) res+="\"";

    return res;
}

QString getUserName()
{
    QString uname;

#ifdef _WIN32

#define INFO_BUFFER_SIZE 32767

TCHAR  infoBuf[INFO_BUFFER_SIZE];
DWORD  bufCharCount = INFO_BUFFER_SIZE;

  bufCharCount = INFO_BUFFER_SIZE;
  if( GetUserName( infoBuf, &bufCharCount ) )
  {
#ifdef UNICODE
      uname = QString::fromUtf16((ushort*)infoBuf);
#else
      uname = QString::fromLocal8Bit(infoBuf);
#endif
  }

  uname = uname.replace(' ','_');

#else

    char *lname = getenv("LOGNAME");
    if (lname!=NULL)
        uname = QString(lname);
    else
    {
        struct passwd *pwd = getpwuid(getuid());
        assert(pwd);
        uname = QString(pwd->pw_name);
    }
#endif
    return uname;
}


QString getFileDir(const QString &file)
{
    int sn1  = file.lastIndexOf("/",-1);
    int sn2  = file.lastIndexOf("\\",-1);
    int sn   = (sn1>=0)?sn1:sn2;
    QString dir;

    if(sn<0) dir = "./";
    else     dir = file.left( sn );

#ifdef _WIN32
/* on windows, if directory is in the root of the drive (like "c:"),
 * I must append "\" to it
 */
    if (dir.indexOf(":")==(dir.length()-1)) dir=dir + "\\";
#endif
    return dir;
}

void setDisabledPalette(QWidget *w)
{
    QPalette    pal=w->palette();

    pal.setCurrentColorGroup( QPalette::Active );
    pal.setColor( QPalette::Text, Qt::black );

    pal.setCurrentColorGroup( QPalette::Inactive );
    pal.setColor( QPalette::Text, Qt::black );

    pal.setCurrentColorGroup( QPalette::Disabled );
    pal.setColor( QPalette::Text, Qt::black );

    w->setPalette( pal );
}

QString getAddrByName(const QString &name, int af_type)
{
    list<InetAddr> results;
    try
    {
        results = DNS::getHostByName(name.toAscii().constData(), af_type);
    } catch (FWException &e)
    {
        if (fwbdebug) qDebug("utils::getAddrByName: DNS lookup error: %s",
                             e.toString().c_str());
        return "";
    }

    try
    {
        if (results.size()>0)
            return QString(results.front().toString().c_str());
    } catch (FWException &e)
    {
        qDebug("utils::getAddrByName: Can not convert address to string");
        qDebug(e.toString().c_str());
    }
    return "";

#if 0
    QHostInfo a = QHostInfo::fromName(name);

    QList<QHostAddress> alist = a.addresses();

    if (alist.empty()) return "";
    return alist.front().toString();
#endif
}

QString getNameByAddr(const QString &addr)
{
    /*QHostAddress ha;
    ha.setAddress(addr);

    Q3Dns qry( ha, Q3Dns::Ptr);

    QStringList nlist = qry.hostNames();

    while (qry.isWorking())
    {
        app->processEvents(200);
    }
    nlist = qry.hostNames();
    if (nlist.empty()) return "";

    return nlist.front();*/
    QHostInfo a = QHostInfo::fromName(addr);

    return a.hostName();
}

QString wordWrap(const QString& s, int maxchinline)
{
    int chcount=0;
    int lastwdpos=0;
    int linestart=0;
    bool fl_wd=true;
    /*unsigned*/ int pos=0;
    QString res="";
    QChar ch;

    for ( ; pos < s.length(); pos++,chcount++)
    {
        ch = s.at(pos);
        if (!ch.isLetter() && !ch.isNumber())
        {
            fl_wd=false;
        } else
        {
            if (!fl_wd)
            {
                fl_wd=true;
                lastwdpos=pos;
            }
        }
        if (chcount>maxchinline)
        {
            if (fl_wd)
            {
                if (linestart<lastwdpos)
                {
                    res.append(s.mid(linestart,lastwdpos-linestart));
                    linestart=lastwdpos;
                    pos=lastwdpos;

                }else
                {
                    res.append(s.mid(linestart,pos-linestart));
                    linestart=pos;
                    lastwdpos=pos;
                }
            }
            else
            {
                res.append(s.mid(linestart,pos-linestart));
                while (++pos< s.length() && s.at(pos).isSpace()) ;
                if (pos<s.length())
                {
                    linestart=pos--;

                }
                else
                {
                    break;
                }
            }
            res.append('\n');
            chcount=0;
        }
    }
    res.append(s.mid(linestart,pos-linestart));
    return res;
}

void loadIcon(QPixmap &pm, libfwbuilder::FWObject *obj)
{
    QString icn_file = (":/Icons/" + obj->getTypeName()+"/icon-tree").c_str();

    if ( ! QPixmapCache::find( icn_file, pm) )
    {
        pm.load( icn_file );
        QPixmapCache::insert( icn_file, pm);
    }
}

void LoadPixmap(const QString path, QPixmap &where)
{
    if ( ! QPixmapCache::find( path, where ) )
    {
        where.load( path );
        if (where.width() == 0)
            qDebug("pixmap load failed: %s", path.toAscii().constData());
        QPixmapCache::insert( path, where );
    }
}

QPixmap LoadPixmap(const QString path)
{

    QPixmap p;
    LoadPixmap(path, p);
    return p;
}

QString calculateIconName(const QString &_icn, bool negation)
{
    QString icn = ":/Icons/" + _icn + "/icon";

    if (negation)
    {
        icn = icn + "-neg";
    }

    if (FWBSettings::SIZE16X16 == st->getIconsInRulesSize())
    {
        return icn+"-tree";
    }

    return icn;
}
