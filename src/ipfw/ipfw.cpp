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
 
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>

#ifndef _WIN32
#  include <unistd.h>
#  include <pwd.h>
#else
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <cstring>
#include <assert.h>

#include "PolicyCompiler_ipfw.h"
#include "NATCompiler_ipfw.h"

#include "OSConfigurator_openbsd.h"
#include "OSConfigurator_macosx.h"
#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_solaris.h"

#include "fwcompiler/Preprocessor.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"

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

int fwbdebug = 0;

static const char      *filename       = NULL;
static const char      *wdir           = NULL;
static const char      *fwobjectname   = NULL;
static string           fw_file_name   = "";
static int              dl             = 0;
static int              drp            = -1;
static int              drn            = -1;
static int              verbose        = 0;
static bool             test_mode      = false;
static bool             ipv4_run       = true;
static bool             ipv6_run       = true;
static bool             fw_by_id       = false;

FWObjectDatabase       *objdb = NULL;

#ifdef _WIN32
string fs_separator = "\\";
#else
string fs_separator = "/";
#endif

string getFileName(const string &file_path)
{
    string::size_type n = file_path.rfind(fs_separator);
    string res = file_path;
    res.erase(0, n+1);
    return res;
}

string getDir(const string &file_path)
{
    string::size_type n = file_path.rfind(fs_separator);
    string res = file_path;
    if (n==string::npos) return "";
    else res.erase(n);
    return res;
}

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    { 
	cout << _("Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it.") << endl;
	return false;
    }
};

/* Find rulesets that belong to other firewall objects but are
 * referenced by rules of this firewall using action Branch
 */
void findImportedRuleSets(Firewall *fw, list<FWObject*> &all_policies)
{
    list<FWObject*> imported_policies;
    for (list<FWObject*>::iterator i=all_policies.begin();
         i!=all_policies.end(); ++i)
    {
        for (list<FWObject*>::iterator r=(*i)->begin();
             r!=(*i)->end(); ++r)
        {
            PolicyRule *rule = PolicyRule::cast(*r);
            RuleSet *ruleset = NULL;
            if (rule->getAction() == PolicyRule::Branch &&
                (ruleset = rule->getBranch())!=NULL &&
                !ruleset->isChildOf(fw))
            {
                imported_policies.push_back(ruleset);
            }
        }
    }
    if (imported_policies.size() > 0)
        all_policies.insert(all_policies.end(),
                            imported_policies.begin(), imported_policies.end());
}
    
void usage(const char *name)
{
    cout << _("Firewall Builder:  policy compiler for ipfw") << endl;
    cout << _("Version ") << VERSION << RELEASE_NUM << endl;
    cout << _("Usage: ") << name << " [-x] [-v] [-V] [-f filename.xml] [-o output.fw] [-d destdir] [-m] firewall_object_name" << endl;
}

int main(int argc, char * const *argv)
{   

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

    while( (opt=getopt(argc,argv,"x:ivVf:d:r:o:46")) != EOF )
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
            fw_file_name = string(optarg);
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
        }
    }
    
    if((argc-1) != optind)
    {
        usage(argv[0]);
        exit(1);
    }

    fwobjectname = strdup( argv[optind++] );

    if (wdir==0) 	wdir="./";

    if (
#ifdef _WIN32
    _chdir(wdir)
#else
    chdir(wdir)
#endif
    ) {
	cerr << _("Can't change to: ") << wdir << endl;
	exit(1);
    }

    init(argv);

    try   {
        new Resources(respath+FS_SEPARATOR+"resources.xml");

	/* create database */
	objdb = new FWObjectDatabase();

	/* load the data file */
	UpgradePredicate upgrade_predicate; 

	if (verbose) cout << _(" *** Loading data ...");

        objdb->setReadOnly( false );
        objdb->load( sysfname, &upgrade_predicate, librespath);
        objdb->setFileName("");
        FWObjectDatabase *ndb = new FWObjectDatabase();
        ndb->load(filename, &upgrade_predicate,  librespath);
        objdb->merge(ndb, NULL);
        delete ndb;
        objdb->setFileName(filename);
        objdb->reIndex();

//	objdb->load(filename,  &upgrade_predicate, librespath);
	if (verbose) cout << _(" done\n");

        FWObject *slib = objdb->getById(FWObjectDatabase::STANDARD_LIB_ID);
        if (slib && slib->isReadOnly()) slib->setReadOnly(false);

	/* Review firewall and OS options and generate commands */
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

        if (fw_file_name.empty())
        {
            fw_file_name=string(fwobjectname)+".fw";
        }

        /* some initial sanity checks */
        list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
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
            } else
            {

                list<FWObject*> la=iface->getByType(IPv4::TYPENAME);
                if ( iface->isRegular() && la.empty() )
                {
                    char errstr[256];
                    sprintf(errstr,_("Missing IP address for interface %s\n"),
                            iface->getName().c_str() );
                    throw FWException(errstr);
                }

                for (list<FWObject*>::iterator j=la.begin(); j!=la.end(); ++j) 
                {
                    IPv4 *ipv4 = IPv4::cast(*j);
                    if ( ipv4->getAddressPtr()->isAny())
                    {
                        char errstr[256];
                        sprintf(errstr,
                      _("Interface %s has IP address \"0.0.0.0\".\n"),
                                iface->getName().c_str() );
                        throw FWException(errstr);
                    }
                }
            }

        }


	FWOptions* options=fw->getOptionsObject();
	string s;

	string firewall_dir=options->getStr("firewall_dir");
	if (firewall_dir=="") firewall_dir="/etc/fw";

	bool debug=options->getBool("debug");
	string shell_dbg=(debug)?"-x":"" ;

/*
 * Process firewall options, build OS network configuration script
 */
	OSConfigurator *oscnf=NULL;
        string family=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceStr("/FWBuilderResources/Target/family");
	if ( family=="macosx")
            oscnf=new OSConfigurator_macosx(objdb , fwobjectname, false);

	if ( family=="freebsd")
            oscnf=new OSConfigurator_freebsd(objdb , fwobjectname, false);

	if (oscnf==NULL)
	    throw FWException(_("Unrecognized host OS ")+fw->getStr("host_OS")+"  (family "+family+")");

	oscnf->prolog();


        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        vector<bool> ipv4_6_runs;
        string generated_script;
        int policy_rules_count  = 0;
        int ipfw_rule_number = 0;

        findImportedRuleSets(fw, all_policies);

        // command line options -4 and -6 control address family for which
        // script will be generated. If "-4" is used, only ipv4 part will 
        // be generated. If "-6" is used, only ipv6 part will be generated.
        // If neither is used, both parts will be done.

        if (options->getStr("ipv4_6_order").empty() ||
            options->getStr("ipv4_6_order") == "ipv4_first")
        {
            if (ipv4_run) ipv4_6_runs.push_back(false);
            if (ipv6_run) ipv4_6_runs.push_back(true);
        }

        if (options->getStr("ipv4_6_order") == "ipv6_first")
        {
            if (ipv6_run) ipv4_6_runs.push_back(true);
            if (ipv4_run) ipv4_6_runs.push_back(false);
        }

        for (vector<bool>::iterator i=ipv4_6_runs.begin(); 
             i!=ipv4_6_runs.end(); ++i)
        {
            bool ipv6_policy = *i;

            /*
              We need to create and run preprocessor for this address
              family before nat and policy compilers, but if there are
              no nat / policy rules for this address family, we do not
              need preprocessor either.
            */

            // Count rules for each address family
            int policy_count = 0;

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p)
            {
                Policy *policy = Policy::cast(*p);
                if (policy->isV6()==ipv6_policy) policy_count++;
            }

            if (policy_count)
            {
                Preprocessor* prep = new Preprocessor(
                    objdb , fwobjectname, ipv6_policy);
                if (test_mode) prep->setTestMode();
                prep->compile();
            }

            ostringstream c_str;
            bool empty_output = true;
 
            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p )
            {
                Policy *policy = Policy::cast(*p);
                string branch_name = policy->getName();

                if (policy->isV6()!=ipv6_policy) continue;

                PolicyCompiler_ipfw c(objdb, fwobjectname, ipv6_policy, oscnf);
                c.setIPFWNumber(ipfw_rule_number);
                c.setSourceRuleSet( policy );
                c.setRuleSetName(branch_name);
                c.setDebugLevel( dl );
                c.setDebugRule(  drp );
                c.setVerbose( (bool)(verbose) );
                if (test_mode) c.setTestMode();

                if ( (policy_rules_count=c.prolog()) > 0 )
                {
                    c.compile();
                    c.epilog();

                    ipfw_rule_number = c.getIPFWNumber();

                    if (c.getCompiledScriptLength() > 0)
                    {
                        c_str << "# ================ Rule set "
                              << branch_name << endl;
                        if (c.haveErrorsAndWarnings())
                        {
                            c_str << "# Policy compiler errors and warnings:"
                                  << endl;
                            c_str << c.getErrors("# ");
                        }
                        c_str << c.getCompiledScript();
                        c_str << endl;
                        empty_output = false;
                    }
                }
            }

            if (!empty_output)
            {
                if (ipv6_policy)
                {
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv6\n";
                    generated_script += "\n\n";
                } else
                {
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv4\n";
                    generated_script += "\n\n";
                }
            }

            generated_script += c_str.str();
        }


#if NO_IPV6
/*
 * create compilers and run the whole thing 
 */
	PolicyCompiler_ipfw c( objdb , fwobjectname, false , oscnf );

	c.setDebugLevel( dl );
	c.setDebugRule(  drp );
	c.setVerbose( verbose );
        if (test_mode) c.setTestMode();

	bool     have_ipfw=false;
	if ( c.prolog() > 0 ) 
        {
	    have_ipfw=true;

	    c.compile();
	    c.epilog();
	}
#endif

/*
 * now write generated scripts to files
 */


        char          *timestr;
        time_t         tm;
        struct tm     *stm;

        tm=time(NULL);
        stm=localtime(&tm);
        timestr=strdup(ctime(&tm));
        timestr[ strlen(timestr)-1 ]='\0';
    
#ifdef _WIN32
        char* user_name=getenv("USERNAME");
#else
        struct passwd *pwd=getpwuid(getuid());
        assert(pwd);
        char *user_name=pwd->pw_name;
#endif
        if (user_name==NULL)
        {
            user_name=getenv("LOGNAME");
            if (user_name==NULL)
            {
                cerr << _("Can't figure out your user name, aborting") << endl;
                exit(1);
            }
        }

	ofstream fw_file;
        fw_file.exceptions(ofstream::eofbit|ofstream::failbit|ofstream::badbit);

#ifdef _WIN32
        fw_file.open(fw_file_name.c_str(), ios::out|ios::binary);
#else
        fw_file.open(fw_file_name.c_str());
#endif
	fw_file << "#!/bin/sh " << shell_dbg << endl << endl;

        fw_file << _("#\n\
#  This is automatically generated file. DO NOT MODIFY !\n\
#\n\
#  Firewall Builder  fwb_ipfw v") << VERSION << "-" << RELEASE_NUM << _(" \n\
#\n\
#  Generated ") << timestr << " " << tzname[stm->tm_isdst] << _(" by ") 
               << user_name << "\n#\n";

        fw_file << MANIFEST_MARKER << "* " << getFileName(fw_file_name) << endl;
        fw_file << "#" << endl;
        fw_file << "#" << endl;

        string fwcomment=fw->getComment();
        string::size_type n1,n2;
        n1=n2=0;
        while ( (n2=fwcomment.find("\n",n1))!=string::npos )
        {
            fw_file << "#  " << fwcomment.substr(n1,n2-n1) << endl;
            n1=n2+1;
        }
        fw_file << "#  " << fwcomment.substr(n1) << endl;
        fw_file << "#\n#\n#\n";

        fw_file << "cd " << firewall_dir << " || exit 1" << endl << endl;

        fw_file << endl;
        fw_file << "#" << endl;
        fw_file << "# Prolog script" << endl;
        fw_file << "#" << endl;

        string pre_hook= fw->getOptionsObject()->getStr("prolog_script");
        fw_file << pre_hook << endl;

        fw_file << "#" << endl;
        fw_file << "# End of prolog script" << endl;
        fw_file << "#" << endl;

	fw_file << oscnf->getCompiledScript();

        fw_file << endl;

        fw_file << "log '";
        fw_file << _("Activating firewall script generated ")
               << timestr << " " << _(" by ")
               << user_name;
        fw_file << "'" << endl;

	fw_file << endl;



/* commented out since we now use sets
        fw_file << "\"$IPFW\" -f -q flush" << endl;
*/
        fw_file << endl;

#if NO_IPV6
	if (have_ipfw)
        {
            if (c.haveErrorsAndWarnings())
            {
                fw_file << "# Policy compiler errors and warnings:"
                         << endl;
                fw_file << c.getErrors("# ");
            }

	    fw_file << c.getCompiledScript();
        }
#else
        PolicyCompiler_ipfw c(objdb, fwobjectname, false, oscnf);
        fw_file << c.defaultRules();
        fw_file << generated_script;
#endif

        fw_file << endl;
        fw_file << "#" << endl;
        fw_file << "# Epilog script" << endl;
        fw_file << "#" << endl;

        string post_hook= fw->getOptionsObject()->getStr("epilog_script");
        fw_file << post_hook << endl;

        fw_file << endl;
        fw_file << "# End of epilog script" << endl;
        fw_file << "#" << endl;

        fw_file << "\"$IPFW\" set swap 0 1 || exit 1" << endl;
        fw_file << "\"$IPFW\" delete set 1" << endl;

	fw_file << endl;
	fw_file.close();

#ifdef _WIN32
        _chmod(fw_file_name.c_str(),_S_IREAD|_S_IWRITE);
#else
        chmod(fw_file_name.c_str(),S_IXUSR|S_IRUSR|S_IWUSR|S_IRGRP);
#endif
        
        cout << _(" Compiled successfully") << endl << flush;
        
        return 0;

    } catch(const FWException &ex)  {
	cerr << ex.toString() << endl;
        return 1;
#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        cerr << "Error while opening or writing to the output file" << endl;
        return 1;
#endif
    } catch (const std::string &s) {
	cerr << s;
        return 1;
    } catch (const std::exception &ex) {
	cerr << ex.what();
        return 1;
    } catch (...) {
	cerr << _("Unsupported exception");
        return 1;
    }

}









