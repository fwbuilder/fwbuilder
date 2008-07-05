/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#include "config.h"

#include <qsettings.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#  include <direct.h>
#else
#  include <unistd.h>
#endif

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>

#include "PolicyCompiler_iosacl.h"
#include "OSConfigurator_ios.h"

#include "fwcompiler/Preprocessor.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Tools.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#ifdef _WIN32
#include <getopt.h>
#else
#include <stdlib.h>
#endif
#endif

#include "../common/init.cpp"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


static const char      *filename       = NULL;
static const char      *wdir           = NULL;
static const char      *fwobjectname   = NULL;
static string           ofname         = "";
static int              dl             = 0;
static int              drp            = -1;
static int              drn            = -1;
static int              verbose        = 0;
static int              test_mode      = 0;

FWObjectDatabase       *objdb = NULL;


class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
        msg.size();  // to make compiler happy about unused parameter
	cout << "Data file has been created in the old version of Firewall Builder.\nLoad it in the GUI to convert it to the new version." << endl;
	return false;
    }
};

    


void usage(const char *name)
{
    cout << "Firewall Builder:  policy compiler for Cisco IOS ACL" << endl;
    cout << "Copyright 2002-2004 NetCitadel, LLC" << endl;
    cout << "Version " << VERSION << "-" << RELEASE_NUM << endl;
    cout << "Usage: " << name << " [-tvV] [-f filename.xml] [-d destdir] [-o output.fw] firewall_object_name" << endl;
}


int main(int argc, char * const * argv)
{   
    if (argc<=1)
    {
        usage(argv0.c_str());
        exit(1);
    }

    int   opt;

    while( (opt=getopt(argc,argv,"x:vVf:d:r:tLo:")) != EOF )
    {
        switch(opt)
        {
        case 'd':
            wdir = strdup(optarg);
            break;
        case 'r':
            respath = string(optarg);
            break;
        case 'f':
            filename = strdup(optarg);
            break;
        case 'o':
            ofname = string(optarg);
            break;
        case 'x':
            if (*optarg=='p') {
                ++optarg;
                drp  = atoi(optarg);
            } else {
                if (*optarg=='n') {
                    ++optarg;
                    drn  = atoi(optarg);
                } else {
                    if (isdigit(*optarg))  dl=atoi(optarg);  // increase debug level
                    else {
                        usage(argv[0]);
                        exit(1);
                    }
                }
            }
            break;
        case 't':
            test_mode++;
            break;
        case 'v':
            verbose++;
            break;
        case 'V':
            usage(argv[0]);
            exit(1);
	case 'h':
            usage(argv[0]);
            exit(1);
        }
    }

    init(argv);

    if((argc-1) != optind)
    {
        usage(argv[0]);
        exit(1);
    }

    fwobjectname = strdup( argv[optind++] );

    if (ofname.empty())
    {
        ofname=string(fwobjectname)+".fw";
    }

    if (filename==NULL || fwobjectname==NULL)
    {
        usage(argv[0]);
        exit(1);
    }

    if (wdir==0) 	wdir="./";

    if (
#ifdef _WIN32
        _chdir(wdir)
#else
        chdir(wdir)
#endif
    ) {
        cerr << "Can't change working directory to: " << wdir << endl;
        exit(1);
    }



    if (test_mode)
        cout << "*** Running in test mode, all errors are ignored" << endl << endl;

    try
    {
        new Resources(respath+FS_SEPARATOR+"resources.xml");

/* create database */
        objdb = new FWObjectDatabase();

/* load the data file */
        UpgradePredicate upgrade_predicate; 

        if (verbose) cout << " *** Loading data ...";

        objdb->setReadOnly( false );
        objdb->load( filename, &upgrade_predicate, librespath);
        objdb->setFileName(filename);
        objdb->reIndex();

        if (verbose) cout << " done\n";

/*  why do I do this ? 

        FWObject *slib = objdb->findInIndex("syslib000");
        if ( slib->isReadOnly()) slib->setReadOnly(false);
*/

/*
 *  some general sanity checks first
 */
        Firewall*  fw=objdb->findFirewallByName(fwobjectname);
        FWOptions* options=fw->getOptionsObject();

        string fwvers = fw->getStr("version");
        if (fwvers == "") fw->setStr("version", "12.x");

        bool ios_acl_basic=options->getBool("ios_acl_basic");
        bool ios_acl_no_clear=options->getBool("ios_acl_no_clear");
        bool ios_acl_substitution=options->getBool("ios_acl_substitution");
        bool ios_add_clear_statements=options->getBool("ios_add_clear_statements");
        
        if ( !ios_acl_basic &&
             !ios_acl_no_clear &&
             !ios_acl_substitution )
        {
            if ( ios_add_clear_statements ) options->setBool("ios_acl_basic",true);
            else options->setBool("ios_acl_no_clear",true);
        }

        Helper helper(NULL);

        std::list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (std::list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if ( iface->isDyn())  
            {
                list<FWObject*> l3=iface->getByType(IPv4::TYPENAME);
                if (l3.size()>0)
                {
                    char errstr[256];
                    for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                        if ( objdb->findAllReferences(*j).size()!=0 )
                        {
                            sprintf(errstr,
                                    _("Dynamic interface %s has an IP address that is used in the firewall policy rule.\n"),
                                    iface->getName().c_str() );
                            throw FWException(errstr);
                        }

                    sprintf(errstr,
                            _("Dynamic interface %s should not have an IP address object attached to it. This IP address object will be ignored.\n"),
                            iface->getName().c_str() );
                    cerr << errstr;
                    for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                        iface->remove(*j);
                }
            }

/*
 * no address
 */
            if (iface->isRegular())
            {
                FWObject *ipv4=iface->getFirstByType(IPv4::TYPENAME);
                if (ipv4==NULL)
                    throw FWException(
                        "Interface "+
                        iface->getName()+" ("+iface->getLabel()+") has no address." );
            }
        }

        char           timestr[256];
        time_t         tm;

        tm=time(NULL);
        strcpy(timestr,ctime(&tm));
        timestr[ strlen(timestr)-1 ]='\0';
    
#ifdef _WIN32
        char* user_name=getenv("USERNAME");
#else
        char* user_name=getenv("USER");
#endif
        if (user_name==NULL) 
            throw FWException("Can't figure out your user name, aborting");

        Preprocessor* prep=new Preprocessor(objdb , fwobjectname, false);
        prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
        OSConfigurator *oscnf=NULL;
        oscnf=new OSConfigurator_ios(objdb , fwobjectname, false);

        oscnf->prolog();
        oscnf->processFirewallOptions();

/* create compilers and run the whole thing */

        PolicyCompiler_iosacl *c = new PolicyCompiler_iosacl(objdb,
                                                             fwobjectname,
                                                             false,
                                                             oscnf);

        if (test_mode) c->setTestMode();
        c->setDebugLevel( dl );
        c->setDebugRule(  drp );
        c->setVerbose( verbose );

        if ( c->prolog() > 0 ) {
            c->compile();
            c->epilog();
        } else
            cout << " Nothing to compile in Policy \n" << flush;


#ifdef _WIN32
        ofstream ofile(ofname.c_str(), ios::out|ios::binary);
#else
        ofstream ofile(ofname.c_str());
#endif

        ofile << "!\n\
!  This is automatically generated file. DO NOT MODIFY !\n\
!\n\
!  Firewall Builder  fwb_iosacl v" << VERSION << "-" << RELEASE_NUM << " \n\
!\n\
!  Generated " << timestr
              << " "
              << tzname[0]
              << " by " 
              << user_name;

        ofile << endl;

        string vers = fw->getStr("version");
        string platform = fw->getStr("platform");

        ofile << "!" << endl;
        ofile << "!" << " Compiled for " << platform << " " << vers << endl;

        ofile << "!" << endl;
        ofile << "!" << MANIFEST_MARKER << "* " << ofname << endl;
        ofile << "!" << endl;

        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Prolog script:" << endl;
        ofile << "!" << endl;

        string pre_hook= fw->getOptionsObject()->getStr("ios_prolog_script");
        ofile << pre_hook << endl;

        ofile << "!" << endl;
        ofile << "! End of prolog script:" << endl;
        ofile << "!" << endl;


        ofile << oscnf->getCompiledScript();
        ofile << endl;

        if (c->haveErrorsAndWarnings())
        {
            ofile << "! Policy compiler errors and warnings:"
                    << endl;
            ofile << c->getErrors("! ");
        }

        ofile << c->getCompiledScript();
        ofile << endl;
        
        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Epilog script:" << endl;
        ofile << "!" << endl;

        string post_hook= fw->getOptionsObject()->getStr("ios_epilog_script");
        ofile << post_hook << endl;

        ofile << endl;
        ofile << "! End of epilog script:" << endl;
        ofile << "!" << endl;

        ofile.close();

        cout << _(" Compiled successfully") << endl << flush;
        
    } catch(libfwbuilder::FWException &ex)  {
        cerr << ex.toString() << endl;
        return 1;
    } catch (std::string s) {
        cerr << s << endl;
        return 1;
    } catch (std::exception ex) {
        cerr << "exception: " << ex.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unsupported exception";
        return 1;
    }

    return 0;
}
