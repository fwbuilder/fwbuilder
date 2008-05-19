/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: fwblookup.cpp 1450 2007-12-05 16:53:10Z vk $

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
#include "fwbuilder/libfwbuilder-config.h"

#include <qsettings.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

// #include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif


#include "../common/init.cpp"

using namespace libfwbuilder;
using namespace std;

static char      *filename          = NULL;
static char      *object            = NULL;
static char      *attr              = NULL;
static char      *optn              = NULL;
static bool       dump              = false;  /* -D */
static bool       recursive         = false;  /* -r */
static bool       print_path        = false;  /* -P */
static bool       print_id          = false;  /* -I */
static bool       print_type        = false;  /* -T */
static bool       print_name        = false;  /* -N */
static bool       print_label       = false;  /* -L */
static bool       print_addr        = false;  /* -A */
static bool       print_mgmt_addr   = false;  /* -M */
static bool       list_children     = false;  /* -l */

FWObjectDatabase       *objdb = NULL;

int fwbdebug = 0;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
	cout << _("Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it.") << endl;
	return false;
    }
};
    
void usage(const char *name)
{
    cout << _("Firewall Builder:  general purpose object lookup tool") << endl;
    cout << _("Version ") << VERSION << "-" << RELEASE_NUM << endl;
    cout << _("Usage: ") << name
         << _(" [-V] -f filename.xml [-a attribute] [-o option_name] [-rADIlLMNPT] object_id | full_path_to_object") << endl;
}

FWObject *find_object(const string &obj_path,
                      FWObject *root=objdb)
{
    string path=obj_path;
    string::size_type n=path.find("/",0);
    string tree_node=path.substr(0,n);

    FWObject::iterator j=std::find_if(root->begin(), root->end(), 
                                      FWObjectNameEQPredicate(tree_node));

    if (j==root->end()) return NULL;
    if ((*j)->getName()==obj_path) return (*j);
    else    return find_object( path.substr(n+1) , (*j) );
}

void simplePrint(FWObject *o)
{
    if ( print_id )   cout << o->getId() << "\t";
    if ( print_name ) cout << o->getName() << "\t";
    if ( print_path )
    {
        FWObject *o1=o;
        string path=o1->getName();
        if (path.empty()) path=o1->getTypeName();
        while (o1->getParent()!=NULL)
        {
            path=o1->getParent()->getName()+"/"+path;
            o1=o1->getParent();
        }
        cout << path << "\t";
    }
    if ( print_type ) cout << o->getTypeName() << "\t";
    cout << endl;
}

void listObject(FWObject *obj,bool recursive)
{
    for (FWObject::iterator i=obj->begin(); i!=obj->end(); i++)
    {
        FWObject *o=*i;
        if (FWReference::cast(o)==NULL)
        {
            simplePrint(o);
            if (recursive) listObject(o,recursive);
        }
    }
}

int main(int argc, char * const *argv)
{   
    char errstr[1024];

#ifdef ENABLE_NLS
    setlocale (LC_ALL, "");

    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#else
#  ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "");
#  endif
#endif
    
    if (argc<=1)
    {
        usage(argv[0]);
        exit(1);
    }

    int   opt;

    while( (opt=getopt(argc,argv,"rADIlLMNPTVf:a:o:")) != EOF )
    {
        switch(opt)
        {
        case 'l':    list_children=true;            break;
        case 'D':    dump=true;                     break;
        case 'r':    recursive=true;                break;
        case 'A':    print_addr=true;               break;
        case 'I':    print_id=true;                 break;
        case 'L':    print_label=true;              break;
        case 'M':    print_mgmt_addr=true;          break;
        case 'N':    print_name=true;               break;
        case 'P':    print_path=true;               break;
        case 'T':    print_type=true;               break;
        case 'f':    filename = strdup(optarg);     break;
        case 'a':    attr = strdup(optarg);         break;
        case 'o':    optn = strdup(optarg);         break;
        case 'V':    usage(argv[0]);                exit(1);
        }
    }
    
    if((argc-1) != optind)
    {
        usage(argv[0]);
        exit(1);
    }

    object = strdup( argv[optind++] );

    init(argv);

    try   {

        new Resources(respath+FS_SEPARATOR+"resources.xml");

        /* create database */
        objdb = new FWObjectDatabase();

        /* load the data file */
        UpgradePredicate upgrade_predicate; 

        objdb->load(filename,  &upgrade_predicate, librespath);
        FWObject *obj;

        if (strchr(object,'/')!=NULL)
        {
/* got full path to the object */
            string path=object;
            string::size_type n=path.find("/",0);
            if (n==0 || 
                path.substr(0,n)=="FWObjectDatabase" || 
                path.substr(0,n)=="User")
                obj= find_object( path.substr(n+1) );
            else
                obj= find_object( path );
            if (obj==NULL)
            {
                SNPRINTF(errstr,sizeof(errstr),_("Object %s not found"),object );
                throw FWException(errstr);
            }
        } else {
/* got object ID */
            obj=objdb->getById(object,true);
            if (obj==NULL) 
            {
                SNPRINTF(errstr,sizeof(errstr),_("Object with ID='%s' not found"),object );
                throw FWException(errstr);
            }
        }

        if (attr!=NULL)
        {
            if (obj->exists(attr))
                cout << obj->getStr(attr) << endl;
            else 
            {
                SNPRINTF(errstr,sizeof(errstr),
                         _("Object %s (ID='%s') does not have attribute %s"),
                         obj->getName().c_str(),
                         obj->getId().c_str(),
                         attr );
                throw FWException(errstr);
            }                
            exit(0);
        } else
        {
            if (optn!=NULL)
            {
                if (Host::isA(obj) || Firewall::isA(obj))
                {
                    FWOptions *opt=Host::cast(obj)->getOptionsObject();
                    if (opt!=NULL)
                    {
                        cout << opt->getStr(optn);
                    } else 
                    {
                        SNPRINTF(errstr,sizeof(errstr),
                            _("Object %s (ID='%s') has no options"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                        throw FWException(errstr);
                    }
                } else 
                {
                    SNPRINTF(errstr,sizeof(errstr),
                             _("Can not print management address for %s (ID='%s'): only Host and Firewall objects have management interface"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                    throw FWException(errstr);
                }

            }
            if ( print_addr ) 
            {
                if (Address::cast(obj)!=NULL)
                    cout << Address::cast(obj)->getAddressPtr()->toString()
                         << endl;
                else {
                    SNPRINTF(errstr,sizeof(errstr),
                             _("Can not print address for %s (ID='%s'): objects of this type do not have address"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                    throw FWException(errstr);
                }
            }
            if ( print_label ) 
            {
                if (Interface::isA(obj))
                    cout << Interface::cast(obj)->getLabel() << endl;
                else {
                    SNPRINTF(errstr,sizeof(errstr),
                             _("Can not print label for %s (ID='%s'): only Interface object has label"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                    throw FWException(errstr);
                }
            }
            if ( print_mgmt_addr )
            {
                if (Host::isA(obj) || Firewall::isA(obj))
                {
                    InetAddr ma = Host::cast(obj)->getManagementAddress();
                    if (ma != InetAddr::getAny())
                    {
                        cout << ma.toString() << endl;
                    } else
                    {
                        SNPRINTF(errstr,sizeof(errstr),
                            _("Object %s (ID='%s') does not have management interface"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                        throw FWException(errstr);
                    }
                } else {
                    SNPRINTF(errstr,sizeof(errstr),
                             _("Can not print management address for %s (ID='%s'): only Host and Firewall objects have management interface"),
                             obj->getName().c_str(),
                             obj->getId().c_str() );
                    throw FWException(errstr);
                }
            }
            if ( list_children )
            {
                simplePrint(obj);
                listObject(obj,recursive);
                exit(0);
            }
            simplePrint(obj);
            if (dump ) obj->dump(cout,recursive,false);
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
	cerr << _("Unsupported exception");
        exit(1);
    }

    return(0);
}

