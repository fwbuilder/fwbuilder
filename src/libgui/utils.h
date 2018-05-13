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


#ifndef  __UTILS_H_
#define  __UTILS_H_


#include <string>
#include <list>
#include <functional>

#include <qglobal.h>
#include <QtWidgets/QMenu>

class QObject;
class QWidget;
class QMenu;
class QComboBox;
class QListWidget;

#include <qkeysequence.h>
#include <qstring.h>
#include <qmap.h>
#include <utility>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWReference.h"

typedef std::pair<const QString,QString> QStringPair;

// a predicate used to compare first string in pair<QString,QString>
// use with find_if
class findFirstInQStringPair {
    QString str;
    public:
    findFirstInQStringPair(const QString &d) { str=d; }
    bool operator()(std::pair<const QString,QString> &_d)
    { return (str == _d.first); }
};

class findSecondInQStringPair {
    QString str;
    public:
    findSecondInQStringPair(const QString &d) { str=d; }
    bool operator()(std::pair<const QString,QString> &_d)
    { return (str == _d.second); }
};



extern QAction* addPopupMenuItem(QObject *res,
                            QMenu* menu, //it was a Q3PopupMenu object
                            const QString &resourceIconPath,
                            const QString itemName,
                            const char* member,
                            const QKeySequence &accel = 0);

extern void fillLibraries(QComboBox *libs, libfwbuilder::FWObject *obj,
                          bool rw=false);
extern void fillLibraries(QListWidget *libs, libfwbuilder::FWObject *obj,
                          bool rw=false);


/**
 * this is a convenience method that checks if the object tree is
 * read-only and shows appropriate error dialog. This method is
 * there so we don't have to repeat the same code in each object
 * class dialog.
 */

extern bool isTreeReadWrite(QWidget *parent, libfwbuilder::FWObject *obj);

/**
 * this function checks if the name of the object 'obj' is a duplicate
 * by scanning all children objects of its parent and comparing their
 * names. It shows pop-up dialog letting user know if the same name
 * was found, and returns false. It returns true otherwise.
 */
extern bool validateName(QWidget *parent,
                         libfwbuilder::FWObject *obj,
                         const QString &newname);

/**
 * returns a copy of the string str, enclosed in quotes if it contains
 * whitespaces
 */
extern QString quoteString(const QString &str);

extern QString getFileDir(const QString &file);

extern void loadIcon(QPixmap &pm, libfwbuilder::FWObject *obj);

extern void setDisabledPalette(QWidget *w);

extern QString getAddrByName(const QString &name, int af_type);
extern QString getNameByAddr(const QString &addr);
extern QString wordWrap(const QString& ,int);

extern void LoadPixmap(const QString &path, QPixmap &where);
extern QPixmap LoadPixmap(const QString &path);
extern QString calculateIconName(const QString &_icn, bool negation);

extern void parseCommandLine(const QString &cmd, QStringList &argv);

QStringList sortStrings(const QStringList &list);

/*
 * @icon_size:  0 - small (16x16) , 1 - medium (25x25), 2 - big (64x64)
 */
extern void doSetObjectIcon(libfwbuilder::FWObject *obj, QPixmap *pm, int icon_size);

#endif
