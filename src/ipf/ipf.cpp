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

#include "config.h"
 
#include <qsettings.h>

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
#include <assert.h>

#include "PolicyCompiler_ipf.h"
#include "NATCompiler_ipf.h"

#include "OSConfigurator_openbsd.h"
#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_solaris.h"

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
static string           ipf_file_name  = "";
static string           nat_file_name  = "";
static int              dl             = 0;
static int              drp            = -1;
static int              drn            = -1;
static int              verbose        = 0;
static bool             test_mode      = false;

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
    
void usage(const char *name)
{
    cout << _("Firewall Builder:  policy compiler for ipfilter") << endl;
    cout << _("Version ") << VERSION << RELEASE_NUM << endl;
    cout << _("Usage: ") << name << " [-x] [-v] [-V] [-f filename.xml] [-o output.fw] [-d destdir] [-m] firewall_object_name" << endl;
}

string printActivationCommandWithSubstitution(Firewall *fw,string filePath,string cmd)
{
    ostringstream str;

    str << "cat " << filePath << " | grep -v '#' ";

    FWObjectTypedChildIterator j=fw->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j ) 
    {
        Interface *iface=Interface::cast(*j);
        if ( iface->isDyn() )
        {
            str << "| sed \"s/ (" << iface->getName() << ") "
                << "/ $i_" << iface->getName() << " /\"";
        }
    }
    str << " | " << cmd << endl;
    return str.str();
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

    while( (opt=getopt(argc,argv,"x:vVf:d:r:o:")) != EOF )
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
        }
    }
    
    if((argc-1) != optind)
    {
        usage(argv[0]);
        exit(1);
    }

    fwobjectname = strdup( argv[optind++] );

    if (fw_file_name.empty())
    {
        fw_file_name=string(fwobjectname)+".fw";
        ipf_file_name=string(fwobjectname)+"-ipf.conf";
        nat_file_name=string(fwobjectname)+"-nat.conf";
    } else
    {
        string::size_type n = fw_file_name.rfind(".");
        ipf_file_name = fw_file_name;
        ipf_file_name.erase(n);
        ipf_file_name.append("-ipf.conf");
        nat_file_name = fw_file_name;
        nat_file_name.erase(n);
        nat_file_name.append("-nat.conf");
    }

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
	Firewall*  fw=objdb->findFirewallByName(fwobjectname);
 
	if (verbose) cout << _(" *** Data checks ...");

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

	if (verbose) cout << _(" done\n");

	FWOptions* options=fw->getOptionsObject();
	string s;

	string firewall_dir=options->getStr("firewall_dir");
	if (firewall_dir=="") firewall_dir="/etc/fw";

	bool debug=options->getBool("debug");
	string shell_dbg=(debug)?"-x":"" ;
	string ipf_dbg=(debug)?"-v":"";

        Preprocessor* prep=new Preprocessor(objdb , fwobjectname, false);
        prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
	OSConfigurator *oscnf=NULL;
        string family=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceStr("/FWBuilderResources/Target/family");
        if ( family=="solaris" )
            oscnf=new OSConfigurator_solaris(objdb , fwobjectname, false);

	if ( family=="openbsd")
            oscnf=new OSConfigurator_openbsd(objdb , fwobjectname, false);

	if ( family=="freebsd")
            oscnf=new OSConfigurator_freebsd(objdb , fwobjectname, false);

	if (oscnf==NULL)
	    throw FWException(_("Unrecognized host OS ")+fw->getStr("host_OS")+"  (family "+family+")");

	oscnf->prolog();

/*
 * create compilers and run the whole thing 
 */
	PolicyCompiler_ipf c( objdb , fwobjectname, false , oscnf );

	c.setDebugLevel( dl );
	c.setDebugRule(  drp );
	c.setVerbose( verbose );
        if (test_mode) c.setTestMode();

	bool     have_ipf=false;
	if ( c.prolog() > 0 ) {
          have_ipf=true;
          c.compile();
          c.epilog();
	}


	NATCompiler_ipf n( objdb , fwobjectname, false , oscnf );

	n.setDebugLevel( dl );
	n.setDebugRule(  drn );
	n.setVerbose( verbose );
        if (test_mode) n.setTestMode();

	bool     have_nat=false;
	if ( n.prolog() > 0 ) {
          have_nat=true;
          n.compile();
          n.epilog();
	}
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
#  Firewall Builder  fwb_ipf v") << VERSION << "-" << RELEASE_NUM << _(" \n\
#\n\
#  Generated ") << timestr << " " << tzname[stm->tm_isdst] << _(" by ") 
               << user_name << "\n#\n#\n";

        fw_file << MANIFEST_MARKER << "* " << fw_file_name << endl;
      	if (have_ipf) 
            fw_file << MANIFEST_MARKER << "  " << ipf_file_name << endl;
      	if (have_nat) 
            fw_file << MANIFEST_MARKER << "  " << nat_file_name << endl;
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

        fw_file << endl 
                << "$IPF -Fa" << endl
                << "$IPNAT -C" << endl;

/*
 *  we add prolog and epilog to the activation shell script rather
 *  than to ipf and nat .conf files. This is more flexible since user
 *  can execute some shell commands, as well as add any policy and/or
 *  nat rules by putting them into their .conf file and loading them
 *  from prolog or epilog script. Because of this, prolog is added
 *  after all policy and nat rules are flushed.
 */ 
        fw_file << endl;
        fw_file << "#" << endl;
        fw_file << "# Prolog script" << endl;
        fw_file << "#" << endl;
        
        string pre_hook= fw->getOptionsObject()->getStr("prolog_script");
        fw_file << pre_hook << endl;

        fw_file << "#" << endl;
        fw_file << "# End of prolog script" << endl;
        fw_file << "#" << endl;

        unlink(ipf_file_name.c_str());
      	if (have_ipf) 
        {
            ofstream ipf_file;
            ipf_file.exceptions(ofstream::eofbit|ofstream::failbit|ofstream::badbit);
        
#ifdef _WIN32
            ipf_file.open(ipf_file_name.c_str(), ios::out|ios::binary);
#else
            ipf_file.open(ipf_file_name.c_str());
#endif

            if (c.haveErrorsAndWarnings())
            {
                ipf_file << "# Policy compiler errors and warnings:"
                         << endl;
                ipf_file << c.getErrors("# ");
            }

	    ipf_file << c.getCompiledScript();
	    ipf_file.close();

            string cmd = string("$IPF ")+ipf_dbg+" -I -f ";
            string filePath = string("${FWDIR}/")+ipf_file_name;
            if (fw->getOptionsObject()->getBool("dynAddr"))
            {
                cmd += "-";
                fw_file << 
                    printActivationCommandWithSubstitution(fw,filePath,cmd);
            } else
            {
                fw_file << cmd << filePath << endl;
            }
	}

        unlink(nat_file_name.c_str());
	if (have_nat) 
        {
            ofstream nat_file;
            nat_file.exceptions(ofstream::eofbit|ofstream::failbit|ofstream::badbit);

#ifdef _WIN32
            nat_file.open(nat_file_name.c_str(), ios::out|ios::binary);
#else
            nat_file.open(nat_file_name.c_str());
#endif

            if (n.haveErrorsAndWarnings())
            {
                nat_file << "# NAT compiler errors and warnings:"
                         << endl;
                nat_file << n.getErrors("# ");
            }


	    nat_file << n.getCompiledScript();
	    nat_file.close();

            string cmd = string("$IPNAT ")+ipf_dbg+" -f ";
            string filePath = string("${FWDIR}/")+nat_file_name;
            if (fw->getOptionsObject()->getBool("dynAddr"))
            {
                cmd += "-";
                fw_file << 
                    printActivationCommandWithSubstitution(fw,filePath,cmd);
            } else
            {
                fw_file << cmd << filePath << endl;
            }
	}

	if (have_ipf)
	    fw_file << "$IPF " << ipf_dbg << " -s " << endl;

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

        fw_file << "/sbin/kldstat -n ipl.ko > /dev/null 2>&1 || $IPF -E" << endl;

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









