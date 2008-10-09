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
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>

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
#include <assert.h>
#include <cstring>

#include "PolicyCompiler_pf.h"
#include "NATCompiler_pf.h"
#include "TableFactory.h"
#include "Preprocessor_pf.h"

#include "OSConfigurator_openbsd.h"
#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_solaris.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"

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
static string           output_dir     = "";
static int              dl             = 0;
static int              drp            = -1;
static int              drn            = -1;
static int              verbose        = 0;
static bool             test_mode      = false;
static bool             ipv4_run       = true;
static bool             ipv6_run       = true;
static bool             fw_by_id       = false;

#ifdef _WIN32
string fs_separator = "\\";
#else
string fs_separator = "/";
#endif

// Note that in the following maps ruleset name will be 
// "__main__" for both main Policy and NAT rulesets.

// map ruleset_name -> conf file name
static map<string, string>   conf_files;

// map ruleset_name -> generated script
static map<string, ostringstream*>   generated_scripts;

// map ruleset_name -> TableFactory*
static map<string, TableFactory*>   table_factories;


FWObjectDatabase       *objdb = NULL;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    { 
	cout << _("Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it.") << endl;
	return false;
    }
};

// a functor to join list<string> into a string with separator sep
class join : public std::unary_function<std::string, void>
{
    std::string *result;
    std::string  separator;
public:
    join(std::string *res, const std::string &s)
    { result = res; separator = s; }
    void operator()(std::string &s);
};
    
void join::operator()(std::string &s)
{
    if (!result->empty()) *result += separator;
    *result += s;
}

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

string getConfFileName(const string &ruleset_name,
                       const string &fwobjectname,
                       const string &fw_file_name)
{
    string conf_file_name;
    string suffix = "-" + ruleset_name;
    if (ruleset_name == "__main__") suffix = "";

    if (fw_file_name.empty())
    {
        conf_file_name = string(fwobjectname) + suffix + ".conf";
    } else
    {
        string just_file = getFileName(fw_file_name);
        string::size_type n = just_file.rfind(".");
        conf_file_name = just_file;
        conf_file_name.erase(n);
        conf_file_name.append(suffix + ".conf");
    }

    return conf_file_name;
}

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
    cout << _("Firewall Builder:  policy compiler for OpenBSD PF") << endl;
    cout << _("Version ") << VERSION << RELEASE_NUM << endl;
    cout << _("Usage: ") << name << " [-x] [-v] [-V] [-f filename.xml] [-o output.fw] [-d destdir] [-m] [-4|-6] firewall_object_name" << endl;
}


string printTimeout(FWOptions* options,
                    const string &OnOffOption,
                    const string &ValOption,
                    const string &pfCode)
{
    std::ostringstream res;
    if (options->getBool(OnOffOption) && options->getInt(ValOption)>0)
    {
        res << "set timeout "
            << pfCode << " " << options->getInt(ValOption) << endl;
    }
    return res.str();
}

void printProlog(ofstream &file, const string &prolog_code)
{
    file << endl;
    file << "#" << endl;
    file << "# Prolog script" << endl;
    file << "#" << endl;
    file << prolog_code << endl;
    file << "#" << endl;
    file << "# End of prolog script" << endl;
    file << "#" << endl;
}

void printStaticOptions(ofstream &file, Firewall* fw)
{
    FWOptions* options = fw->getOptionsObject();
    list<FWObject*> all_interfaces=fw->getByType(Interface::TYPENAME);
    string prolog_place = options->getStr("prolog_place");
    if (prolog_place.empty()) prolog_place = "fw_file";  // old default
    string pre_hook = options->getStr("prolog_script");

    if (prolog_place == "pf_file_top")
        printProlog(file, pre_hook);

    file << endl;

    list<string> limits;
    if (options->getBool("pf_do_limit_frags") &&
        options->getInt("pf_limit_frags")>0 )
        limits.push_back(string("frags ") +
                         options->getStr("pf_limit_frags"));

    if (options->getBool("pf_do_limit_states") &&
        options->getInt("pf_limit_states")>0 )
        limits.push_back(string("states ") +
                         options->getStr("pf_limit_states"));

    if (options->getBool("pf_do_limit_src_nodes") &&
        options->getInt("pf_limit_src_nodes")>0 )
        limits.push_back(string("src-nodes ") +
                         options->getStr("pf_limit_src_nodes"));

    if (options->getBool("pf_do_limit_tables") &&
        options->getInt("pf_limit_tables")>0 )
        limits.push_back(string("tables ") +
                         options->getStr("pf_limit_tables"));

    if (options->getBool("pf_do_limit_table_entries") &&
        options->getInt("pf_limit_table_entries")>0 )
        limits.push_back(string("table-entries ") +
                         options->getStr("pf_limit_table_entries"));

    if (limits.size() > 0)
    {
        file << "set limit ";
        if (limits.size() > 1 ) file << "{ ";
        string all_limits;
        for_each(limits.begin(), limits.end(), join( &all_limits, ", "));
        file << all_limits;
        if (limits.size() > 1 ) file << " }";
        file << endl;
    }

    if ( ! options->getStr("pf_optimization").empty() )
        file << "set optimization "
             << options->getStr("pf_optimization") << endl;

    file << printTimeout(options,
                         "pf_do_timeout_interval","pf_timeout_interval",
                         "interval");
    file << printTimeout(options,
                         "pf_do_timeout_frag","pf_timeout_frag",
                         "frag");

    file << printTimeout(options,
                         "pf_set_tcp_first","pf_tcp_first",
                         "tcp.first" );
    file << printTimeout(options,
                         "pf_set_tcp_opening","pf_tcp_opening",
                         "tcp.opening" );
    file << printTimeout(options,
                         "pf_set_tcp_established","pf_tcp_established",
                         "tcp.established" );
    file << printTimeout(options,
                         "pf_set_tcp_closing","pf_tcp_closing",
                         "tcp.closing" );
    file << printTimeout(options,
                         "pf_set_tcp_finwait","pf_tcp_finwait",
                         "tcp.finwait" );
    file << printTimeout(options,
                         "pf_set_tcp_closed","pf_tcp_closed",
                         "tcp.closed" );
    file << printTimeout(options,
                         "pf_set_udp_first","pf_udp_first",
                         "udp.first" );
    file << printTimeout(options,
                         "pf_set_udp_single","pf_udp_single",
                         "udp.single" );
    file << printTimeout(options,
                         "pf_set_udp_multiple","pf_udp_multiple",
                         "udp.multiple" );
    file << printTimeout(options,
                         "pf_set_icmp_first","pf_icmp_first",
                         "icmp.first" );
    file << printTimeout(options,
                         "pf_set_icmp_error","pf_icmp_error",
                         "icmp.error" );
    file << printTimeout(options,
                         "pf_set_other_first","pf_other_first",
                         "other.first" );
    file << printTimeout(options,
                         "pf_set_other_single","pf_other_single",
                         "other.single" );
    file << printTimeout(options,
                         "pf_set_other_multiple","pf_other_multiple",
                         "other.multiple" );

    file << printTimeout(options,
                         "pf_set_adaptive","pf_adaptive_start",
                         "adaptive.start" );
    file << printTimeout(options,
                         "pf_set_adaptive","pf_adaptive_end",
                         "adaptive.end");

    // check if any interface is marked as 'unprotected'
    // and generate 'set skip on <ifspec>' commands

    if (fw->getStr("version")=="ge_3.7" ||
        fw->getStr("version")=="4.x") 
    {
        for (list<FWObject*>::iterator i=all_interfaces.begin();
             i!=all_interfaces.end(); ++i) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if ( iface->isUnprotected())  
                file << "set skip on " << iface->getName() << endl;
        }
    }

    file << endl;

    if (prolog_place == "pf_file_after_set")
        printProlog(file, pre_hook);

    string   scrub_options;

    if (options->getBool("pf_do_scrub"))
    {
        if (options->getBool("pf_scrub_reassemble"))     
            scrub_options+="fragment reassemble ";
        else
        {
            if (options->getBool("pf_scrub_fragm_crop"))
                scrub_options+="fragment crop ";
            else
            {
                if (options->getBool("pf_scrub_fragm_drop_ovl"))
                    scrub_options+="fragment drop-ovl ";
            }
        }
    }

    if (options->getBool("pf_scrub_no_df"))  scrub_options+="no-df ";

    if (!scrub_options.empty())
    {
        file << "#" << endl;
        file << "# Scrub rules" << endl;
        file << "#" << endl;
        file << "scrub in all " << scrub_options << endl;
    }

    scrub_options="";
    if (options->getBool("pf_scrub_random_id"))
        scrub_options+="random-id ";
    if (options->getBool("pf_scrub_use_minttl"))
        scrub_options+="min-ttl " + options->getStr("pf_scrub_minttl") + " ";
    if (options->getBool("pf_scrub_use_maxmss"))
        scrub_options+="max-mss " + options->getStr("pf_scrub_maxmss") + " ";
    if (!scrub_options.empty())
    {
        file << "scrub out all " << scrub_options << endl; 
    }
    file << endl;

    if (prolog_place == "pf_file_after_scrub")
        printProlog(file, pre_hook);

    //file << table_factory->PrintTables();
    //file << endl;

    if (prolog_place == "pf_file_after_tables")
        printProlog(file, pre_hook);

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

    try
    {
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
            fw_file_name = string(fwobjectname)+".fw";
            output_dir = "";
        } else
        {
            output_dir = getDir(fw_file_name);
            if (!output_dir.empty()) output_dir += "/";
        }

        /* some initial sanity checks */
        list<FWObject*> all_interfaces=fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=all_interfaces.begin();
             i!=all_interfaces.end(); ++i) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if ( iface->isDyn())  
            {
                list<FWObject*> l3=iface->getByType(IPv4::TYPENAME);
                if (l3.size()>0)
                {
                    char errstr[256];
                    for (list<FWObject*>::iterator j=l3.begin();
                         j!=l3.end(); ++j)
                    { 
                        if ( objdb->findAllReferences(*j).size()!=0 )
                        {
                            sprintf(errstr,
                                    "Dynamic interface %s has an IP address that"
                                    "is used in the firewall policy rule.\n",
                                    iface->getName().c_str() );
                            throw FWException(errstr);
                        }
                    }
                    sprintf(errstr,
                            "Dynamic interface %s should not have an IP address"
                            " object attached to it. This IP address object "
                            "will be ignored.\n",
                            iface->getName().c_str() );
                    cerr << errstr;
                    for (list<FWObject*>::iterator j=l3.begin();
                         j!=l3.end(); ++j) 
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
                    if (ipv4->getAddressPtr()->isAny())
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

	FWOptions* options = fw->getOptionsObject();
	string s;

	string firewall_dir = options->getStr("firewall_dir");
	if (firewall_dir=="") firewall_dir="/etc/fw";

	string prolog_place = options->getStr("prolog_place");
        if (prolog_place.empty()) prolog_place = "fw_file";  // old default
        string pre_hook = fw->getOptionsObject()->getStr("prolog_script");

	bool debug = options->getBool("debug");
	string shell_dbg = (debug)?"-x":"" ;
	string pfctl_dbg = (debug)?"-v ":"";

        string pfctl_f_option = "-f ";
//        if (fw->getStr("version")=="obsd_3.2")    pfctl_f_option="-f ";
        if (fw->getStr("version")=="obsd_lt_3.2") pfctl_f_option="-R ";
        
/*
 * Process firewall options, build OS network configuration script
 */
	OSConfigurator *oscnf=NULL;
        string family = Resources::os_res[fw->getStr("host_OS")
          ]->Resources::getResourceStr("/FWBuilderResources/Target/family");

	if (family=="solaris")
            oscnf=new OSConfigurator_solaris(objdb , fwobjectname, false);

	if (family=="openbsd")
        {
            cerr << "Calling OSConfigurator_openbsd" << endl;
            oscnf=new OSConfigurator_openbsd(objdb , fwobjectname, false);
        }

	if (family=="freebsd")
            oscnf=new OSConfigurator_freebsd(objdb , fwobjectname, false);

	if (oscnf==NULL)
	    throw FWException(_("Unrecognized host OS ") + 
                              fw->getStr("host_OS")+"  (family "+family+")");

	oscnf->prolog();

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

        findImportedRuleSets(fw, all_policies);

        vector<bool> ipv4_6_runs;
        bool have_nat = false;
        bool have_pf = false;

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

        ostringstream *main_str = new ostringstream();

        for (vector<bool>::iterator i=ipv4_6_runs.begin(); 
             i!=ipv4_6_runs.end(); ++i)
        {
            bool ipv6_policy = *i;

            // Count rules for each address family
            int nat_count = 0;
            int policy_count = 0;

            for (list<FWObject*>::iterator p=all_nat.begin();
                 p!=all_nat.end(); ++p)
            {
                NAT *nat = NAT::cast(*p);
                if (nat->isV6()==ipv6_policy) nat_count++;
            }

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p)
            {
                Policy *policy = Policy::cast(*p);
                if (policy->isV6()==ipv6_policy) policy_count++;
            }

            if (nat_count || policy_count)
            {
                Preprocessor_pf* prep = new Preprocessor_pf(
                    objdb , fwobjectname, ipv6_policy);
                if (test_mode) prep->setTestMode();
                prep->compile();
            }

            list<NATCompiler_pf::redirectRuleInfo> redirect_rules_info;

            for (list<FWObject*>::iterator p=all_nat.begin();
                 p!=all_nat.end(); ++p )
            {
                NAT *nat = NAT::cast(*p);

                if (nat->isV6()!=ipv6_policy) continue;

                string ruleset_name = nat->getName();
                if (nat->isTop())
                    ruleset_name = "__main__";

                if (!table_factories.count(ruleset_name))
                    table_factories[ruleset_name] = new TableFactory();

                NATCompiler_pf n( objdb, fwobjectname, ipv6_policy, oscnf,
                                  table_factories[ruleset_name] );

                n.setSourceRuleSet( nat );
                n.setRuleSetName(nat->getName());

                n.setDebugLevel( dl );
                n.setDebugRule(  drn );
                n.setVerbose( verbose );
                if (test_mode) n.setTestMode();

                int nat_rules_count = 0;
                if ( (nat_rules_count=n.prolog()) > 0 ) 
                {
                    n.compile();
                    n.epilog();
                }
                have_nat = (have_nat || (nat_rules_count > 0));

                if (nat->isTop())
                {
                    generated_scripts[ruleset_name] = main_str;
                } else
                {
                    generated_scripts[ruleset_name] = new ostringstream();
                }

                if (n.getCompiledScriptLength() > 0)
                {
                    if (n.haveErrorsAndWarnings())
                    {
                        *(generated_scripts[ruleset_name])
                            << "# NAT compiler errors and warnings:"
                            << endl;
                        *(generated_scripts[ruleset_name]) << n.getErrors("# ");
                    }
                    *(generated_scripts[ruleset_name]) << n.getCompiledScript();
                    *(generated_scripts[ruleset_name]) << endl;
                }

                conf_files[ruleset_name] = getConfFileName(ruleset_name,
                                                           fwobjectname,
                                                           fw_file_name);
                const list<NATCompiler_pf::redirectRuleInfo> lst = 
                    n.getRedirRulesInfo();
                redirect_rules_info.insert(redirect_rules_info.begin(),
                                           lst.begin(), lst.end());
            }

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p )
            {
                Policy *policy = Policy::cast(*p);

                if (policy->isV6()!=ipv6_policy) continue;

                string ruleset_name = policy->getName();
                if (policy->isTop())
                    ruleset_name = "__main__";

                if (!table_factories.count(ruleset_name))
                    table_factories[ruleset_name] = new TableFactory();

                PolicyCompiler_pf c( objdb, fwobjectname, ipv6_policy, oscnf,
                                     &redirect_rules_info,
                                     table_factories[ruleset_name] );

                c.setSourceRuleSet( policy );
                c.setRuleSetName(policy->getName());

                c.setDebugLevel( dl );
                c.setDebugRule(  drp );
                c.setVerbose( verbose );
                if (test_mode) c.setTestMode();

                int pf_rules_count = 0;
                if ( (pf_rules_count=c.prolog()) > 0 ) 
                {
                    c.compile();
                    c.epilog();
                }
                have_pf = (have_pf || (pf_rules_count > 0));

                if (policy->isTop())
                {
                    generated_scripts[ruleset_name] = main_str;
                } else
                {
                    generated_scripts[ruleset_name] = new ostringstream();
                }

                if (c.getCompiledScriptLength() > 0)
                {
                    if (c.haveErrorsAndWarnings())
                    {
                        *(generated_scripts[ruleset_name])
                            << "# Policy compiler errors and warnings:"
                            << endl;
                        *(generated_scripts[ruleset_name]) << c.getErrors("# ");
                    }
                    *(generated_scripts[ruleset_name]) << c.getCompiledScript();
                    *(generated_scripts[ruleset_name]) << endl;
                }

                conf_files[ruleset_name] = getConfFileName(ruleset_name,
                                                           fwobjectname,
                                                           fw_file_name);
            }
        }
 
/*
 * now write generated scripts to files
 */
        for (map<string, ostringstream*>::iterator fi=generated_scripts.begin();
             fi!=generated_scripts.end(); fi++)
        {
            string ruleset_name = fi->first;
            string file_name = output_dir + conf_files[ruleset_name];
            ostringstream *strm = fi->second;

            ofstream pf_file;
            pf_file.exceptions(
                ofstream::eofbit|ofstream::failbit|ofstream::badbit);

#ifdef _WIN32
            pf_file.open(file_name.c_str(), ios::out|ios::binary);
#else
            pf_file.open(file_name.c_str());
#endif

            if (ruleset_name == "__main__")
            {
                printStaticOptions(pf_file, fw);
            }
            pf_file << table_factories[ruleset_name]->PrintTables();
            pf_file << strm->str();
            pf_file.close();
        }

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
	fw_file << "#!/bin/sh " << shell_dbg << endl;

        fw_file << _("#\n\
#  This is automatically generated file. DO NOT MODIFY !\n\
#\n\
#  Firewall Builder  fwb_pf v") << VERSION << "-" << RELEASE_NUM << _(" \n\
#\n\
#  Generated ") << timestr << " " << tzname[stm->tm_isdst] << _(" by ") 
               << user_name << "\n#\n";

        fw_file << MANIFEST_MARKER << "* " << getFileName(fw_file_name) << endl;
        for (map<string,string>::iterator i=conf_files.begin();
                                          i!=conf_files.end(); ++i)
            fw_file << MANIFEST_MARKER << "  " << getFileName(i->second) << endl;

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

        fw_file << "FWDIR=`dirname $0`" << endl << endl;

	fw_file << oscnf->getCompiledScript();

        fw_file << endl;

        fw_file << "log '";
        fw_file << _("Activating firewall script generated ")
               << timestr << " " << _(" by ")
               << user_name;
        fw_file << "'" << endl;

	fw_file << endl;

        if (prolog_place == "fw_file")
            printProlog(fw_file, pre_hook);

	fw_file << endl;

        fw_file << "$PFCTL " << pfctl_dbg << pfctl_f_option
                << "${FWDIR}/" << getFileName(conf_files["__main__"])
                << " || exit 1"
                << endl;

        for (map<string,string>::iterator i=conf_files.begin();
             i!=conf_files.end(); ++i)
        {
            if (i->first != "__main__")
                fw_file << "$PFCTL " << pfctl_dbg
                        << "-a " << i->first << " "
                        << pfctl_f_option
                        << "${FWDIR}/" << i->second
                        << " || exit 1"
                        << endl;
        }
        fw_file << endl;
        fw_file << "#" << endl;
        fw_file << "# Epilog script" << endl;
        fw_file << "#" << endl;

        string post_hook= fw->getOptionsObject()->getStr("epilog_script");
        fw_file << post_hook << endl;

        fw_file << endl;
        fw_file << "# End of epilog script" << endl;
        fw_file << "#" << endl;

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









