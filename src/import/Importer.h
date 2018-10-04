/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "objectMaker.h"
#include "addressObjectMaker.h"
#include "serviceObjectMaker.h"

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include <QString>

typedef std::pair<std::string,std::string> str_tuple;
typedef std::vector<std::string> str_vector;


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
    int created_from_line_number;
    int default_action_line_number;
    bool to_be_deleted;
    
    UnidirectionalRuleSet()
    {
        created_from_line_number = -1;
        default_action_line_number = -1;
        default_action = libfwbuilder::PolicyRule::Deny;
        to_be_deleted = false;
    }
};

class ImporterException : public std::exception
{
    QString err;
public:
    ImporterException(const std::string &e) { err = e.c_str(); }
    ImporterException(const QString &e) { err = e; }
    virtual ~ImporterException() throw() {}
    QString toString() { return err; }
    virtual const char* what() const throw() { return err.toStdString().c_str(); }
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

    AddressObjectMaker *address_maker;
    ServiceObjectMaker *service_maker;

    int error_counter;

    // line number in the original stream being imported
    int current_line_number;
    
    libfwbuilder::FWObject *library;

    std::string input_file_name;
    std::istringstream &input;
    
    std::string platform;

    std::string discovered_platform;
    std::string discovered_version;

    std::string user_choice_host_os;
    std::string user_choice_version;

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

    // registry of broken objects. Sometimes we create an AddressTable
    // or a group object during import that may have some kind of a problem
    // that we leave for the user to fix manually. In order to be able to mark
    // all rules that use this object as "broken", we should register these
    // broken objects somewhere.
    std::map<libfwbuilder::FWObject*, QString> broken_objects;
    
    UnidirectionalRuleSet* current_ruleset;
    
    libfwbuilder::Rule* current_rule;

    void addAddressObjectToInterface(libfwbuilder::Interface*intf,
                                     const std::string &addr,
                                     const std::string &netm);
    std::string getBadRuleColor();

    // this method returns fw. It is created if fw==nullptr
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
    bool haveFirewallObject() { return (fw!=nullptr); }

    // checks if ruleset "rsname" exists. Returns pointer if yes,
    // otherwise returns nullptr
    virtual UnidirectionalRuleSet* checkUnidirRuleSet(const std::string &rsname);

    // finds and rturns pointer to ruleset "rsname". If it does not
    // exists, it is created
    virtual UnidirectionalRuleSet* getUnidirRuleSet(
        const std::string &ruleset_name, const std::string &ruleset_type_name);

    virtual libfwbuilder::FWObject* createTCPService(const QString &name="");
    virtual libfwbuilder::FWObject* createUDPService(const QString &name="");

    // create libfwbuilder::ObjectGroup and place all interfaces in it
    // argument represents a list of interface names
    virtual libfwbuilder::FWObject* createGroupOfInterfaces(
        const std::string &ruleset_name, std::list<std::string> &interfaces);

    virtual libfwbuilder::FWObject* makeAddressObj(const std::string addr,
                                                   const std::string netm);
    
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

    virtual void addLogging();

    void registerBrokenObject(libfwbuilder::FWObject *o, const QString &err);
    bool isObjectBroken(libfwbuilder::FWObject*);
    QString getBrokenObjectError(libfwbuilder::FWObject*);
    
public:

    ObjectMakerErrorTracker *error_tracker;
    
    // making logger public so I can access it from the code in the grammar
    libfwbuilder::Logger *logger;
    
    QStringList last_comment;
    bool add_standard_comments;
    
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
    std::string tmp_port_spec_2;

    std::string tmp_range_1;
    std::string tmp_range_2;

    int  tmp_tcp_flag_code;
    QList<int> tmp_tcp_flags_list;
    QList<int> tcp_flags_mask;
    QList<int> tcp_flags_comp;
    
    bool  logging;
    std::string log_level;
    std::string log_interval;
    
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

    void setSrcSelf();
    void setDstSelf();
    
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
    void prepareForDeduplication();
    
    // add standard line to rule comment, this adds something like
    // "created during import from <file>, line <line>"
    virtual void addStandardImportComment(libfwbuilder::FWObject *obj,
                                          const QString &additional_comment);
    
    virtual libfwbuilder::FWObject* commitObject(libfwbuilder::FWObject *obj);
    
    int errorCounter() { return error_counter; }

    virtual void setDiscoveredPlatform(const std::string &v);
    virtual void setDiscoveredVersion(const std::string &v);

    void setUserChoiceHostOS(const std::string &s) { user_choice_host_os = s; }
    void setUserChoiceVersion(const std::string &s) { user_choice_version = s; }
    void setAddStandardCommentsFlag(bool f) { add_standard_comments = f; }
    
    virtual void setHostName(const std::string &hn);
    virtual libfwbuilder::Interface* newInterface(const std::string &interface_name);
    virtual void clearCurrentInterface() { current_interface = nullptr; }
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

    virtual void setInterfaceAndDirectionForRuleSet(
        libfwbuilder::Interface *intf,
        const std::string &ruleset_name,
        const std::string &dir);

    virtual void newPolicyRule();
    virtual void newNATRule();
    virtual void pushRule();

    void setCurrentLineNumber(int n) { current_line_number = n; }
    int getCurrentLineNumber() { return current_line_number;}
    
    void markCurrentRuleBad();
    void reportError(const std::string &comment);
    void reportError(const QString &comment);
    
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
    void addMessageToLog(const QString &msg);

    
    void rearrangeVlanInterfaces();

};

#endif
