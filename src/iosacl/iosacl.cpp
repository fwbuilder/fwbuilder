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
#include <cstring>

#include "PolicyCompiler_iosacl.h"
#include "RoutingCompiler_iosacl.h"
#include "OSConfigurator_ios.h"

#include "fwcompiler/Preprocessor.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
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
static bool             ipv4_run       = true;
static bool             ipv6_run       = true;
static bool             fw_by_id       = false;

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

string safetyNetInstall(Firewall *fw)
{
    ostringstream output;
    if ( fw->getOptionsObject()->getBool("iosacl_acl_substitution") )
    {
        /* Generate short temporary ACL and assign it to all
         * interfaces. This ACL permits IPSEC (IP proto 50 and UDP port 500)
         as well as ssh from given subnet to any.
        */

        string platform = fw->getStr("platform");
        string version = fw->getStr("version");

        string temp_acl = "tmp_acl";
        string temp_acl_addr = fw->getOptionsObject()->getStr(
            "iosacl_acl_temp_addr");

        if (temp_acl_addr.empty())
        {
            cerr << "Missing address for management host or subnet for temporary ACL.\nPlease enter it in the tab 'Script options' in 'Firewall Settings' dialog"
                 << endl;
            exit(-1);
        }

        // if templ_acl_addr is ipv4 address, then we can not create this
        // temporary ACL while compiling ipv6 policy. And vice versa.

        bool create_temp_acl = false;
        bool tmp_acl_ipv6 = false;
        if (temp_acl_addr.find(":")!=string::npos)
        {
            //looks like ipv6
            create_temp_acl = true;
            tmp_acl_ipv6 = true;
        } else
        {
            // not ipv6, assume ipv4
            create_temp_acl = true;
            tmp_acl_ipv6 = false;
        }

        if (create_temp_acl)
        {
            string::size_type slash_idx = temp_acl_addr.find('/');
            string addr = temp_acl_addr;
            string netmask = "255.255.255.255";
            bool tmp_acl_v6 = false;

            // check if addr is v6

            try
            {
                InetAddr addrv6(AF_INET6, temp_acl_addr);
                tmp_acl_v6 = true;
            } catch(FWException &ex)
            {
                // Assume cnf->maddr is ipv4 
                if (slash_idx!=string::npos)
                {
                    addr = temp_acl_addr.substr(0,slash_idx);
                    netmask = temp_acl_addr.substr(slash_idx+1);
                    try
                    {
                        if (netmask.find(".")!=string::npos)
                        {
                            InetAddr nm(netmask);
                            nm.getLength(); // to avoid warning abt unused var
                        } else
                        {
                            int nm_length;
                            istringstream  str(netmask);
                            str >> nm_length;
                            InetAddr nm(nm_length);
                            netmask = nm.toString();
                        }
                    } catch(FWException &ex)
                    {
                        cerr << "Invalid netmask for management subnet: '"+netmask+"'"
                             << endl;
                        exit(-1);
                    }
                }

                try
                {
                    InetAddr a(addr);
                    a.isAny();
                } catch(FWException &ex)
                {
                    cerr << "Invalid address for management subnet: '"+addr+"'"
                         << endl;
                    exit(-1);
                }
            }

            string xml_element = "clear_ip_acl";
            if (tmp_acl_ipv6) xml_element = "clear_ipv6_acl";

            string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
                string("/FWBuilderResources/Target/options/")+
                "version_"+version+"/iosacl_commands/" + xml_element);

            output << endl;

            string addr_family_prefix = "ip";

            string access_group_cmd =
                PolicyCompiler_iosacl::getAccessGroupCommandForAddressFamily(tmp_acl_v6);

            output << "! temporary access list for \"safety net install\""
                   << endl;
            output << endl;

            if (tmp_acl_v6)
            {
                addr_family_prefix = "ipv6";
                output << clearACLcmd << " " << temp_acl << endl;
                output << "ipv6 access-list " << temp_acl << endl;
                if (slash_idx!=string::npos)
                    output << "  permit ipv6 " << addr << " any " << endl;
                else
                    output << "  permit ipv6 host " << addr << " any " << endl;
                output << "  deny ipv6 any any " << endl;
                output << "exit" << endl;
                output << endl;
            } else
            {
                // cisco uses "wildcards" instead of netmasks

                //long nm = InetAddr(netmask).to32BitInt();
                //struct in_addr na;
                //na.s_addr = ~nm;
                InetAddr nnm( ~(InetAddr(netmask)) );
                addr_family_prefix = "ip";
                output << clearACLcmd << " " << temp_acl << endl;
                output << "ip access-list extended " << temp_acl << endl;
                output << "  permit ip "
                       << addr << " " << nnm.toString() << " any " << endl;
                output << "  deny ip any any " << endl;
                output << "exit" << endl;
                output << endl;
            }

            // find management interface
            int nmi = 0;
            list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
            for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
            {
                Interface *intf = Interface::cast( *i );
                if (intf->isManagement())
                {
                    nmi++;
                    output << "interface " << intf->getName() << endl;
                    output << "  no " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " in" << endl;

                    output << "  no " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " out" << endl;

                    output << " " << addr_family_prefix << " ";
                    output << access_group_cmd;
                    output << " " << temp_acl << " in" << endl;
                    output << "exit" << endl;
                }
            }
            if (nmi==0)
            {
                cerr << "One of the interfaces of the firewall must be marked as management interface."
                     << endl;
                exit(-1);
            }

            output << endl;
        }
    }
    return output.str();
}

int main(int argc, char * const * argv)
{   
    if (argc<=1)
    {
        usage(argv0.c_str());
        exit(1);
    }

    int   opt;

    while( (opt=getopt(argc,argv,"x:ivVf:d:r:tLo:46")) != EOF )
    {
        switch(opt)
        {
        case 'i':
            fw_by_id = true;
            break;
        case '4':
            ipv4_run = true;
            ipv6_run = false;
            break;
        case '6':
            ipv4_run = false;
            ipv6_run = true;
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
            if (*optarg=='t') {
                test_mode = true;
            } else if (*optarg=='p') {
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

        /*
         *  some general sanity checks first
         */
	Firewall* fw;
        if (fw_by_id)
        {
            // fwobjectname is actually object id
            fw = Firewall::cast(
                objdb->findInIndex(objdb->getIntId(fwobjectname)));
            fwobjectname = fw->getName().c_str();
        }
        else
            fw = objdb->findFirewallByName(fwobjectname);

        if (ofname.empty())
            ofname = string(fwobjectname)+".fw";

        FWOptions* options=fw->getOptionsObject();

        string fwvers = fw->getStr("version");
        if (fwvers == "") fw->setStr("version", "12.x");

        string platform = fw->getStr("platform");
        string clearACLCmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") +
            "version_" + fwvers + "/iosacl_commands/clear_ip_acl");
        if (clearACLCmd.empty())
        {
            // incorrect version. This could have happened if user converted
            // firewall platform. See bug #2662290
            fw->setStr("version", "12.x");
        }

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




        OSConfigurator *oscnf=NULL;
        oscnf=new OSConfigurator_ios(objdb , fwobjectname, false);

        oscnf->prolog();
        oscnf->processFirewallOptions();





        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);

        int policy_rules_count  = 0;

        vector<int> ipv4_6_runs;
        string generated_script;

        generated_script = safetyNetInstall(fw);


        // command line options -4 and -6 control address family for which
        // script will be generated. If "-4" is used, only ipv4 part will 
        // be generated. If "-6" is used, only ipv6 part will be generated.
        // If neither is used, both parts will be done.

        if (options->getStr("ipv4_6_order").empty() ||
            options->getStr("ipv4_6_order") == "ipv4_first")
        {
            if (ipv4_run) ipv4_6_runs.push_back(AF_INET);
            if (ipv6_run) ipv4_6_runs.push_back(AF_INET6);
        }

        if (options->getStr("ipv4_6_order") == "ipv6_first")
        {
            if (ipv6_run) ipv4_6_runs.push_back(AF_INET6);
            if (ipv4_run) ipv4_6_runs.push_back(AF_INET);
        }

        for (vector<int>::iterator i=ipv4_6_runs.begin(); 
             i!=ipv4_6_runs.end(); ++i)
        {
            int policy_af = *i;
            bool ipv6_policy = (policy_af == AF_INET6);

            // Count rules for each address family
            int policy_count = 0;

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p)
            {
                Policy *policy = Policy::cast(*p);
                if (policy->matchingAddressFamily(policy_af)) policy_count++;
            }
            if (policy_count)
            {
                Preprocessor* prep = new Preprocessor(objdb,
                                                      fwobjectname, false);
                prep->compile();
            }

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p )
            {
                Policy *policy = Policy::cast(*p);

                if (!policy->matchingAddressFamily(policy_af)) continue;

                PolicyCompiler_iosacl c(objdb, fwobjectname,
                                        ipv6_policy, oscnf);

                c.setSourceRuleSet( policy );

                if (test_mode) c.setTestMode();
                c.setDebugLevel( dl );
                c.setDebugRule(  drp );
                c.setVerbose( verbose );

                if ( c.prolog() > 0 )
                {
                    c.compile();
                    c.epilog();

                    if (c.haveErrorsAndWarnings())
                    {
                        if (ipv6_policy)
                        {
                            generated_script += "\n\n";
                            generated_script += "! ================ IPv6\n";
                            generated_script += "\n\n";
                        } else
                        {
                            generated_script += "\n\n";
                            generated_script += "! ================ IPv4\n";
                            generated_script += "\n\n";
                        }

                        generated_script +=
                            "! Policy compiler errors and warnings:";
                        generated_script += "\n";
                        generated_script +=  c.getErrors("! ");
                    }
                    generated_script +=  c.getCompiledScript();

                } else
                    cout << " Nothing to compile in Policy \n" << flush;
            }

            if (!ipv6_policy)
            {
                // currently routing is supported only for ipv4
                RoutingCompiler_iosacl r( objdb, fwobjectname, false, oscnf);

                if (test_mode) r.setTestMode();
                r.setDebugLevel( dl );
                r.setDebugRule(  drp );
                r.setVerbose( verbose );
                
                if ( r.prolog() > 0 )
                {
                    r.compile();
                    r.epilog();

                    if (r.haveErrorsAndWarnings())
                    {
                        generated_script +=
                            "! Routing compiler errors and warnings:";
                        generated_script += "\n";
                        generated_script += r.getErrors("! ");
                    }

                    generated_script += r.getCompiledScript();
                } else
                    cout << " Nothing to compile in Routing \n" << flush;
            }
        }

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

        ofile << "!" << endl;
        ofile << "!" << " Compiled for " << platform << " " << fwvers << endl;

        ofile << "!" << endl;
        ofile << "!" << MANIFEST_MARKER << "* " << ofname << endl;
        ofile << "!" << endl;

        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Prolog script:" << endl;
        ofile << "!" << endl;

        string pre_hook= fw->getOptionsObject()->getStr("iosacl_prolog_script");
        ofile << pre_hook << endl;

        ofile << "!" << endl;
        ofile << "! End of prolog script:" << endl;
        ofile << "!" << endl;


        ofile << oscnf->getCompiledScript();
        ofile << endl;

        ofile << generated_script;

        ofile << endl;
        
        ofile << endl;
        ofile << "!" << endl;
        ofile << "! Epilog script:" << endl;
        ofile << "!" << endl;

        string post_hook= fw->getOptionsObject()->getStr("iosacl_epilog_script");
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
