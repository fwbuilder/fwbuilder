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


#ifndef _FWB_POLICY_IMPORTER_IPT_H_
#define _FWB_POLICY_IMPORTER_IPT_H_

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include "Importer.h"

#include "fwbuilder/Logger.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"

#include <QString>
#include <QStringList>
#include <QMap>


class IPTImporter : public Importer
{

    QMap<QString, QString> reject_action_arg_mapping;
    int aux_branch_number;

    libfwbuilder::FWObject* createTCPUDPService(str_tuple &src_range,
                                                str_tuple &dst_range,
                                                const std::string &proto);

    libfwbuilder::FWObject* createTCPUDPService(const std::string &proto);
    
    virtual libfwbuilder::FWObject* createTCPService(const QString &name="");
    virtual libfwbuilder::FWObject* createUDPService(const QString &name="");

    virtual libfwbuilder::FWObject* makeSrcObj();
    virtual libfwbuilder::FWObject* makeDstObj();
    
    void processModuleMatches();
    void addAllModuleMatches(libfwbuilder::PolicyRule *rule);
    void addMarkMatch(libfwbuilder::PolicyRule *rule);
    void addLengthMatch(libfwbuilder::PolicyRule *rule);
    void addLimitMatch(libfwbuilder::PolicyRule *rule);
    void addRecentMatch(libfwbuilder::PolicyRule *rule);
    void addPktTypeMatch(libfwbuilder::PolicyRule *rule);
    void addStateMatch(libfwbuilder::PolicyRule *rule, const std::string &state);

    std::string getBranchName(const std::string &suffix);
    
    libfwbuilder::PolicyRule* createPolicyBranch(
        libfwbuilder::PolicyRule *rule, const std::string &branch_name,
        bool clear_rule_elements, bool make_stateless);

    libfwbuilder::NATRule* createNATBranch(
        libfwbuilder::NATRule *rule, const std::string &branch_name,
        bool clear_rule_elements);

    public:

    int service_group_name_seed;

    std::string current_table;
    std::string current_chain;
    std::string current_state;
    
    std::string i_intf;
    std::string o_intf;
    std::string target;

    std::string tmp_port_range_start;
    std::string tmp_port_range_end;

    std::list<str_tuple> src_port_list;
    std::list<str_tuple> dst_port_list;
    std::list<str_tuple> both_port_list;

    std::map<std::string, std::string> action_params;

    // need to keep track of branches in 2.1
    // should not be neccessary in 3.0 when multiple
    // rule can refer to the same branch ruleset
    std::map<std::string, UnidirectionalRuleSet*> branch_rulesets;

    std::string match_mark;
    bool neg_match_mark;
    
    bool src_neg;
    bool dst_neg;
    bool srv_neg;
    bool intf_neg;
    bool tmp_neg;
    
    std::string limit_val;
    std::string limit_suffix;
    std::string limit_burst;
    std::string length_spec;
    std::string recent_match;
    std::string pkt_type_spec;
    
    std::string nat_addr1;
    std::string nat_addr2;
    std::string nat_nm;
    std::string nat_port_range_start;
    std::string nat_port_range_end;

    bool using_iprange_src;
    std::string iprange_src_from;
    std::string iprange_src_to;
    bool using_iprange_dst;
    std::string iprange_dst_from;
    std::string iprange_dst_to;
    
    libfwbuilder::PolicyRule *last_mark_rule;
    
    IPTImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~IPTImporter();

    virtual void run();
    virtual void clear();

    void startSrcMultiPort();
    void pushTmpPortSpecToSrcPortList();

    void startDstMultiPort();
    void pushTmpPortSpecToDstPortList();

    void startBothMultiPort();
    void pushTmpPortSpecToBothPortList();
    
    void pushPolicyRule();
    void pushNATRule();

    virtual void addSrv();

    virtual void pushRule();

    virtual UnidirectionalRuleSet* getUnidirRuleSet(
        const std::string &rsname, const std::string &ruleset_type_name);

    virtual UnidirectionalRuleSet* checkUnidirRuleSet(
        const std::string &rsname);
    
    virtual void newUnidirRuleSet(const std::string &name, const std::string &ruleset_type);

    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();

    bool isStandardChain(const std::string &ipt_chain);

    void registerTable(const std::string &table_name);

    bool isSupportedTable(const std::string &table_name);
};

#endif
