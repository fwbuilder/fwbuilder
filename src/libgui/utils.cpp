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
#include <qpixmapcache.h>

#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QPixmap>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QtDebug>

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

#include "FWBTree.h"

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
    QAction *act = nullptr;

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
        assert(libobj!=nullptr);
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
        assert(libobj!=nullptr);
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
    if (newname.isEmpty())
    {
        // show warning dialog only if app has focus
        if (QApplication::focusWidget() != nullptr)
        {
            parent->blockSignals(true);
            
            QMessageBox::warning(
                parent, "Firewall Builder",
                QObject::tr("Object name should not be blank"),
                QObject::tr("&Continue"), nullptr, nullptr, 0, 2 );
                
            parent->blockSignals(false);
        }
        return false;
    }

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
            /*
             * when we open this warning dialog, the dialog class
             * loses focus and obj_name lineEdit widget sends signal
             * "editingfinished" again.  To the user this looks like the
             * warning dialog popped up twice (in fact two copies of the
             * same warning dialog appear at the same time, one exactly on
             * top of another). To avoid this, block signals for the
             * duration while we show the dialog. Note that documentation
             * does not mention that QObject::blockSignals() affects not
             * only the widget but all its children, but it seems to work
             * that way. Tested with Qt 4.6.1. See #1171
             */

            // show warning dialog only if app has focus
            if (QApplication::focusWidget() != nullptr)
            {
                parent->blockSignals(true);
            
                QMessageBox::warning(
                    parent, "Firewall Builder",
                    QObject::tr("Object with name '%1' already exists, "
                                "please choose different name.").
                    arg(o1->getName().c_str()),
                    QObject::tr("&Continue"), nullptr, nullptr, 0, 2 );
                
                parent->blockSignals(false);
            }
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

QString getFileDir(const QString &file)
{
    return QFileInfo(file).dir().path();
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
        results = DNS::getHostByName(name.toLatin1().constData(), af_type);
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
        qDebug() << "utils::getAddrByName: Can not convert address to string";
        qDebug() << e.toString().c_str();
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

void loadIcon(QPixmap &pm, FWObject *obj)
{
    QString icn_file = (":/Icons/" + obj->getTypeName()+"/icon-tree").c_str();
    LoadPixmap(icn_file, pm);

    // if ( ! QPixmapCache::find( icn_file, pm) )
    // {
    //     pm.load( icn_file );
    //     QPixmapCache::insert( icn_file, pm);
    // }
}

void LoadPixmap(const QString &path, QPixmap &pm)
{
    if ( ! QPixmapCache::find( path, pm ) )
    {
        pm.load( path );
        if (pm.width() == 0)
            qDebug("pixmap load failed: %s", path.toLatin1().constData());
        QPixmapCache::insert( path, pm );
    }
}

QPixmap LoadPixmap(const QString &path)
{

    QPixmap p;
    LoadPixmap(path, p);
    return p;
}

QString calculateIconName(const QString &_icn, bool negation)
{
    // if _icn has white space, use only the first word
    QStringList icnl = _icn.split(" ");

    QString icn = ":/Icons/" + icnl[0] + "/icon";

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

void doSetObjectIcon(FWObject *obj, QPixmap *pm, int icon_size)
{
    QString icn_alias;
    QString icn_sfx;

    switch (icon_size)
    {
    case 0: icn_sfx = "icon-tree"; break;
    case 2: icn_sfx = "icon-big"; break;
    default: icn_sfx = "icon"; break;
    }

    // note that we do not have "locked" version of large icons
    if (obj->getRO() && icon_size != 2)
        icn_alias = ":/Icons/lock";
    else
    {
        if (FWBTree().isStandardFolder(obj))
            icn_alias = ":/Icons/SystemGroup/" + icn_sfx;
        else
            icn_alias = QString(":/Icons/") + obj->getTypeName().c_str() + "/" + icn_sfx;
    }

    LoadPixmap(icn_alias, *pm);
}

QString _getNextToken(QStringList &args)
{
    QString a;
    while (args.size() > 0)
    {
        a = args.front();
        args.pop_front();
        if (!a.isEmpty()) break;
    }
    return a;
}

// reassemble quoted strings from the list of tokens, possibly
// recursively if there are quoted strings inside. The first token is
// assumed to be " or '. Pops all processed tokens from the list but
// leaves the rest in it.
QString _parseTokens(QStringList &args, const QChar closing_quote='\0')
{
    QString a = _getNextToken(args);
    if (args.size() == 0) return a;

    if (closing_quote != QChar('\0') && a.endsWith(closing_quote))
        return a;

    if (a.startsWith("\"") || a.startsWith("'"))
    {
        QStringList res;
        QChar closing_quote = a[0];

        res.append(a);

        while (!a.endsWith(closing_quote) && args.size() > 0)
        {
            a = _parseTokens(args, closing_quote);
            res.append(a);
        }

        return res.join(" ");
    }

    return a;
}

/**
 * parse command line for ssh or scp given by user in the global
 * preferences dialog. The challenge is to be able to handle situation
 * when the program is installed in directory with a whitespace in the
 * name, so we can't just split the string by a " ".
 *
 * Recognize the following constructs:
 *
 * /path/to/program/program -arg1 val1 -arg2 val2 -arg3 "value 3"
 *
 * everything before the first "-" is considered executable name
 * (possibly with full path)
 * 
 * word that starts with "-" or "/" preceded by a space is an argument
 * argument may have an optional parameter
 * parameter may be quoted using double or single quotes
 */
void parseCommandLine(const QString &cmd, QStringList &argv)
{
    int first_arg = cmd.indexOf(QRegExp(" *-"));
    if (first_arg == -1)
    {
        // no arguments
        argv.append(cmd.trimmed());
        return;
    }
    QString program = cmd.mid(0, first_arg).trimmed();
    if (!program.isEmpty()) argv.append(program);

    QStringList args = cmd.mid(first_arg).split(QRegExp("\\s+"));
//                                                QString::SkipEmptyParts);
    // splits like this:
    // ["", "-arg1", "val1", "-arg2", "\"value", "2", "\""]

    while (args.size() > 0)
    {
        QString t = _parseTokens(args);

        // remove quotes from quoted strings
        if ((t.startsWith('\"') && t.endsWith('\"')) ||
            (t.startsWith('\'') || t.endsWith('\'')))
        {
            t = t.mid(1, t.length()-2);
        }
        argv.append(t);
   }
}


static bool stringsCompare(const QString &a, const QString &b)
{
    QString aLower = a.toLower();
    QString bLower = b.toLower();
    int result = aLower.localeAwareCompare(bLower);
    if (result == 0) {
        return a.localeAwareCompare(b) < 0;
    } else {
        return result < 0;
    }
}


QStringList sortStrings(const QStringList &list)
{
    QStringList ret = list;
    qSort(ret.begin(), ret.end(), stringsCompare);
    return ret;
}
