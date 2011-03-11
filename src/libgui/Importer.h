/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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


#ifndef _FWB_POLICY_IMPORTER_H_
#define _FWB_POLICY_IMPORTER_H_

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Logger.h"

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include <QString>


class Importer;

/*
 * Used for platforms where interface and direction are set for the
 * whole ruleset (like in router access lists), as opposed to
 * platforms where interface and direction are set on a per-rule basis
 * (iptables)
 */


class UnidirectionalRuleSet
{

public:
    libfwbuilder::RuleSet* ruleset;
    std::string name;
    // interface names and directions
    std::map<std::string,std::string> intf_dir;
    libfwbuilder::PolicyRule::Action default_action;

    UnidirectionalRuleSet() { default_action = libfwbuilder::PolicyRule::Deny; }
};

class ImporterException : public std::exception
{
    std::string err;
public:
    ImporterException(const std::string &e) { err = e; }
    virtual ~ImporterException() throw() {}
    std::string toString() { return err; }
};

class Importer
{

    // firewall object
    // one instance of Importer creates only one firewall object.
    //
    // Do not access this member directly, always use getFirewallObject()
    // This ensures the object is created only when it is needed
    // so that if we get ane xception in parser early, we do not
    // create unnecessary object

    libfwbuilder::Firewall *fw;
    std::string fwname;
   
protected:

    int error_counter;

    // line number in the original stream being imported
    int current_line_number;
    
    libfwbuilder::FWObject *library;

    std::string input_file_name;
    std::istringstream &input;
    
    std::string platform;

    std::string discovered_version;
    
    libfwbuilder::Interface* current_interface;
    
    // map :  ruleset name : ruleset
    // in case of IOS ACls or PIX policy ruleset name == acl name
    // all other platforms have single ruleset for policy
    // and another for NAT
    std::map<const std::string,UnidirectionalRuleSet*>  all_rulesets;

    // map :  interface name : interface
    std::map<const std::string,libfwbuilder::Interface*>  all_interfaces;

    // map :  object signature : object
    // use this to quickly find objects to avoid creating duplicates
    std::map<const std::string,libfwbuilder::FWObject*>   all_objects;

    int custom_service_code_tracker;
    std::map<const std::string, int> custom_service_codes;

    UnidirectionalRuleSet* current_ruleset;
    
    libfwbuilder::Rule* current_rule;

    libfwbuilder::FWObject* createObject(const std::string &objType,
                                         const std::string &objName);
    libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                         const std::string &objType,
                                         const std::string &objName);
    void addAddressObjectToInterface(libfwbuilder::Interface*intf,
                                     const std::string &addr,
                                     const std::string &netm);
    std::string getBadRuleColor();

    // this method returns fw. It is created if fw==NULL
    // Using getFirewallObject() instead of accessing fw directly
    // provides a way to create firewall object only when
    // it is really needed.
    libfwbuilder::Firewall* getFirewallObject();


    // need to be able to tell if firewall object has really
    // been created during import. If the file is empty or in case of
    // a parser error firewall object may not have been created.
    // However in other cases there could have been an error after
    // the object was created. This method allows us to tell one
    // situation from another.
    bool haveFirewallObject() { return (fw!=NULL); }

    // checks if ruleset "rsname" exists. Returns pointer if yes,
    // otherwise returns NULL
    virtual UnidirectionalRuleSet* checkUnidirRuleSet(const std::string &rsname);

    // finds and rturns pointer to ruleset "rsname". If it does not
    // exists, it is created
    virtual UnidirectionalRuleSet* getUnidirRuleSet(
        const std::string &ruleset_name, const std::string &ruleset_type_name);
    
    virtual libfwbuilder::FWObject* getCustomService(const std::string &platform,
                                                     const std::string &code,
                                                     const std::string &protocol);
    virtual libfwbuilder::FWObject* getIPService(int proto);
    virtual libfwbuilder::FWObject* getICMPService(int type, int code);

    virtual libfwbuilder::FWObject* getTCPService(int srs, int sre,
                                                  int drs, int dre,
                                                  bool established,
                                                  std::list<int> &flags_mask,
                                                  std::list<int> &flags_comp);

    virtual libfwbuilder::FWObject* getUDPService(int srs, int sre,
                                                  int drs, int dre);

    virtual libfwbuilder::FWObject* getTagService(const std::string &tagcode);

    
    virtual libfwbuilder::FWObject* createAddress(const std::string &a,
                                                  const std::string &nm);
    virtual libfwbuilder::FWObject* createAddressRange(const std::string &a1,
                                                       const std::string &a2);
    virtual libfwbuilder::FWObject* createIPService();
    virtual libfwbuilder::FWObject* createICMPService();
    virtual libfwbuilder::FWObject* createTCPService();
    virtual libfwbuilder::FWObject* createUDPService();

    // create libfwbuilder::ObjectGroup and place all interfaces in it
    // argument represents a list of interface names
    virtual libfwbuilder::FWObject* createGroupOfInterfaces(
        const std::string &ruleset_name, std::list<std::string> &interfaces);

    virtual libfwbuilder::FWObject* makeSrcObj();
    virtual libfwbuilder::FWObject* makeDstObj();
    virtual libfwbuilder::FWObject* makeSrvObj();
    
    // importer may need to create multiple objects for
    // either rule element for some platforms. It is more convenient to
    // make these special virtual methods rather than use createAddress
    // and createService every time.
    virtual void addSrc();
    virtual void addDst();
    virtual void addSrv();

    virtual void addOSrc();
    virtual void addODst();
    virtual void addOSrv();
    
public:

    // making logger public so I can access it from the code in the grammar
    libfwbuilder::Logger *logger;
    
    // temporary variables used by parser to store values
    // Importer converts these into a proper rule using method
    // pushRule()
    // Method clear() resets all these variables to their defaults.
    //
    // TODO:  need to add more variables to cover everything needed
    // for NAT rules
    
    std::string action;
    std::string protocol;
    std::string rule_comment;

    std::string src_a;
    std::string src_nm;
    std::string src_port_op;
    std::string src_port_spec;

    std::string dst_a;
    std::string dst_nm;
    std::string dst_port_op;
    std::string dst_port_spec;

    std::string tmp_a;
    std::string tmp_nm;
    std::string tmp_port_op;
    std::string tmp_port_spec;

    int            tmp_tcp_flag_code;
    std::list<int> tmp_tcp_flags_list;
    std::list<int> tcp_flags_mask;
    std::list<int> tcp_flags_comp;
    std::map<int,std::string> tcp_flag_names;
    
    bool  logging;
    bool  established;
    bool  fragments;

    std::string icmp_spec;
    std::string icmp_code;
    std::string icmp_type;

    std::string time_range_name;

    void SaveTmpAddrToSrc();
    void SaveTmpAddrToDst();

    void SaveTmpPortToSrc();
    void SaveTmpPortToDst();

    virtual void clear();
    
    Importer(libfwbuilder::FWObject *lib,
             const std::string      &platform,
             std::istringstream     &input,
             libfwbuilder::Logger   *log,
             const std::string &fwname);
    virtual ~Importer();

    virtual void run();

    void setFileName(const std::string &fn) { input_file_name = fn; }
    void setPlatform(const std::string &pl) { platform = pl; }

    // add standard line to rule comment, this adds something like
    // "created during import from <file>, line <line>"
    std::string addStandardRuleComment(const std::string &comment);
    
    int errorCounter() { return error_counter; }

    virtual void setDiscoveredVersion(const std::string &v);
    virtual void setHostName(const std::string &hn);
    virtual void newInterface(const std::string &interface_name);
    virtual void clearCurrentInterface() { current_interface = NULL; }
    virtual void ignoreCurrentInterface();
    virtual void addInterfaceAddress(const std::string &a,
                                     const std::string &nm);
    virtual void addInterfaceAddress(const std::string &label,
                                     const std::string &a,
                                     const std::string &nm);
    virtual void setInterfaceComment(const std::string &descr);
    virtual void setInterfaceLabel(const std::string &descr);
    virtual void setInterfaceSecurityLevel(const std::string &seclevel);
    virtual void setInterfaceParametes(const std::string &phys_intf,
                                       const std::string &label,
                                       const std::string &sec_level);
    virtual void setInterfaceVlanId(const std::string &vlan_id);

    virtual void addRuleComment(const std::string &comm);

    /**
     * create new unidirectional ruleset. Unidirectional ruleset
     * has interface association and direction that apply to all rules
     * in the set.
     */
    virtual void newUnidirRuleSet(const std::string &name,
                                  const std::string &ruleset_type);

    /**
     * Sets default action for the current rule set.
     */
    virtual void setDefaultAction(const std::string &iptables_action_name);

    /**
     * add interface and direction setting to a ruleset. Note that the
     * same ruleset may be associated with multiple interfaces and 
     * each association may have its own direction.
     */
    virtual void setInterfaceAndDirectionForRuleSet(
        const std::string &ruleset_name,
        const std::string &interface_name,
        const std::string &dir);

    virtual void newPolicyRule();
    virtual void newNATRule();
    virtual void pushRule();

    void setCurrentLineNumber(int n) { current_line_number = n; }
    int getCurrentLineNumber() { return current_line_number;}
    
    void markCurrentRuleBad(const std::string &comment);
    
    
    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();

    int countRules();
    int countInterfaces();
    
    QString noFirewallErrorMessage();
    QString noRulesErrorMessage();
    QString noInterfacesErrorMessage();
    QString commonFailureErrorMessage();
    
    // statistics
    int getNumberOfRuleSets();
    int getTotalNumberOfRules();
    int getNumberOfInterfaces();

    void addMessageToLog(const std::string &msg);
};

#endif
