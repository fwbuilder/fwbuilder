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

#include "fwbuilder/libfwbuilder-config.h"
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

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif

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

#include "../common/init.cpp"

#include "fwbedit.h"
#include "upgradePredicate.h"

using namespace libfwbuilder;
using namespace std;

string cmd_str = "";
command cmd = NONE;

bool autoupgrade_flag = false;
string filename = "";
string filemerge = "";
int conflict_res = 1;

vector<string> platforms;

FWObjectDatabase *objdb = NULL;

int fwbdebug = 0;

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
    cout << endl;

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

    cout <<
        "    delete -f file.fwb -o object\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -o object: object to be deleted, full path or ID\n";
    cout << endl;

    cout <<
        "    modify -f file.fwb -o object -c comment [-a attrs]\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -o object: object to be modified, full path or ID\n"
        "           -c txt:  specify comment for the new object\n"
        "           -a attribute1[,attribute2...]  :  specify attributes that\n"
        "              define parameters of the new object (see below)\n";
    cout << endl;

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

    cout <<
        "       add -f file.fwb -g group -o object\n"
        "\n"
        "           -f file.fwb: data file\n"
        "           -g group: group the object should be added to, \n"
        "              full path or ID\n"
        "           -o object: object to be added, full path or ID\n";
    cout << endl;

    cout <<
        "   remove -f file.fwb -g group -o object\n"
        "\n"
        "          -f file.fwb: data file\n"
        "          -g group: group the object should be removed from,\n"
        "             full path or ID\n"
        "          -o object: object to be removed, full path or ID\n";
    cout << endl;

    cout <<
        "   upgrade -f file.fwb\n"
        "\n"
        "          -f file.fwb: data file\n";
    cout << endl;

    cout <<
        "   checktree -f file.fwb\n"
        "\n"
        "          -f file.fwb: data file\n";
    cout << endl;

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

    if (argc<=2)
    {
        usage();
        exit(1);
    }

    /*
     *  Command line format:
     *  fwbedit command [options]
     *
     *  argv[1] is always command
     */
    cmd_str = string(argv[1]);

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

    char * const *args = argv;
    args++;
    argc--;

    int   opt;

    switch (cmd)
    {
    case NEWOBJECT:
    {
        // -f file.fwb -t objtype -n name -c comment -p parent [-a attrs]
        while( (opt=getopt(argc, args, "f:t:n:c:p:a:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 't': objtype = optarg; break;
            case 'n': name = optarg; break;
            case 'c': comment_txt = optarg; break;
            case 'p': parent = optarg; break;
            case 'a':
                int num=0;
                if (optarg!=NULL)
                {
                    string str = optarg;
                    num = splitStr(',', str, &ops);
                }
                break;
            }
        }
        break;
    }

    case DELOBJECT:
        // -f file.fwb -o object_def
        // object_def can be either full path or object ID
        while( (opt=getopt(argc, args, "f:o:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 'o': object = optarg; break;
            }
        }
        break;

    case MODOBJECT:
    {
        // -f file.fwb -o object -c comment [-a attrs]
        while( (opt=getopt(argc, args, "f:o:c:a:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 'o': object = optarg; break;
            case 'c': comment_txt = optarg; break;
            case 'a':
                int num=0;
                if (optarg!=NULL)
                {
                    string str = optarg;
                    num = splitStr(',', str, &ops);
                }
                break;
            }
        }
        break;
    }

    case ADDGRP:
    case REMGRP:
        // -f file.fwb -p group -o object
        // Add/remove object to group
        // both group and object can be either path or ID
        while( (opt=getopt(argc, args, "f:g:o:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 'g': group = optarg; break;
            case 'o': object = optarg; break;
            }
        }
        break;

    case LIST:
        // -f file.fwb -o object [-r] [-Fformat_string] [-d]
        // object can be either path or ID
        while( (opt=getopt(argc, args, "f:o:crdF:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 'o': object = optarg; break;
            case 'c': list_children = true; break;
            case 'r': recursive = true; break;
            case 'F': list_format = optarg; break;
            case 'd': full_dump = true; break;
            }
        }
        break;

    case UPGRADE:
        // -f file.fwb
        autoupgrade_flag = true;
        while( (opt=getopt(argc, args, "f:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            }
        }
        break;

    case STRUCT:
        // -f file.fwb
        while( (opt=getopt(argc, args, "f:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            }
        }
        break;
    
    case MERGE:
        // -f file1.fwb -i file2.fwb
        while( (opt=getopt(argc, args, "f:i:c:")) != EOF )
        {
            switch(opt)
            {
            case 'f': filename = optarg; break;
            case 'i': filemerge = optarg; break;
            case 'c': conflict_res = atoi(optarg); break;
            }
        }
        break;

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
        new Resources(respath+FS_SEPARATOR+"resources.xml");

        //new Resources(respath);   // creates singleton Resources::global_res

        /* create database */
        objdb = new FWObjectDatabase();

        /* load the data file */
        UpgradePredicate upgrade_predicate(autoupgrade_flag); 

        objdb->load(filename,  &upgrade_predicate, librespath);
    
        if (cmd == MERGE)
        {
            if (filemerge.empty())
            {
                usage();
                exit(1);
            }
            mergeTree(objdb, filemerge, conflict_res);
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

        string bakfile = filename+".bak";
        if (rename(filename.c_str(),bakfile.c_str()) == 0)
            objdb->saveFile(filename);
        else
        {
            cout << "Could not rename data file, abroting operation" << endl;
            cout << strerror(errno) << endl;
            exit(-1);
        }

    } catch(FWException &ex)  {
        cerr << ex.toString() << endl;
        exit(1);
    } catch (std::string s) {
        cerr << s;
        exit(1);
    } catch (std::exception ex) {
        cerr << ex.what();
        exit(1);
    } catch (...) {
        cerr << "Unsupported exception";
        exit(1);
    }

    return(0);
}

