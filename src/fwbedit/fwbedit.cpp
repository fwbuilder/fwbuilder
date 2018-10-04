/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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


#include "fwbuilder/Constants.h"

#include <qsettings.h>
#include <qdatetime.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>


#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#include <QCommandLineParser>

#include "fwbuilder/Resources.h"

#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Interface.h"

#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include "fwbuilder/Constants.h"

#include "../common/init.cpp"

#include "fwbedit.h"
#include "upgradePredicate.h"

#include "FWWindow.h"
#include "FWBSettings.h"
#include "FWBApplication.h"

#include <QStringList>
#include <QFile>


using namespace libfwbuilder;
using namespace std;


FWWindow *mw = nullptr; 
FWBSettings *st = nullptr; 
FWBApplication *app = nullptr; 

string cmd_str = "";
command cmd = NONE;

bool autoupgrade_flag = false;
string filename = "";
string filemerge = "";
int conflict_res = 1;

vector<string> platforms;

FWObjectDatabase *objdb = nullptr;

int fwbdebug = 0;


void list_attributes()
{
    cout << "Attributes for the new objects, by type:" << endl;
    cout << endl;
    cout << "      "
         << " -t " <<Firewall::TYPENAME
         << " -a platform, host OS" << endl;
    cout << "      "
         << " -t " <<Cluster::TYPENAME
         << " -a platform, host OS" << endl;
    cout << "      "
         << " -t " <<IPv4::TYPENAME
         << " -a IP address [,netmask]" << endl;
    cout << "      "
         << " -t " <<IPv6::TYPENAME
         << " -a IPv6 address [,masklen]" << endl;
    cout << "      "
         << " -t " <<DNSName::TYPENAME
         << " -a DNS record,run time" << endl;
    cout << "      "
         << " -t " <<AddressRange::TYPENAME
         << " -a start address, end address" << endl;
    cout << "      "
         << " -t " <<ObjectGroup::TYPENAME
         <<  endl;
    cout << "      "
         << " -t " <<Network::TYPENAME
         << " -a address,netmask" << endl;
    cout << "      "
         << " -t " <<NetworkIPv6::TYPENAME
         << " -a ipv6_address,netmask_length" << endl;
    cout << "      "
         << " -t " <<Interval::TYPENAME
         << " -a start time,start date,start day,end time, end date, end day" << endl;
    cout << "      "
         << " -t " <<Interface::TYPENAME
         << " -a security level,address type "
        "(dynamic or unnumbered),management" << endl;
    cout << "      "
         << " -t " <<Host::TYPENAME
         <<  endl;
    cout << "      "
         << " -t " <<ServiceGroup::TYPENAME
         <<  endl;
    cout << "      "
         << " -t " <<TCPService::TYPENAME
         << " -a source port range start,end,destination port range start,end,UAPRSF,UAPRSF" << endl;
    cout << "      "
         << " -t " <<UDPService::TYPENAME
         << " -a source port range start,end,Destination port range start,end" << endl;
    cout << "      "
         << " -t " <<ICMPService::TYPENAME
         << " -a ICMP type,ICMP code" << endl;
    cout << "      "
         << " -t " <<IPService::TYPENAME
         << " -a protocol "
        "number,lsrr/ssrr/rr/ts/fragm/short_fragm" << endl;
}


void usage()
{
    cout << "Firewall Builder:  general purpose object tree editing tool"
         << endl;
    cout << "Version " << VERSION << endl;
    cout << endl;
    cout << "Usage: fwbedit command [options]" << endl;
    cout << endl;

    cout << "Command is one of:" << endl;
    cout << "      new         create new object" << endl;
    cout << "      delete      delete object" << endl;
    cout << "      modify      modify object" << endl;
    cout << "      list        print object" << endl;
    cout << "      add         add object to a group" << endl;
    cout << "      remove      remove object from a group" << endl;
    cout << "      upgrade     upgrade data file" << endl;
    cout << "      checktree   check object tree and repair if necessary" << endl;
    cout << "      merge       merge one data file into another" << endl;
    cout << "      import      import firewall configuration (iptables, CIsco IOS," << endl;
    cout << "                  Cisco PIX, ASA and FWSM)" << endl;
    cout << endl;
    cout << "Type   'fwbedit command' to get summary of options for the command"
         << endl;
    cout << endl;
}

void usage_new()
{
    cout << "Options:" << endl;
    cout << endl;
    cout <<
        "       new -f file.fwb -t objtype -n name -c comment "
        "-p parent [-a attrs]\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -t objtype: create a new object of this type\n"
        "           -n name: the name of the new object\n"
        "           -c txt:  specify comment for the new object\n"
        "           -a attribute1[,attribute2...]  :  specify attributes that\n"
        "              define parameters of the new object (see below)\n";
    cout << endl;

    list_attributes();
}

void usage_delete()
{
    cout <<
        "    delete -f file.fwb -o object\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -o object: object to be deleted, full path or ID\n";
    cout << endl;
}

void usage_modify()
{
    cout <<
        "    modify -f file.fwb -o object -c comment [-a attrs]\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -o object: object to be modified, full path or ID\n"
        "           -c txt:  specify comment for the new object\n"
        "           -a attribute1[,attribute2...]  :  specify attributes that\n"
        "              define parameters of the new object (see below)\n";
    cout << endl;
}

void usage_list()
{
    cout <<
        "      list -f file.fwb -o object [-r|-c] [-d|-Fformat]\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -o object: object to print, full path or ID\n"
        "           -r print given object and all object below it in the tree\n"
        "           -c print all children of given object but not the object\n"
        "              itself\n"
        "           -d print full dump of all object's attributes including\n"
        "              internal debugging information if available\n"
        "           -F format_string print according to the format; see\n"
        "              man fwbedit(1) for the list of macros and examples\n";
    cout << endl;
}

void usage_add()
{
    cout <<
        "       add -f file.fwb -g group -o object\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -g group: group the object should be added to, \n"
        "              full path or ID\n"
        "           -o object: object to be added, full path or ID\n";
    cout << endl;
}

void usage_remove()
{
    cout <<
        "   remove -f file.fwb -g group -o object\n"
        "\n"
        "          -f file.fwb: data file\n"
        "          -g group: group the object should be removed from,\n"
        "             full path or ID\n"
        "          -o object: object to be removed, full path or ID\n";
    cout << endl;
}

void usage_upgrade()
{
    cout <<
        "   upgrade -f file.fwb\n"
        "\n"
        "          -f file.fwb: data file\n";
    cout << endl;
}

void usage_checktree()
{
    cout <<
        "   checktree -f file.fwb\n"
        "\n"
        "          -f file.fwb: data file\n";
    cout << endl;
}

void usage_merge()
{
    cout <<
        "   merge -f file1.fwb -i file2.fwb -c[1|2]\n"
        "\n"
        "          -f file1.fwb: data file #1\n"
        "          -i file2.fwb: data file #2\n"
        "          -cN in case of conflict (the same object is found in both files),\n"
        "           keep the object from the file N (can be '1' or '2').\n"
        "           Default is '1'.\n"
        "           Objects from the file2.fwb are merged with objects in file1\n"
        "           and combined object tree saved in file1.fwb\n";
    cout << endl;
}

void usage_import()
{
    cout <<
        "   import -f file1.fwb -i firewall_config.txt -o firewall_object_path [-d]\n"
        "\n"
        "          -f file1.fwb: data file #1\n"
        "          -i firewall_config.txt: firewall configuration file that\n"
        "             should be imported #2\n"
        "          -o firewall_object_path: a full path to the firewall object\n"
        "             to be created, e.g. '/User/Firewalls/my_new_firewall'\n"
        "             Note that path must start with the library name\n"
        "          -d reuse address and service objects created in the process\n"
        "             of import\n";
    cout << endl;
}


void _findObjects(const string &obj_path, FWObject *obj, list<FWObject*> &res)
{
    string path = fixPath(obj_path);
    if (obj->getPath()==path) res.push_back(obj);
    for (FWObject::iterator it=obj->begin(); it!=obj->end(); ++it)
    {
        if (FWReference::cast(*it)) continue;
        _findObjects(path, *it, res);
    }
}

string fixPath(const string &obj_path)
{
    string res = obj_path;
    // add leading "/" if it is not there
    if (res[0]!='/') res = string("/") + res;
            
    // strip trailing "/"
    if (res[res.length()-1] == '/')
        res = res.substr(0, res.length()-1);

    if (res.find("/FWObjectDatabase")!=0)
        res = string("/FWObjectDatabase") + res;

    return res;
}

void findObjects(const string &obj_path, FWObject *obj, list<FWObject*> &res)
{
    if (obj_path.find('/')==string::npos)
    {
        int id = FWObjectDatabase::getIntId(obj_path);
        if (id>=0)
        {
            FWObject *o = obj->getRoot()->findInIndex(id);
            if (o)
            {
                res.push_back(o);
                return;
            }
        }
    }

    string path = fixPath(obj_path);
    _findObjects(path, obj, res);
}

int splitStr(char ch,string s, operands * ops)
{
   int res=0;
   string::size_type pos;
   ops->clear();
   if (s.length()>0)
   {
       while((pos=s.find_first_of(ch))!=string::npos)
       {
           ops->push_back(s.substr(0,pos));
           s=s.substr(pos+1);
           res++;
       }
       ops->push_back(s);
       res++;
   }
           
   return res;
}

string getNextOpt(operands &ops)
{
    operands::iterator it = ops.begin();
    if (it == ops.end())
        throw OperandsError();

    string s = *it;
    ops.pop_front();
    return s;    
}

bool getBool(string s)
{
    return (s.find("y")!=string::npos) || (s.find("Y")!=string::npos) || (s.find("1")!=string::npos);
}



int main(int argc, char * const *argv)
{   
    operands ops;

    string objtype;
    string name;
    string object;
    string group;
    string parent;
    string comment_txt;
    bool list_children = false;
    bool recursive = false;
    string list_format = "%path%";
    bool full_dump = false;
    string import_config;
    bool deduplicate = false;

    if (argc<=1)
    {
        usage();
        exit(1);
    }

    QStringList arguments;
    for (int i = 0; i < argc; ++i) {
        arguments.append(QString(argv[i]));
    }

    /*
     *  Command line format:
     *  fwbedit command [options]
     *
     *  argv[1] is always command
     */

    cmd_str = arguments.at(1).toStdString();

    cmd = NONE;
    if (cmd_str=="new") cmd = NEWOBJECT;
    if (cmd_str=="delete") cmd = DELOBJECT;
    if (cmd_str=="modify") cmd = MODOBJECT;
    if (cmd_str=="add") cmd = ADDGRP;
    if (cmd_str=="remove") cmd = REMGRP;
    if (cmd_str=="list") cmd = LIST;
    if (cmd_str=="upgrade") cmd = UPGRADE;
    if (cmd_str=="checktree") cmd = STRUCT;
    if (cmd_str=="merge") cmd = MERGE;
    if (cmd_str=="import") cmd = IMPORT;

    QCommandLineParser parser;

    QCommandLineOption fileNameOption("f", "filename", "filename");
    QCommandLineOption objectTypeOption("t", "objtype", "objtype");
    QCommandLineOption nameOption("n", "name", "name");
    QCommandLineOption commentOption("c", "comment", "comment");
    QCommandLineOption parentOption("p", "parent", "parent");
    QCommandLineOption attributesOption("a", "attributes", "attributes");
    QCommandLineOption objectOption("o", "object", "object");
    QCommandLineOption formatOption("F", "format", "format");
    QCommandLineOption groupOption("g", "group", "group");

    switch (cmd)
    {
    case NEWOBJECT:
    {
        // -f file.fwb -t objtype -n name -c comment -p parent [-a attrs]

        parser.addOption(fileNameOption);
        parser.addOption(objectTypeOption);
        parser.addOption(nameOption);
        parser.addOption(commentOption);
        parser.addOption(parentOption);
        parser.addOption(attributesOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        if (filename.empty()) {
            usage_new();
            exit(1);
        }

        objtype = parser.value(objectTypeOption).toStdString();
        name = parser.value(nameOption).toStdString();
        comment_txt = parser.value(commentOption).toStdString();
        parent = parser.value(parentOption).toStdString();
        auto attributes = parser.value(attributesOption).split(',');
        for (const QString& attribute : attributes) {
            ops.push_back(attribute.toStdString());
        }

        break;
    }

    case DELOBJECT:
    {
        // -f file.fwb -o object_def
        // object_def can be either full path or object ID

        parser.addOption(fileNameOption);
        parser.addOption(objectOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        object = parser.value(objectOption).toStdString();

        if (filename.empty() || object.empty()) {
            usage_delete();
            exit(1);
        }

        break;
    }
    case MODOBJECT:
    {
        // -f file.fwb -o object -c comment [-a attrs]

        parser.addOption(fileNameOption);
        parser.addOption(objectOption);
        parser.addOption(commentOption);
        parser.addOption(attributesOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        object = parser.value(objectOption).toStdString();
        comment_txt = parser.value(commentOption).toStdString();
        auto attributes = parser.value(attributesOption).split(',');
        for (const QString& attribute : attributes) {
            ops.push_back(attribute.toStdString());
        }

        if (filename.empty() || object.empty()) {
            usage_modify();
            exit(1);
        }

        break;
    }
    case ADDGRP:
    case REMGRP:
    {
        // -f file.fwb -p group -o object
        // Add/remove object to group
        // both group and object can be either path or ID

        parser.addOption(fileNameOption);
        parser.addOption(groupOption);
        parser.addOption(objectOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        object = parser.value(objectOption).toStdString();
        group = parser.value(groupOption).toStdString();

        if (filename.empty() || group.empty() || object.empty())
        {
            if (cmd == ADDGRP) usage_add();
            if (cmd == REMGRP) usage_remove();
            exit(1);
        }
        break;
    }
    case LIST:
    {
        // -f file.fwb -o object [-r] [-Fformat_string] [-d]
        // object can be either path or ID

        QCommandLineOption recursiveOption("r", "recursive");
        QCommandLineOption listChildrenOption("c", "list_children");
        QCommandLineOption fullDumpOption("d", "full_dump");

        parser.addOption(fileNameOption);
        parser.addOption(objectOption);
        parser.addOption(formatOption);
        parser.addOption(recursiveOption);
        parser.addOption(listChildrenOption);
        parser.addOption(fullDumpOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        object = parser.value(objectOption).toStdString();
        list_format = parser.value(formatOption).toStdString();
        recursive = parser.isSet(recursiveOption);
        list_children = parser.isSet(listChildrenOption);
        full_dump = parser.isSet(fullDumpOption);

        if (filename.empty() || object.empty())
        {
            usage_list();
            exit(1);
        }

        break;
    }
    case UPGRADE:
    {
        // -f file.fwb
        autoupgrade_flag = true;

        parser.addOption(fileNameOption);
        parser.parse(arguments);
        filename = parser.value(fileNameOption).toStdString();

        if (filename.empty())
        {
            usage_upgrade();
            exit(1);
        }

        break;
    }
    case STRUCT:
    {
        // -f file.fwb

        parser.addOption(fileNameOption);
        parser.parse(arguments);
        filename = parser.value(fileNameOption).toStdString();

        if (filename.empty())
        {
            usage_checktree();
            exit(1);
        }

        break;
    }
    case MERGE:
    {
        // -f file1.fwb -i file2.fwb

        QCommandLineOption mergeFileNameOption("f", "mergeFile", "mergeFile");
        QCommandLineOption conflictResolveOption("c", "conflictResolve", "conflictResolve");

        parser.addOption(fileNameOption);
        parser.addOption(mergeFileNameOption);
        parser.addOption(conflictResolveOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        filemerge = parser.value(mergeFileNameOption).toStdString();
        if (parser.isSet(conflictResolveOption)) {
            bool ok = false;
            conflict_res = parser.value(conflictResolveOption).toInt(&ok);
            if (!ok) {
                conflict_res = 1;
            }
        }

        if (filename.empty() || filemerge.empty())
        {
            usage_merge();
            exit(1);
        }

        break;
    }
    case IMPORT:
    {
        // -f file.fwb -i config.txt -o /User/Firewalls/new_firewall

        QCommandLineOption importConfigFileNameOption("i", "importConfigFilename", "importConfigFilename");
        QCommandLineOption deduplicateOption("d", "deduplicate");

        parser.addOption(fileNameOption);
        parser.addOption(importConfigFileNameOption);
        parser.addOption(objectOption);
        parser.addOption(deduplicateOption);
        parser.parse(arguments);

        filename = parser.value(fileNameOption).toStdString();
        import_config = parser.value(importConfigFileNameOption).toStdString();
        object = parser.value(objectOption).toStdString();
        deduplicate = parser.isSet(deduplicateOption);

        if (filename.empty() || import_config.empty() || object.empty())
        {
            usage_import();
            exit(1);
        }

        break;
    }
    case NONE:
        break;
    }

    if (cmd==NONE || filename=="")
    {
        usage();
        exit(1);
    }

    init(argv);

    try 
    {
        new Resources(Constants::getResourcesFilePath());

        /* create database */
        objdb = new FWObjectDatabase();

        /* load the data file */
        UpgradePredicate upgrade_predicate(autoupgrade_flag); 

        objdb->load(filename,  &upgrade_predicate, Constants::getDTDDirectory());
    
        if (cmd == MERGE)
        {
            if (filemerge.empty())
            {
                cerr << "The name of the file that should be merged is missing"
                     << endl;
                usage_merge();
                exit(1);
            }
            mergeTree(objdb, filemerge, conflict_res);
        }

        else if (cmd == IMPORT)
        {
            if (import_config.empty() || object.empty())
            {
                cerr << "Configuration file name and path to the new firewall "
                    "object are mandatory options for import" << endl;
                usage_import();
                exit(1);
            }

            QStringList components = QString::fromUtf8(object.c_str())
                .split("/", QString::SkipEmptyParts);
            string fw_name = components.last().toUtf8().constData();
            
            Library *library = nullptr;
            while (library == nullptr)
            {
                components.pop_back();
                string library_path = components.join("/").toUtf8().constData();

                list<FWObject*> objects;
                findObjects(library_path, objdb, objects);

                if (objects.size() == 0)
                {
                    cerr << "Library or folder '"
                         << library_path << "' not found" << endl;
                    usage_import();
                    exit(1);
                }

                library = Library::cast(objects.front());
            }

            cout << "Import firewall configuration from file "
                 << import_config
                 << endl;

            cout << "New firewall object '"
                 << fw_name
                 << "' will be created in library '"
                 << library->getName()
                 << "'"
                 << endl;

            importConfig(import_config, library, fw_name, deduplicate);
        }

        else if (cmd == STRUCT)
        {
            checkAndRepairTree(objdb);
        }

        else if (cmd == LIST)
        {
            listObject(objdb, object, list_children, recursive,
                       list_format, full_dump);
            return(0);
        }

        else if (cmd == UPGRADE)
        {
            cout << "File upgraded; current data format version: "
                 << libfwbuilder::Constants::getDataFormatVersion() << endl;
        }

        else  if (cmd == NEWOBJECT)
        {
            newObject(objdb, objtype, name, comment_txt, parent, ops);
        }

        else  if (cmd == DELOBJECT)
        {
            delObject(objdb, object);
        }

        else  if (cmd == MODOBJECT)
        {
            modObject(objdb, object, comment_txt, ops);
        }

        else
        {

            list<FWObject*> objects;
            findObjects(object, objdb, objects);
            if (objects.size()==0)
            {
                cout << "Object " << object << " not found" << endl;
                exit(-1);
            }

            for (list<FWObject*>::iterator it=objects.begin();
                 it!=objects.end(); ++it)
            {
                FWObject *obj = *it;

                if (cmd==ADDGRP)
                {
                    list<FWObject*> groups;
                    findObjects(group, objdb, groups);
                    if (groups.size()==0)
                    {
                        cout << "Group " << group << " not found" << endl;
                        exit(-1);
                    }
                    FWObject *grp = groups.front();
                    cout << "Adding object '" << obj->getName()
                         << "' to the group '" << grp->getName()
                         << "'" << endl;
                    grp->addRef(obj);
                }
                if (cmd==REMGRP)
                {
                    list<FWObject*> groups;
                    findObjects(group, objdb, groups);
                    if (groups.size()==0)
                    {
                        cout << "Group " << group << " not found" << endl;
                        exit(-1);
                    }
                    FWObject *grp = groups.front();
                    cout << "Removing object '" << obj->getName()
                         << "' from the group '" << grp->getName()
                         << "'" << endl;
                    grp->removeRef(obj);
                }
            }
        }

        QString filename_qstr = QString::fromUtf8(filename.c_str());
        QString bakfile = filename_qstr + ".bak";

        QFile bakf(bakfile);
        if (bakf.exists()) bakf.remove();

        QFile dataf(filename_qstr);
        if (dataf.rename(bakfile))
        {
            objdb->saveFile(filename);
        } else
        {
            cout << "Could not rename data file, abroting operation" << endl;
            cout << dataf.errorString().toStdString() << endl;
            exit(-1);
        }

    } catch(FWException &ex)  {
        cerr << ex.toString() << endl;
        exit(1);
    } catch (std::string s) {
        cerr << s;
        exit(1);
    } catch (std::exception &ex) {
        cerr << ex.what();
        exit(1);
    } catch (...) {
        cerr << "Unsupported exception";
        exit(1);
    }

    return(0);
}

