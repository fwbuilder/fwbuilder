/* 

                         Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "OSConfigurator_pix_os.h"

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
static int              only_print_inspection_code = 0;

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

    

class sort_by_net_zone {
    string any_address_id;
    public:
    explicit sort_by_net_zone()
    {
        any_address_id = FWObjectDatabase::getStringId(
            FWObjectDatabase::ANY_ADDRESS_ID);
    }
    bool operator()(const FWObject *a, const FWObject *b)
    {
	if (Interface::constcast(a) && Interface::constcast(b))
        {
	    string netzone_a=a->getStr("network_zone");
	    string netzone_b=b->getStr("network_zone");
	    if ( netzone_a==any_address_id) return false;
	    if ( netzone_b==any_address_id) return true;
	}
	return false;
    }
};


void usage(const char *name)
{
    cout << "Firewall Builder:  policy compiler for Cisco PIX firewall (with support for FWSM)" << endl;
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

    while( (opt=getopt(argc,argv,"x:vVf:d:r:tLo:i")) != EOF )
    {
        switch(opt)
        {
        case 'i':
            only_print_inspection_code++;
            break;
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

/* can use init and init2 only after command line option "-r" has been read */

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

#if 0
        objdb->setReadOnly( false );
        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(filename, &upgrade_predicate,  librespath);
        objdb->merge(ndb, NULL);
        delete ndb;
        objdb->setFileName(filename);
#endif


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

        bool pix_acl_basic=options->getBool("pix_acl_basic");
        bool pix_acl_no_clear=options->getBool("pix_acl_no_clear");
        bool pix_acl_substitution=options->getBool("pix_acl_substitution");
        bool pix_add_clear_statements=options->getBool("pix_add_clear_statements");
        
        if ( !pix_acl_basic &&
             !pix_acl_no_clear &&
             !pix_acl_substitution )
        {
            if ( pix_add_clear_statements ) options->setBool("pix_acl_basic",true);
            else options->setBool("pix_acl_no_clear",true);
        }

        if (only_print_inspection_code)
        {
            OSConfigurator_pix_os *oscnf=NULL;
            oscnf = new OSConfigurator_pix_os(objdb , fwobjectname, false);
            oscnf->prolog();

            cout << oscnf->getProtocolInspectionCommands();
            return 0;
        }

        Helper helper(NULL);

        multimap<string, FWObject*> netzone_objects;

        std::list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (std::list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);
/*
 * missing labels on interfaces
 */
            if (iface->getLabel()=="")
            {
                string lbl;
                if (iface->getSecurityLevel()==0)   lbl="outside";
                else
                {
                    if (iface->getSecurityLevel()==100) lbl="inside";
                    else
                    {
                        char s[64];
                        sprintf(s,"dmz%d",iface->getSecurityLevel());
                        lbl=s;
                    }
                }
                iface->setLabel(lbl);
            }

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
/*
 * there shouldn't be two interfaces with the same security level
 */
            for (std::list<FWObject*>::iterator j=l2.begin(); j!=l2.end(); ++j)
            {
                Interface *iface2=dynamic_cast<Interface*>(*j);
                assert(iface2);
                if (iface->getId()==iface2->getId()) continue;
                
                if (iface->getSecurityLevel()==iface2->getSecurityLevel())
                    throw FWException(
                        "Security level of each interface should be unique, however interfaces "+
                        iface->getName()+" ("+iface->getLabel()+")"+
                        " and "+
                        iface2->getName()+" ("+iface2->getLabel()+")"+
                        " have the same security level."
                    );

            }
/*
 * in PIX, we need network zones to be defined for all interfaces
 */
            string netzone_id=iface->getStr("network_zone");
            if (netzone_id=="") {
                throw FWException(
                    "Network zone definition is missing for interface "
                    +iface->getName()+" ("+iface->getLabel()+")");
            }
            FWObject *netzone=objdb->findInIndex(
                FWObjectDatabase::getIntId(netzone_id));
            if (netzone==NULL) 
            {
                throw FWException(
                    "Network zone points at nonexisting object for interface "
                    +iface->getName()+" ("+iface->getLabel()+")");
            }
/*
 * netzone may be a group, in which case we need to expand it
 * (recursively). 
 * 
 * 1. We create new temporary object (type Group).
 *
 * 2. put it in the database somewhere
 *
 * 3. add all objects that belong to the network zone to this
 * group. We add objects directly, not as a reference.
 *
 * 4. finally replace reference to the old network zone object in the
 * interface with reference to this new group.
 *
 * 5. we store ID of the original network zone object 
 *    using iface->setStr("orig_netzone_id")
 *
 * This ensures netzones do not contain other groups and do not
 * require any recursive expanding anymore. Since objects were added
 * to netzones directly, we do not need to bother with dereferencing,
 * too.
 */
            list<FWObject*> ol;
            helper.expand_group_recursive_no_cache(netzone,ol);

            FWObject *nz=objdb->create(ObjectGroup::TYPENAME);
            assert(nz!=NULL);
            nz->setName("netzone_"+iface->getLabel());
            objdb->add(nz);

            for (list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); ++j)
            {
                netzone_objects.insert( pair<string,FWObject*>(iface->getLabel(),*j));
                nz->add(*j);
            }
            iface->setStr("orig_netzone_id", netzone_id );
            iface->setStr("network_zone",
                          FWObjectDatabase::getStringId(nz->getId()) );
        }

/*
 * the same object (network or host) can not belong to network zones
 * of two different interfaces. Map netzone_objects holds pairs
 * interface_id/object. We just make sure the same object does not
 * appear in two pairs with different interfaces.
 */
        multimap<string,FWObject*>::iterator k;
        for (k=netzone_objects.begin(); k!=netzone_objects.end(); ++k)
        {
            multimap<string,FWObject*>::iterator l;
            l=k;
            ++l;
            for ( ; l!=netzone_objects.end(); ++l)
            {
                if ( l->second->getId() == k->second->getId() )
                {
                    if (k->first==l->first)
                        throw FWException(
                            "Object "+l->second->getName()
                            +" is used more than once in network zone of interface '"
                            +k->first+"'");
                    else
                        throw FWException(
                            "Object "+l->second->getName()
                            +" is used more than once in network zones of the following interfaces: '"
                            +k->first+"' and '"+l->first+"'");
                }
            }
            
        }

/*
 *  now sort interfaces by their network zone "width" (that is, more narrow 
 *  network zone should go first, interface with network zone "any" should be
 *  the last)
 *
 std::sort(fw->begin(), fw->end(), sort_by_net_zone() );
*/

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
        oscnf = new OSConfigurator_pix_os(objdb , fwobjectname, false);

        oscnf->prolog();
        oscnf->processFirewallOptions();


/* create compilers and run the whole thing */

        NATCompiler_pix *n = new NATCompiler_pix( objdb ,
                                                  fwobjectname, false, oscnf );

        if (test_mode) n->setTestMode();
        n->setDebugLevel( dl );
        n->setDebugRule(  drn );
        n->setVerbose( verbose );

        if ( n->prolog() > 0 ) {
            n->compile();
            n->epilog();
        } else
            cout << " Nothing to compile in NAT \n" << flush;


        PolicyCompiler_pix *c = new PolicyCompiler_pix( objdb ,
                                                        fwobjectname ,
                                                        false,
                                                        oscnf , n);

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
!  Firewall Builder  fwb_pix v" << VERSION << "-" << RELEASE_NUM << " \n\
!\n\
!  Generated " << timestr
              << " "
              << tzname[0]
              << " by " 
              << user_name;

        ofile << endl;

        string vers = fw->getStr("version");
        string platform = fw->getStr("platform");

        bool outbound_acl_supported = Resources::platform_res[platform]->getResourceBool(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+
            "/pix_outbound_acl_supported");
        bool afpa = options->getBool("pix_assume_fw_part_of_any");
        bool emulate_outb_acls = options->getBool("pix_emulate_out_acl");
        bool generate_outb_acls = options->getBool("pix_generate_out_acl");

        ofile << "!" << endl;
        ofile << "!" 
              << " Compiled for " << platform << " " << vers << endl;
        ofile << "!"
              << " Outbound ACLs " << string((outbound_acl_supported)?"supported":"not supported")
              << endl;
        if (!outbound_acl_supported)
        {
            ofile << "!"
                  << " Emulate outbound ACLs: " << string((emulate_outb_acls)?"yes":"no")
                  << endl;
        }
            ofile << "!"
                  << " Generating outbound ACLs: " << string((generate_outb_acls)?"yes":"no")
                  << endl;

        ofile << "!"
              << " Assume firewall is part of 'any': " << string((afpa)?"yes":"no")
              << endl;

        ofile << "!" << endl;
        ofile << "!" << MANIFEST_MARKER << "* " << ofname << endl;
        ofile << "!" << endl;

        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Prolog script:" << endl;
        ofile << "!" << endl;

        string pre_hook= fw->getOptionsObject()->getStr("pix_prolog_script");
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

        if (n->haveErrorsAndWarnings())
        {
            ofile << "! NAT compiler errors and warnings:"
                    << endl;
            ofile << n->getErrors("! ");
        }

        ofile << n->getCompiledScript();
        
        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Epilog script:" << endl;
        ofile << "!" << endl;

        string post_hook= fw->getOptionsObject()->getStr("pix_epilog_script");
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
//     } catch (std::exception ex) {
//         cerr << "exception: " << ex.what() << endl;
//         return 1;
//     } catch (...) {
//         cerr << "Unsupported exception";
//         return 1;
    }

    return 0;
}
