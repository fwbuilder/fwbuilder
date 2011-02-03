/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#ifndef __CONFIGLET_HH__
#define __CONFIGLET_HH__

#include <QString>
#include <QStringList>
#include <QMap>

namespace libfwbuilder {
    class FWObject;
};


class Configlet {

    bool processIf(QString &stream, int pos);
    
protected:

    QString name;
    QString prefix;
    QString file_path;
    QStringList code;
    QMap<QString, QString> vars;
    bool remove_comments;
    QString comment_str;
    bool collapse_empty_strings;

    static bool configlet_debugging;
    static QString begin_marker;
    static QString end_marker;
    
    QString getFullPath(const QString &rel_path);
    QString getConfigletPath(const QString &configlet_name);
    
public:

    Configlet(const std::string &prefix, const QString &filename);
    Configlet(const std::string &prefix, const std::string &default_prefix,
              const QString &filename);
    Configlet(libfwbuilder::FWObject *fw, const std::string &default_prefix,
              const QString &filename);
    virtual ~Configlet();

    bool reload(const std::string &prefix, const QString &filename);
    void clear();

    void setVariable(const QString &name, const QString &value);
    void setVariable(const QString &name, int value);

    QString expand();

    void removeComments(const QString &comment_str="##");
    void collapseEmptyStrings(bool f);

    /*
     * the following methods are used in unit tests
     */
    static void setDebugging(bool f) { configlet_debugging = f; }
    static QString findGeneratedText(const QString &configlet_name,
                                     const QString &text,
                                     int nth=1);
    static QString findConfigletInFile(const QString &configlet_name,
                                       const QString &file_path,
                                       int nth=1);
};

#endif
