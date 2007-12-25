/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: ipt.cpp 1459 2007-12-15 05:56:12Z vk $

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

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
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
#include <assert.h>

#include "PolicyCompiler_ipt.h"
#include "MangleTableCompiler_ipt.h"
#include "NATCompiler_ipt.h"
#include "RoutingCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwcompiler/Preprocessor.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

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
static bool             omit_timestamp = false;
static int              drn            = -1;
static int              verbose        = 0;
static bool             have_dynamic_interfaces = false;
static bool             test_mode      = false;

FWObjectDatabase       *objdb = NULL;

class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string &msg) const 
    { 
	cout << _("Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it.") << endl;
	return false;
    }
};

string addPrologScript(bool nocomment,const string &script)
{
    string res="";

    if ( !nocomment )
    {
        res += "\n";
        res += "#\n";
        res += "# Prolog script\n";
        res += "#\n";
    }

    res += script;
    res += "\n";

    if ( !nocomment )
    {
        res += "#\n";
        res += "# End of prolog script\n";
        res += "#\n";
    }
    return res;
}

void usage(const char *name)
{
    cout << _("Firewall Builder:  policy compiler for Linux 2.4.x and 2.6.x iptables") << endl;
    cout << _("Version ") << VERSION << "-" << RELEASE_NUM << endl;
    cout << _("Usage: ") << name << _(" [-x level] [-v] [-V] [-q] [-f filename.xml] [-d destdir] [-m] firewall_object_name") << endl;
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

    while( (opt=getopt(argc,argv,"x:vVqf:d:r:o:")) != EOF )
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
        case 'q':
            omit_timestamp = true;
            break;
        }
    }
    
    if((argc-1) != optind)
    {
        usage(argv[0]);
        exit(1);
    }

    fwobjectname = strdup( argv[optind++] );

    if (fw_file_name.empty())
        fw_file_name=string(fwobjectname)+".fw";

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

	if (verbose) cout << _(" done\n");

        //objdb->dump(true,true);

        FWObject *slib = objdb->findInIndex("syslib000");
        if ( slib->isReadOnly()) slib->setReadOnly(false);

	/* Review firewall and OS options and generate commands */
	Firewall*  fw=objdb->findFirewallByName(fwobjectname);
	FWOptions* options=fw->getOptionsObject();
	string s;

        /* some initial sanity checks */

        list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            string::size_type n;
            if ( (n=iface->getName().find("*"))!=string::npos) 
            {
/* this is a special 'wildcard' interface. Its name must end with '*',
 * it must be dynamic and should not have a child IPv4 or
 * physAddress object
 */

                if (n!=iface->getName().length()-1)
                {
                    char errstr[256];
                    sprintf(errstr,
    _("'*' must be the last character in the wildcard's interface name: '%s'."),
                            iface->getName().c_str() );
                    throw FWException(errstr);
                }
/*
  removed test to implement RFE #837238: "unnummbered wildcard interfaces"

                if (!iface->isDyn())
                {
                    char errstr[256];
                    sprintf(errstr,
                            _("Wildcard interface '%s' must be dynamic."),
                            iface->getName().c_str() );
                    throw FWException(errstr);
                }
*/
                list<FWObject*> l3=iface->getByType(physAddress::TYPENAME);
                if (l3.size()>0)
                {
                    char errstr[256];
                    sprintf(errstr,
_("Wildcard interface '%s' should not have a physcal address object attached to it. The physical address object will be ignored.\n"),
                            iface->getName().c_str() );
                    cerr << errstr;
                    for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                        iface->remove(*j);
                }
            }

            if ( iface->isDyn())  
            {
                have_dynamic_interfaces=true;

                iface->setBool("use_var_address",true);

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
                    if ( ipv4->getAddress().toString()=="0.0.0.0")
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

	string firewall_dir=options->getStr("firewall_dir");
	if (firewall_dir=="") firewall_dir="/etc";

	bool debug=options->getBool("debug");
	string shell_dbg=(debug)?"set -x":"" ;
	string pfctl_dbg=(debug)?"-v":"";

        Preprocessor* prep=new Preprocessor(objdb , fwobjectname);
        prep->compile();

	OSConfigurator_linux24 *oscnf=NULL;
        string family=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceStr("/FWBuilderResources/Target/family");
        if ( family=="linux24" )
            oscnf=new OSConfigurator_linux24(objdb , fwobjectname);

	if (oscnf==NULL)
	    throw FWException(_("Unrecognized host OS ")+fw->getStr("host_OS")+"  (family "+family+")");

        oscnf->prolog();

        int policy_rules_count  = 0;
        int mangle_rules_count  = 0;
        int nat_rules_count     = 0;
        int routing_rules_count = 0;

	MangleTableCompiler_ipt m( objdb , fwobjectname , oscnf );

	m.setDebugLevel( dl );
	m.setDebugRule(  drp );
	m.setVerbose( (bool)(verbose) );
        m.setHaveDynamicInterfaces(have_dynamic_interfaces);
        if (test_mode) m.setTestMode();

	if ( (mangle_rules_count=m.prolog()) > 0 )
        {
	    m.compile();
	    m.epilog();
	}

// compile NAT rules before policy rules because policy compiler
// needs to know the number of virtual addresses being created for NAT

	NATCompiler_ipt n( objdb , fwobjectname , oscnf );

	n.setDebugLevel( dl );
	n.setDebugRule(  drn );
	n.setVerbose( (bool)(verbose) );
        n.setHaveDynamicInterfaces(have_dynamic_interfaces);
        if (test_mode) n.setTestMode();

	if ( (nat_rules_count=n.prolog()) > 0 )
        {
            oscnf->generateCodeForProtocolHandlers(true);
	    n.compile();
	    n.epilog();
	} else
            oscnf->generateCodeForProtocolHandlers(false);

	PolicyCompiler_ipt c( objdb , fwobjectname , oscnf );

	c.setDebugLevel( dl );
	c.setDebugRule(  drp );
	c.setVerbose( (bool)(verbose) );
        c.setHaveDynamicInterfaces(have_dynamic_interfaces);
        if (test_mode) c.setTestMode();

	if ( (policy_rules_count=c.prolog()) > 0 )
        {
	    c.compile();
	    c.epilog();
	}

        RoutingCompiler_ipt r( objdb , fwobjectname , oscnf );

	r.setDebugLevel( dl );
	r.setDebugRule(  drp );
	r.setVerbose( verbose );
        if (test_mode) r.setTestMode();

	if ( (routing_rules_count=r.prolog()) > 0 )
        {
	    r.compile();
	    r.epilog();
	}

        oscnf->printChecksForRunTimeMultiAddress();
        oscnf->processFirewallOptions();
        oscnf->configureInterfaces();
        oscnf->printCommandsToAddVirtualAddressesForNAT();

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
/*
 * assemble the script and then perhaps post-process it if it should
 * run on Linksys device with sveasoft firmware
 */

        ostringstream script;

	script << "#!/bin/sh "  << endl;

        script << _("#\n\
#  This is automatically generated file. DO NOT MODIFY !\n\
#\n\
#  Firewall Builder  fwb_ipt v") << VERSION << "-" << RELEASE_NUM << _(" \n");

        if (!omit_timestamp)
        {
            script << _("#\n\
#  Generated ") << timestr << " " << tzname[stm->tm_isdst] << _(" by ") 
               << user_name << "\n#\n";
        }

        script << MANIFEST_MARKER << "* " << fw_file_name << endl;
        script << "#" << endl;
        script << "#" << endl;

/* do not put comment in the script if it is intended for linksys */
        bool nocomm=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_comments");
        if ( !nocomm )
        {
            string fwcomment=fw->getComment();
            string::size_type n1,n2;
            n1=n2=0;
            while ( (n2=fwcomment.find("\n",n1))!=string::npos )
            {
                script << "#  " << fwcomment.substr(n1,n2-n1) << endl;
                n1=n2+1;
            }
            script << "#  " << fwcomment.substr(n1) << endl;
            script << "#\n#\n#\n";
        }

        script << shell_dbg << endl;
        script << endl;

        script << "PATH=\"/sbin:/usr/sbin:/bin:/usr/bin:${PATH}\"" << endl;
        script << "export PATH" << endl;
        script << endl;

/*
 * print definitions for variables IPTABLES, IP, LOGGER. Some day we may
 * add a choice of distro in the GUI. Right now paths are either default
 * for a given distro, or custom strings entered by user in the GUI and stored
 * in firewall options.
 */
        script << oscnf->printPathForAllTools(DISTRO);

        string prolog_place= fw->getOptionsObject()->getStr("prolog_place");
        if (prolog_place == "") prolog_place="top";

        if (prolog_place == "top")
        {
            script <<
                addPrologScript(nocomm, 
                                fw->getOptionsObject()->getStr("prolog_script"));
        }

	script << oscnf->getCompiledScript();

        script << endl;

        if (prolog_place == "after_interfaces")
        {
            script <<
                addPrologScript(nocomm, 
                                fw->getOptionsObject()->getStr("prolog_script"));
        }

        script << "log '";
        if (omit_timestamp)
        {
            script << _("Activating firewall script");
        } else
        {
        script << _("Activating firewall script generated ")
               << timestr << " " << _(" by ")
/* timezone removed because of bug #1205665 - sometimes timezone name
 * has "'" in it which confuses shell and causes an error (for
 * instance French daylight savings time is "Paris, Madrid (heure
 * d'ete)" where 'e' are actually accented 'e')
 * 
 *               << timestr << " " << tzname[stm->tm_isdst] << _(" by ")
 */
               << user_name;
        }

        script << "'" << endl;

	script << endl;

        if (options->getBool("use_iptables_restore"))
        {
            if (have_dynamic_interfaces)
            {
                script << "(" << endl;

                script << c.flushAndSetDefaultPolicy();

                if (prolog_place == "after_flush")
                {
                    script << addPrologScript(nocomm, 
                                 fw->getOptionsObject()->getStr("prolog_script"));
                }

                script << c.getCompiledScript();
                script << c.commit();

                if (m.getCompiledScriptLength()>0)
                {
                    script << m.flushAndSetDefaultPolicy();
                    script << m.getCompiledScript();
                    script << m.commit();
                }
                if (n.getCompiledScriptLength()>0)
                {
                    script << n.flushAndSetDefaultPolicy();
                    script << n.getCompiledScript();
                    script << n.commit();
                }
                script << "#" << endl;
                script << ") | $IPTABLES_RESTORE" << endl;
            } else
            {
                script << "cat << EOF | $IPTABLES_RESTORE" << endl;

                script << c.flushAndSetDefaultPolicy();

                if (prolog_place == "after_flush")
                {
                    script << addPrologScript(nocomm, 
                                 fw->getOptionsObject()->getStr("prolog_script"));
                }

                script << c.getCompiledScript();
                script << c.commit();

                if (m.getCompiledScriptLength()>0)
                {
                    script << m.flushAndSetDefaultPolicy();
                    script << m.getCompiledScript();
                    script << m.commit();
                }
                if (n.getCompiledScriptLength()>0)
                {
                    script << n.flushAndSetDefaultPolicy();
                    script << n.getCompiledScript();
                    script << n.commit();
                }
                script << "#" << endl;
                script << "EOF" << endl;
            }
        } else
        {

            script << c.flushAndSetDefaultPolicy();
            if (m.getCompiledScriptLength()>0)
                script << m.flushAndSetDefaultPolicy();
            if (n.getCompiledScriptLength()>0)
                script << n.flushAndSetDefaultPolicy();

            if (prolog_place == "after_flush")
            {
                script << addPrologScript(nocomm, 
                             fw->getOptionsObject()->getStr("prolog_script"));
            }

            if (n.getCompiledScriptLength()>0)
            {
                script << n.getCompiledScript();
                script << n.commit();
            }

            if (m.getCompiledScriptLength()>0)
            {
                script << m.getCompiledScript();
                script << m.commit();
            }

            script << c.getCompiledScript();
            script << c.commit();
        }
        script << r.getCompiledScript();




        oscnf->epilog();
	script << oscnf->getCompiledScript();

        if ( !nocomm )
        {
            script << endl;
            script << "#" << endl;
            script << "# Epilog script" << endl;
            script << "#" << endl;
        }

        string post_hook= fw->getOptionsObject()->getStr("epilog_script");
        script << post_hook << endl;

        if ( !nocomm )
        {
            script << endl;
            script << "# End of epilog script" << endl;
            script << "#" << endl;
        }

	script << endl;

        string sbuf = script.str();

/* starting with 2.0.3 we copy script to linksys using scp and do not
 * need to escape double quotes and '$' anymore
 */

#if 0
        if ( Resources::getTargetOptionBool(fw->getStr("host_OS"),
                                            "escape_everything") )
        {
/* need to escape single and double quotes, as well as '$' in the script */

            string::size_type i;

            i = 0;
            while ( (i=sbuf.find('\"',i))!=string::npos )
            {
                sbuf.replace(i,1,"\\\"");
                i+=2;
            }

            i = 0;
            while ( (i=sbuf.find('\'',i))!=string::npos )
            {
                sbuf.replace(i,1,"\\\'");
                i+=2;
            }

            i = 0;
            while ( (i=sbuf.find('`',i))!=string::npos )
            {
                sbuf.replace(i,1,"\\`");
                i+=2;
            }

            i = 0;
            while ( (i=sbuf.find('$',i))!=string::npos )
            {
                sbuf.replace(i,1,"\\$");
                i+=2;
            }
        }
#endif

	ofstream fw_file;
        fw_file.exceptions(ofstream::eofbit|ofstream::failbit|ofstream::badbit);

#ifdef _WIN32
        fw_file.open(fw_file_name.c_str(), ios::out|ios::binary);
#else
        fw_file.open(fw_file_name.c_str());
#endif

        fw_file << sbuf << endl;
	fw_file.close();

#ifdef _WIN32
        _chmod(fw_file_name.c_str(),_S_IREAD|_S_IWRITE);
#else
        chmod(fw_file_name.c_str(),S_IXUSR|S_IRUSR|S_IWUSR|S_IRGRP);
#endif

        cout << _(" Compiled successfully") << endl << flush;
        
        return 0;

    } catch(const FWException &ex)  {
	cerr << "Error: " << ex.toString() << endl;
        return 1;
#if __GNUC__ >= 3
/* need to check version because std::ios::failure does not seem to be
 * supported in gcc 2.9.5 on FreeBSD 4.10 */
    } catch (const std::ios::failure &e) {
        cerr << "Error while opening or writing to the output file" << endl;
        return 1;
#endif
    } catch (const std::string &s) {
	cerr << s << endl;
        return 1;
    } catch (const std::exception &ex) {
	cerr << ex.what() << endl;
        return 1;
    } catch (...) {
	cerr << _("Unsupported exception") << endl;
        return 1;
    }

}
