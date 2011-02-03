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

#include "../../config.h"

#include "CompilerDriver.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"

#include <QFileInfo>
#include <QString>

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
                                              Firewall *current_fw,
                                              bool cluster_member)
{
    QString current_firewall_name = 
        QString::fromUtf8(current_fw->getName().c_str());

    if (cluster_member)
    {
        // member of a cluster
        QString fw_id = objdb->getStringId(current_fw->getId()).c_str();
        if (member_file_names.contains(fw_id))
        {
            fw_file_name = getOutputFileNameInternal(
                current_fw,
                member_file_names[fw_id],
                "output_file", current_firewall_name, ".fw");

        } else
        {
            fw_file_name = getOutputFileNameInternal(
                current_fw,
                "", "output_file", current_firewall_name, ".fw");
        }

    } else
    {
        // standalone firewall
        fw_file_name = getOutputFileNameInternal(
            current_fw,
            file_name_setting_from_command_line,
            "output_file", current_firewall_name, ".fw");
    }

    FWOptions* options = current_fw->getOptionsObject();
    QString name_from_option =
        QString::fromUtf8(options->getStr("conf1_file").c_str());
    if (!name_from_option.isEmpty()) conf1_file_name = name_from_option;
    else
    {
        conf1_file_name = getConfFileNameFromFwFileName(fw_file_name, ".conf");
    }

    if (prepend_cluster_name_to_output_file && cluster_member && cluster!=NULL)
    {
        fw_file_name = QString("%1_%2")
            .arg(QString::fromUtf8(cluster->getName().c_str()))
            .arg(fw_file_name);
        conf1_file_name = QString("%1_%2")
            .arg(QString::fromUtf8(cluster->getName().c_str()))
            .arg(conf1_file_name);
    }

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
        QString::fromUtf8(options->getStr(option_name.toStdString()).c_str());

    if (!name_from_option.isEmpty()) return name_from_option;
    else return fw_name + ext;
}

QString CompilerDriver::getConfFileNameFromFwFileName(const QString &file_name,
                                                      const QString &ext)
{
    QString res;
    QFileInfo fi(file_name);
    if (fi.isRelative())
    {
        res = QString(fi.completeBaseName() + ext);
    } else
    {
        res = QString(fi.path() + "/" + fi.completeBaseName() + ext);
    }
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

