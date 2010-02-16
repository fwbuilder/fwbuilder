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

#include "../../config.h"

#include "Configlet.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Resources.h"

#include <QRegExp>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QtDebug>

#include <iostream>

extern std::string     respath;

using namespace libfwbuilder;
using namespace std;


/*
 * @filename is a name of the configlet file. The program searches for
 * it in resources directory, subdirectory configlets/@prefix. If
 * @filename is absolute path, the program tries to open file as
 * specified.
 */
Configlet::Configlet(const std::string &prefix, const QString &file_name)
{
    reload(prefix, file_name);
}

Configlet::Configlet(const std::string &prefix,
                     const std::string &default_prefix,
                     const QString &file_name)
{
    clear();
    if (!reload(prefix, file_name)) reload(default_prefix, file_name);
}

Configlet::Configlet(FWObject *fw, const std::string &default_prefix,
                     const QString &file_name)
{
    string host_os = fw->getStr("host_OS");
    string os_family = Resources::os_res[host_os]->
        getResourceStr("/FWBuilderResources/Target/family");
    remove_comments = true;
    comment_str = "##";
    collapse_empty_strings = false;
    if (!reload(os_family, file_name)) reload(default_prefix, file_name);
    if (code.size() == 0)
        qCritical() << "Could not open configlet file"
                    << os_family.c_str() << "/" << file_name
                    << "or"
                    << default_prefix.c_str() << "/" << file_name;
}

Configlet::~Configlet()
{
}

void Configlet::clear()
{
    vars.clear();
    remove_comments = true;
    comment_str = "##";
    collapse_empty_strings = false;
}

bool Configlet::reload(const std::string &_prefix, const QString &file_name)
{
    prefix = _prefix.c_str();

    code.clear();

    file_path = getConfigletPath(file_name);

    if (!QFile(file_path).exists())  return false;
    else
    {
        QFile file(file_path);
        if (file.open(QFile::ReadOnly))
        {
            QTextStream ts(&file);
            do
            {
                QString line = ts.readLine();
                code.push_back(line);
            } while (!ts.atEnd());
            removeComments();
            return true;
        }
    }
    return false;
}

QString Configlet::getFullPath(const QString &path)
{
    if (QDir::isRelativePath(path))
        return QString(respath.c_str()) + "/configlets/" + path;
    else
        return path;
}

QString Configlet::getConfigletPath(const QString &configlet_name)
{
    if (QDir::isAbsolutePath(configlet_name)) return configlet_name;

    QString home = QDir::homePath();
    QString file_path;

    file_path = home + "/fwbuilder/configlets/" + prefix + "/" + configlet_name;
    if (QFile(file_path).exists()) return file_path;

    file_path = getFullPath(prefix + "/" + configlet_name);
    return file_path;
}

// ************************************************************************

void Configlet::setVariable(const QString &name, const QString &value)
{
    vars[name] = value.trimmed();
}

void Configlet::setVariable(const QString &name, int value)
{
    QString val;
    val.setNum(value);
    vars[name] = val;
}

QString Configlet::expand()
{
    // Need non-greedy matching so that if_re matches only one {{?var}} ... {{?}}
    // clause
    QRegExp var_re("\\{\\{\\$([^}]*)\\}\\}", Qt::CaseSensitive, QRegExp::RegExp2);
    var_re.setMinimal(true);

    // remove comments before processing {{$var}} and {{if var}} so we can
    // use these in comments
    QString all_code;
    if (remove_comments)
    {
        QStringList res;
        foreach(QString line, code)
        {
            if (line.startsWith(comment_str)) continue;
            res.push_back(line);
        }
        all_code = res.join("\n");
    } else
        all_code = code.join("\n");

    char *configlet_test = getenv("CONFIGLET_DEBUG");

    int counter = 0;
    int pos = 0;
    while ((pos = var_re.indexIn(all_code, pos)) != -1 && counter < 1000)
    {
        QString var = var_re.cap(1);

        if (configlet_test)
        {
            qDebug() << QString("Configlet %1  var '%2'").arg(file_path).arg(var);
        }

        if (vars.count(var) > 0)
        {
            all_code.replace(QString("{{$%1}}").arg(var), vars[var]);
        } else
        {
            // template has a variable that has not been defined
            // remove '$' from the macro but leave it in place for debugging
            qDebug() << QObject::tr("Found undefined variable '%1' in configlet %2")
                .arg(var).arg(file_path);

            all_code.replace(QString("{{$%1}}").arg(var), QString("{{%1}}").arg(var));

        }
        counter++;
    }

    if (counter >= 1000)
        qDebug() << QObject::tr("Configlet expansion stopped by "
                                "infinite loop protector. "
                                "Check configlet syntax. %1").arg(file_path);

    counter = 0;
    while (processIf(all_code, 0) && counter < 1000) counter++;

    if (counter >= 1000)
        qDebug() << QObject::tr("Configlet expansion stopped by "
                                "infinite loop protector. "
                                "Check configlet syntax. %1").arg(file_path);

    if (collapse_empty_strings)
    {
        QStringList res;
        foreach(QString line, all_code.split("\n"))
        {
            if (line.trimmed().isEmpty()) continue;
            res.push_back(line);
        }
        return res.join("\n");
    }

    return all_code;
}

/*
 * pos points to the position of "{{?var}}" in the stream
 */
bool Configlet::processIf(QString &stream, int pos)
{
    QRegExp if_re("\\{\\{if {1,}([^}]{1,})\\}\\}", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp endif_re("\\{\\{endif\\}\\}", Qt::CaseSensitive, QRegExp::RegExp2);
    if_re.setMinimal(true);
    endif_re.setMinimal(true);
    int current_if_pos = if_re.indexIn(stream, pos);
    if (current_if_pos == -1) return false;

    int current_if_length = if_re.cap(0).length();
    QString current_if_var = if_re.cap(1);

    // look what is next, another opening if or closing endif
    int next_if_pos = if_re.indexIn(
        stream, current_if_pos + current_if_length);
    int next_endif_pos = endif_re.indexIn(
        stream, current_if_pos + current_if_length);

    if (next_if_pos != -1 && next_if_pos < next_endif_pos)
    {
        processIf(stream, next_if_pos);
        // the next if statement has been replaced, process current if
        // statement again
        return true;
    } 

    if (next_endif_pos != -1)
    {
        int next_endif_length = endif_re.cap(0).length();
        // current if statement starts at current_if_pos
        // and ends at next_endif_pos + next_endif_length
        int current_if_clause_length =
            next_endif_pos + next_endif_length - current_if_pos;
        QString body = stream.mid(
            current_if_pos + current_if_length,
            next_endif_pos - current_if_pos - current_if_length);

        QString replacement;
        if (vars.count(current_if_var) > 0)
        {
            bool ok = false;
            int f = vars[current_if_var].toInt(&ok);
            if (ok && f)
                replacement = body;
        }
        stream.replace(current_if_pos, current_if_clause_length, replacement);
    }
    return true;
}


/**
 * Set internal flag to remove comments from the produced script and
 * set comment string. By default comment string is "##". This means
 * we can still put comments with single "#" in templates and have
 * these comments appear in the generated script. At the same time,
 # comments marked with "##" will be removed.
 */
void Configlet::removeComments(const QString &_str)
{
    remove_comments = true;
    comment_str = _str;
}

void Configlet::collapseEmptyStrings(bool f)
{
    collapse_empty_strings = f;
}
