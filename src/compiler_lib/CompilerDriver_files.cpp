/* 

                          Firewall Builder

                 Copyright (C) 2009-2011 NetCitadel, LLC

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


#include "CompilerDriver.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"

#include <QFileInfo>
#include <QString>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

 
/**
 *  Determine output file name. If compiling standalone firewall, the
 *  name can be enforced via -o command line switch in which case it
 *  is stored in file_name_setting_from_command_line. If not,
 *  determine automatically using firewall name.
 *
 *  The name is determined as follows:
 *
 *  - if file_name_setting_from_command_line is not empty, it is used
 *  - if file_name_setting_from_command_line is empty, check firewall option
 *    "output_file" and use it if it is not empty
 *  - if it is empty, generate the name using firewall object name and suffix .fw
 * 
 *  If compiling a cluster, the name could have been enforced via -O
 *  command line switch, in which case it will be found in
 *  member_file_names. If not, determine automatically using the same
 *  algorithm as for the standalone firewall (see above).
 * 
 * This function also determines the name of the .conf file we should
 * generate. Used only by compielers that generate multiple
 * files. There is no cli switch to set this name, so the name is
 * taken from firewall option "conf1_file" or derived from
 * fw_file_name
 *
 * This function stores script file name in variable fw_file_name and
 * conf file name in conf1_file_name
 */
void CompilerDriver::determineOutputFileNames(Cluster *cluster,
                                              Firewall *fw,
                                              bool cluster_member,
                                              const QStringList &suffixes,
                                              const QStringList &extensions,
                                              const QStringList &remote_name_fw_options)
{
    file_names.clear();
    remote_file_names.clear();

    assert(suffixes.size()==extensions.size());
    assert(suffixes.size()==remote_name_fw_options.size());

    foreach(QString ext, extensions)
    {
        file_names << "";
        remote_file_names << "";
    }

    QString firewall_name =
        QString::fromUtf8(fw->getName().c_str());

    if (cluster_member)
    {
        // member of a cluster
        QString fw_id = objdb->getStringId(fw->getId()).c_str();
        if (member_file_names.contains(fw_id))
        {
            file_names[FW_FILE] = getOutputFileNameInternal(
                fw,
                member_file_names[fw_id], "output_file", firewall_name,
                extensions[FW_FILE]);

        } else
        {
            file_names[FW_FILE] = getOutputFileNameInternal(
                fw,
                "", "output_file", firewall_name,
                extensions[FW_FILE]);
        }

    } else
    {
        // standalone firewall
        file_names[FW_FILE] = getOutputFileNameInternal(
            fw,
            file_name_setting_from_command_line, "output_file", firewall_name,
            extensions[FW_FILE]);
    }

    FWOptions* options = fw->getOptionsObject();

    if (suffixes.size() > 1)
    {
        // if we need to deal with conf files at all ...
        // skip item 0 since it is the .fw file
        for (int i=1; i<file_names.size(); ++i)
        {
            QString opt_name("conf%1_file");
            QString name_from_option =
                QString::fromUtf8(options->getStr(
                                      opt_name.arg(i).toStdString()).c_str());

            if (!name_from_option.isEmpty())
            {
                // user provided a name for the conf1 file in the
                // firewall settings dialog.
                file_names[i] = name_from_option;
            } else
            {
                // special-case file names for the 2-d and subsequent conf
                // files: if we have the name for the first conf file from
                // fw option, use it as a prototype, otherwise use fw file
                // name as a prototype. This is useful when user specifies
                // the name for pf.conf file, this name will be used as a
                // prototype for anchor .conf file names instead of the fw
                // file name
                if (i >= CONF2_FILE)
                    file_names[i] = getConfFileNameFromFwFileName(
                        file_names[CONF1_FILE], extensions[i]);
                else
                    file_names[i] = getConfFileNameFromFwFileName(
                        file_names[FW_FILE], extensions[i]);
            }

        }

        // file_names at this point is like this:
        // file_names= ("ipf4.fw", "ipf4.conf", "ipf4.conf") 
        //
        // qDebug() << "file_names=" << file_names;

        // suffixes are inserted right before the file extension, such as in
        // firewall-suffix.conf
        for (int i=1; i<suffixes.size(); ++i)
        {
            QString sfx = suffixes[i];
            if (!sfx.isEmpty())
            {
                QFileInfo fi(file_names[i]);
                QString path = fi.path();
                if (path == ".")
                {
                    file_names[i] = fi.completeBaseName() +
                        "-" + sfx + "." + fi.suffix();
                } else
                {
                    file_names[i] = path + "/" + fi.completeBaseName() +
                        "-" + sfx + "." + fi.suffix();
                }
            }
        }

        // file_names at this point is like this:
        // file_names= ("ipf4.fw", "ipf4-ipf.conf", "ipf4-nat.conf") 
        //
        //qDebug() << "file_names=" << file_names;

    }

    if (prepend_cluster_name_to_output_file && cluster_member && cluster!=nullptr)
    {
        for (int i=0; i<file_names.size(); ++i)
        {
            file_names[i] = QString("%1_%2")
                .arg(QString::fromUtf8(cluster->getName().c_str()))
                .arg(file_names[i]);
        }
    }

    // Determine remote file names using fw options, if any. If option
    // has not been specificed in the list remote_name_fw_options
    // (list item is empty string), or the option value is empty, then
    // guess using firewall_dir option and local file name

    QString fw_dir = options->getStr("firewall_dir").c_str();

    if (fw_dir.isEmpty()) fw_dir = Resources::getTargetOptionStr(
        fw->getStr("host_OS"), "activation/fwdir").c_str();

    for (int i=0; i<file_names.size(); ++i)
    {
        QString fn = file_names[i];
        if (remote_name_fw_options[i].isEmpty())
            remote_file_names[i] = fw_dir + "/" + QFileInfo(fn).fileName();
        else
        {
            QString remote_file_name_from_fw_option =
                options->getStr(remote_name_fw_options[i].toStdString()).c_str();
            if (remote_file_name_from_fw_option.isEmpty())
            {
                remote_file_names[i] = fw_dir + "/" + QFileInfo(fn).fileName();
            } else
                remote_file_names[i] = remote_file_name_from_fw_option;
        }
    }

    //qDebug() << remote_file_names;
}


QString CompilerDriver::getOutputFileNameInternal(Firewall *current_fw,
                                                  const QString &from_cli,
                                                  const QString &option_name,
                                                  const QString &fw_name,
                                                  const QString &ext)
{
    if (!from_cli.isEmpty()) return from_cli;

    FWOptions* options = current_fw->getOptionsObject();
    QString name_from_option =
        QString::fromUtf8(options->getStr(option_name.toStdString()).c_str()).trimmed();

    if (!name_from_option.isEmpty()) return name_from_option;
    else return fw_name + "." + ext;
}

QString CompilerDriver::getConfFileNameFromFwFileName(const QString &file_name,
                                                      const QString &ext)
{
    QString res;
    QFileInfo fi(file_name);
    QString path = fi.path();

    if (path == ".")
        res = fi.completeBaseName() + "." + ext;
    else
        res = path + "/" + fi.completeBaseName() + "." + ext;

    return res;
}

/*
 * Replace ' ' with '\ ' in the string. 
 *
 * DO NOT CHANGE WITHOUT EXTENSIVE TESTING OF THE POLICY INSTALLER !
 *
 * This is used in different places to deal with generated files with
 * spaces in the name. This can happen if firewall object name has
 * spaces. This method of escaping the space is used when we generate
 * manifest line in the .fw file to find other generated files. The
 * same method is also used to generate command line arguments for scp
 * where spaces cause all sorts of problems, for example scp can't
 * find the file to copy or issues "ambiguous target" error when
 * remote file name has a space. Finally, the same method is used to
 * escape space in the file name before using it for the configlet
 * variable because this name is used in the shell script that we run
 * on the firewall.
 */
QString CompilerDriver::escapeFileName(QString fileName)
{
    return fileName.replace(' ', "\\ ");
}

QString CompilerDriver::unescapeFileName(QString fileName)
{
    return fileName.replace("\\ ", " ");
}

