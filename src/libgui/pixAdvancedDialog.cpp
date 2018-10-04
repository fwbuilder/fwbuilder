/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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



#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"

#include "pixAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "CompilerDriver_pix.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/XMLTools.h"

#include <memory>

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <qtabwidget.h>
#include <qlistwidget.h>
#include <qlabel.h>

#include <iostream>
#include <sstream>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


pixAdvancedDialog::pixAdvancedDialog(QWidget*parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pixAdvancedDialog_q;
    m_dialog->setupUi(static_cast<QDialog*>(this));
    obj = o;

    string version = obj->getStr("version");
    string vers = "version_" + obj->getStr("version");
    string platform = obj->getStr("platform");   // could be 'pix' or 'fwsm'

    QString      s;
    QStringList  logLevels;
    QStringList  logLevelMapping;

    // logLevelMapping.push_back("");
    // logLevelMapping.push_back("");

/* filling pop-down menu and pushing the same strings to the mapping
 * list at the same time so we could use translation
 */
    s=QObject::tr("0 - System Unusable");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("0");

    s=QObject::tr("1 - Take Immediate Action");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("1");

    s=QObject::tr("2 - Critical Condition");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("2");

    s=QObject::tr("3 - Error Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("3");

    s=QObject::tr("4 - Warning Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("4");

    s=QObject::tr("5 - Normal but significant condition");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("5");

    s=QObject::tr("6 - Informational");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("6");

    s=QObject::tr("7 - Debug Message");
    logLevels.push_back(s);
    logLevelMapping.push_back(s);
    logLevelMapping.push_back("7");

/* do not need to translate syslog facilities, but will use the same
 * method just in case */

    QStringList syslogFacilities;
    QStringList syslogFacilityMapping;
    syslogFacilities.push_back("");
    syslogFacilityMapping.push_back("");
    syslogFacilityMapping.push_back("");

    syslogFacilities.push_back("LOCAL0");
    syslogFacilityMapping.push_back("LOCAL0");
    syslogFacilityMapping.push_back("16");

    syslogFacilities.push_back("LOCAL1");
    syslogFacilityMapping.push_back("LOCAL1");
    syslogFacilityMapping.push_back("17");

    syslogFacilities.push_back("LOCAL2");
    syslogFacilityMapping.push_back("LOCAL2");
    syslogFacilityMapping.push_back("18");

    syslogFacilities.push_back("LOCAL3");
    syslogFacilityMapping.push_back("LOCAL3");
    syslogFacilityMapping.push_back("19");

    syslogFacilities.push_back("LOCAL4");
    syslogFacilityMapping.push_back("LOCAL4");
    syslogFacilityMapping.push_back("20");

    syslogFacilities.push_back("LOCAL5");
    syslogFacilityMapping.push_back("LOCAL5");
    syslogFacilityMapping.push_back("21");

    syslogFacilities.push_back("LOCAL6");
    syslogFacilityMapping.push_back("LOCAL6");
    syslogFacilityMapping.push_back("22");

    syslogFacilities.push_back("LOCAL7");
    syslogFacilityMapping.push_back("LOCAL7");
    syslogFacilityMapping.push_back("23");

    FWOptions *fwoptions = (Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);

/* Page Script */

    bool f1=fwoptions->getBool("pix_acl_basic");
    bool f2=fwoptions->getBool("pix_acl_no_clear");
    bool f3=fwoptions->getBool("pix_acl_substitution");
    bool f4=fwoptions->getBool("pix_add_clear_statements");

    /*
     * If none of the new pix_acl_* options is set and old pix_add_clear_statements
     * option is true, set pix_acl_basic to true.
     *
     * If old option pix_add_clear_statements iss false, set
     * pix_acl_no_clear to true
     */
    if (!f1 && !f2 && !f3)
    {
        if ( f4 ) fwoptions->setBool("pix_acl_basic",true);
        else fwoptions->setBool("pix_acl_no_clear",true);
    }

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);

    data.registerOption( m_dialog->short_script, fwoptions, "short_script");

/* Page "Compiler Options" */

    bool outboundACLSupported= (Resources::platform_res[platform]->getResourceBool(
                  "/FWBuilderResources/Target/options/" +
                  vers + "/pix_outbound_acl_supported"));

    if (outboundACLSupported)
        m_dialog->pix_emulate_out_acl->hide();
    else
        m_dialog->pix_generate_out_acl->hide();

    m_dialog->tabWidget->setTabEnabled(9,false); //Disable tab
    data.registerOption(m_dialog->ipv4before_2, fwoptions,
                        "ipv4_6_order",
                        QStringList() <<  tr("IPv4 before IPv6")
                        <<"ipv4_first" << tr("IPv6 before IPv4") << "ipv6_first");

    data.registerOption( m_dialog->outputFileName, fwoptions,
                         "output_file");

    data.registerOption( m_dialog->pix_assume_fw_part_of_any, fwoptions,
                         "pix_assume_fw_part_of_any");

    if (XMLTools::version_compare(version, "8.3") >= 0)
    {
        m_dialog->pix_replace_natted_objects->setChecked(false);
        m_dialog->pix_replace_natted_objects->setEnabled(false);
    } else
    {
        m_dialog->pix_replace_natted_objects->setEnabled(true);

        data.registerOption( m_dialog->pix_replace_natted_objects, fwoptions,
                             "pix_replace_natted_objects");
    }

    data.registerOption( m_dialog->pix_emulate_out_acl, fwoptions,
                         "pix_emulate_out_acl");

    data.registerOption( m_dialog->pix_generate_out_acl, fwoptions,
                         "pix_generate_out_acl");


    data.registerOption( m_dialog->pix_acl_basic, fwoptions,
                         "pix_acl_basic");

/*
    data.registerOption( m_dialog->pix_acl_alwaysNew, fwoptions,
                         "pix_acl_always_new");
*/

    data.registerOption( m_dialog->pix_acl_no_clear, fwoptions,
                         "pix_acl_no_clear");

    data.registerOption( m_dialog->pix_acl_substitution, fwoptions,
                         "pix_acl_substitution");

    data.registerOption( m_dialog->pix_acl_temp_addr, fwoptions,
                         "pix_acl_temp_addr");


    data.registerOption( m_dialog->pix_include_comments, fwoptions,
                         "pix_include_comments");

    data.registerOption( m_dialog->pix_use_acl_remarks, fwoptions,
                         "pix_use_acl_remarks");

    data.registerOption( m_dialog->pix_regroup_commands, fwoptions,
                         "pix_regroup_commands");

    data.registerOption( m_dialog->pix_use_manual_commit, fwoptions,
                         "pix_use_manual_commit");

    m_dialog->pix_use_manual_commit->setEnabled(platform=="fwsm");
/*
    data.registerOption( m_dialog->pix_add_clear_statements, fwoptions,
                         "pix_add_clear_statements");
*/

    data.registerOption( m_dialog->pix_optimize_default_nat, fwoptions,
                         "pix_optimize_default_nat");

    data.registerOption( m_dialog->pix_check_shadowing, fwoptions,
                         "check_shading");

    data.registerOption( m_dialog->pix_ignore_empty_groups, fwoptions,
                         "ignore_empty_groups");


    data.registerOption( m_dialog->pix_check_duplicate_nat, fwoptions,
                         "pix_check_duplicate_nat");

    data.registerOption( m_dialog->pix_check_overlapping_global_pools, fwoptions,
                         "pix_check_overlapping_global_pools");

    data.registerOption( m_dialog->pix_check_overlapping_statics, fwoptions,
                         "pix_check_overlapping_statics");

    data.registerOption( m_dialog->pix_check_overlapping_global_statics, fwoptions,
                         "pix_check_overlapping_global_statics");

    data.registerOption( m_dialog->mgmt_ssh, fwoptions,  "mgmt_ssh");
    data.registerOption( m_dialog->mgmt_addr, fwoptions,  "mgmt_addr");

/* page Installer */

    data.registerOption( m_dialog->user,fwoptions, "admUser");
    data.registerOption( m_dialog->altAddress,fwoptions, "altAddress");
    data.registerOption( m_dialog->sshArgs, fwoptions, "sshArgs");
    data.registerOption( m_dialog->scpArgs, fwoptions, "scpArgs");

    data.registerOption( m_dialog->use_scp, fwoptions, "use_scp");
    data.registerOption( m_dialog->filesystem, fwoptions, "filesystem");
    data.registerOption( m_dialog->filesystem, fwoptions, "firewall_dir");

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(     pis->getCommand().c_str());
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str());


/* page "Prolog/Epilog" */
    data.registerOption( m_dialog->pix_prolog_script, fwoptions,
                         "pix_prolog_script");

    data.registerOption( m_dialog->pix_epilog_script, fwoptions,
                         "pix_epilog_script");

/* page "Timeouts" */
    data.registerOption( m_dialog->xlate_hh, fwoptions, "xlate_hh");
    data.registerOption( m_dialog->xlate_mm, fwoptions, "xlate_mm");
    data.registerOption( m_dialog->xlate_ss, fwoptions, "xlate_ss");

    data.registerOption( m_dialog->conn_hh, fwoptions, "conn_hh");
    data.registerOption( m_dialog->conn_mm, fwoptions, "conn_mm");
    data.registerOption( m_dialog->conn_ss, fwoptions, "conn_ss");

    data.registerOption( m_dialog->udp_hh, fwoptions, "udp_hh");
    data.registerOption( m_dialog->udp_mm, fwoptions, "udp_mm");
    data.registerOption( m_dialog->udp_ss, fwoptions, "udp_ss");

    data.registerOption( m_dialog->rpc_hh, fwoptions, "rpc_hh");
    data.registerOption( m_dialog->rpc_mm, fwoptions, "rpc_mm");
    data.registerOption( m_dialog->rpc_ss, fwoptions, "rpc_ss");

    data.registerOption( m_dialog->h323_hh, fwoptions, "h323_hh");
    data.registerOption( m_dialog->h323_mm, fwoptions, "h323_mm");
    data.registerOption( m_dialog->h323_ss, fwoptions, "h323_ss");

    data.registerOption( m_dialog->sip_hh, fwoptions, "sip_hh");
    data.registerOption( m_dialog->sip_mm, fwoptions, "sip_mm");
    data.registerOption( m_dialog->sip_ss, fwoptions, "sip_ss");

    data.registerOption( m_dialog->sip_media_hh, fwoptions, "sip_media_hh");
    data.registerOption( m_dialog->sip_media_mm, fwoptions, "sip_media_mm");
    data.registerOption( m_dialog->sip_media_ss, fwoptions, "sip_media_ss");

    data.registerOption( m_dialog->half_closed_hh, fwoptions, "half-closed_hh");
    data.registerOption( m_dialog->half_closed_mm, fwoptions, "half-closed_mm");
    data.registerOption( m_dialog->half_closed_ss, fwoptions, "half-closed_ss");

    data.registerOption( m_dialog->uauth_hh, fwoptions, "uauth_hh");
    data.registerOption( m_dialog->uauth_mm, fwoptions, "uauth_mm");
    data.registerOption( m_dialog->uauth_ss, fwoptions, "uauth_ss");
    data.registerOption( m_dialog->uauth_abs, fwoptions, "uauth_abs");
    data.registerOption( m_dialog->uauth_inact, fwoptions, "uauth_inact");

    data.registerOption( m_dialog->telnet_timeout, fwoptions, "pix_telnet_timeout");
    data.registerOption( m_dialog->ssh_timeout, fwoptions, "pix_ssh_timeout");

/* page Fixups */

    allFixups.push_back(fixupControl(
                            m_dialog->pix_ctiqbe_switch,
                            m_dialog->pix_ctiqbe_port,
                            nullptr,
                            nullptr,
                            "ctiqbe_fixup", "ctiqbe", 0));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_dns_switch, 
                            m_dialog->pix_dns_max_length, 
                            nullptr, 
                            nullptr, 
                            "dns_fixup", "dns", 1));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_espike_switch, 
                            nullptr, 
                            nullptr, 
                            nullptr, 
                            "espike_fixup", "esp-ike", 2));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_ftp_switch, 
                            m_dialog->pix_ftp_port, 
                            nullptr, 
                            m_dialog->pix_ftp_strict, 
                            "ftp_fixup", "ftp", 3));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_h323h225_switch, 
                            m_dialog->pix_h323h225_port1, 
                            m_dialog->pix_h323h225_port2, 
                            nullptr, 
                            "h323_h225_fixup", "h323 h225", 4));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_h323ras_switch, 
                            m_dialog->pix_h323ras_port1, 
                            m_dialog->pix_h323ras_port2, 
                            nullptr, 
                            "h323_ras_fixup", "h323 ras", 5));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_http_switch, 
                            m_dialog->pix_http_port1, 
                            m_dialog->pix_http_port2, 
                            nullptr, 
                            "http_fixup", "http", 6));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_icmperror_switch, 
                            nullptr, 
                            nullptr, 
                            nullptr, 
                            "icmp_error_fixup", "icmp error", 7));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_ils_switch, 
                            m_dialog->pix_ils_port1, 
                            m_dialog->pix_ils_port2, 
                            nullptr, 
                            "ils_fixup", "ils", 8));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_mgcp_switch, 
                            m_dialog->pix_mgcp_gateway_port, 
                            m_dialog->pix_mgcp_call_agent_port, 
                            nullptr, 
                            "mgcp_fixup", "mgcp", 9));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_pptp_switch, 
                            m_dialog->pix_pptp_port, 
                            nullptr, 
                            nullptr, 
                            "pptp_fixup", "pptp", 10));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_rsh_switch, 
                            m_dialog->pix_rsh_port1, 
                            nullptr, 
                            nullptr, 
                            "rsh_fixup", "rsh", 11));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_rtsp_switch, 
                            m_dialog->pix_rtsp_port, 
                            nullptr, 
                            nullptr, 
                            "rtsp_fixup", "rtsp", 12));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_sip_switch, 
                            m_dialog->pix_sip_port1, 
                            m_dialog->pix_sip_port2, 
                            nullptr, 
                            "sip_fixup", "sip", 13));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_sipudp_switch, 
                            m_dialog->pix_sip_udp_port1, 
                            nullptr, 
                            nullptr, 
                            "sip_udp_fixup", "sip udp", 14));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_skinny_switch, 
                            m_dialog->pix_skinny_port1, 
                            m_dialog->pix_skinny_port2, 
                            nullptr, 
                            "skinny_fixup", "skinny", 15));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_smtp_switch, 
                            m_dialog->pix_smtp_port1, 
                            m_dialog->pix_smtp_port2, 
                            nullptr, 
                            "smtp_fixup", "smtp", 16));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_sqlnet_switch, 
                            m_dialog->pix_sqlnet_port1, 
                            m_dialog->pix_sqlnet_port2, 
                            nullptr, 
                            "sqlnet_fixup", "sqlnet", 17));
    allFixups.push_back(fixupControl(
                            m_dialog->pix_tftp_switch, 
                            m_dialog->pix_tftp_port, 
                            nullptr, 
                            nullptr, 
                            "tftp_fixup", "tftp", 18));

    allFixups.push_back(fixupControl(
                            m_dialog->pix_ip_options_eool_switch, 
                            nullptr,
                            nullptr, 
                            nullptr, 
                            "ip_options_eool_fixup", "IP options", 19));

    allFixups.push_back(fixupControl(
                            m_dialog->pix_ip_options_nop_switch, 
                            nullptr,
                            nullptr, 
                            nullptr, 
                            "ip_options_nop_fixup", "IP options", 20));

    allFixups.push_back(fixupControl(
                            m_dialog->pix_ip_options_rtralt_switch, 
                            nullptr,
                            nullptr, 
                            nullptr, 
                            "ip_options_rtralt_fixup", "IP options", 21));

    QStringList allowed_fixups = 
        QString(Resources::platform_res[platform]->getResourceStr(
                    "/FWBuilderResources/Target/options/" + vers +
                    "/fixups/list").c_str()).split(",");

    if (fwbdebug)
        qDebug() << "pixAdvancedDialog::pixAdvancedDialog allowed_fixups:"
                 << allowed_fixups;


    for (list<fixupControl>::iterator fi=allFixups.begin();
         fi!=allFixups.end(); fi++)
    {
        if (fwbdebug)
            qDebug() << "pixAdvancedDialog::pixAdvancedDialog fwopt:"
                     << fi->fwoption;

        if (fi->switch_widget!=nullptr)
            connect( fi->switch_widget, SIGNAL(activated(int)),
                     this, SLOT(fixupCmdChanged()));

        if (fi->arg1!=nullptr) connect( fi->arg1, SIGNAL(valueChanged(int)),
                                     this, SLOT(fixupCmdChanged()));

        if (fi->arg2!=nullptr) connect( fi->arg2, SIGNAL(valueChanged(int)),
                                     this, SLOT(fixupCmdChanged()));

        if (fi->arg3!=nullptr) connect( fi->arg3, SIGNAL(clicked()),
                                     this, SLOT(fixupCmdChanged()));

        bool active = allowed_fixups.contains(fi->fwoption);

        fi->active = active;
        m_dialog->fixup_notebook->setTabEnabled( fi->page, active);

    }

/* page Logging */

    m_dialog->emblem_log_format->setEnabled(
        Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/"+vers+"/pix_emblem_log_format"));


    syslogDeviceIdSupported=(Resources::platform_res[platform]->getResourceBool(
                                 "/FWBuilderResources/Target/options/"+
                                 vers+"/pix_syslog_device_id_supported"));

    m_dialog->syslog_device_id_hostname->setEnabled(syslogDeviceIdSupported);
    m_dialog->syslog_device_id_interface->setEnabled(syslogDeviceIdSupported);
    m_dialog->syslog_device_id_interface_val->setEnabled(syslogDeviceIdSupported);
    m_dialog->syslog_device_id_string->setEnabled(syslogDeviceIdSupported);
    m_dialog->syslog_device_id_string_val->setEnabled(syslogDeviceIdSupported);


    data.registerOption( m_dialog->syslog_host, fwoptions, "pix_syslog_host");
    data.registerOption( m_dialog->syslog_queue_size, fwoptions, "pix_syslog_queue_size");

    m_dialog->syslog_facility->clear();
    m_dialog->syslog_facility->addItems( syslogFacilities);
    data.registerOption( m_dialog->syslog_facility, fwoptions,
                         "pix_syslog_facility", syslogFacilityMapping);

    m_dialog->logging_trap_level->clear();
    m_dialog->logging_trap_level->addItems(logLevels);

    data.registerOption( m_dialog->logging_trap_level, fwoptions,
                         "pix_logging_trap_level", logLevelMapping);


    data.registerOption( m_dialog->emblem_log_format, fwoptions,
                         "pix_emblem_log_format");



    QStringList interfaces;
    list<FWObject*> l2=obj->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        interfaces.push_back( (Interface::cast(*i))->getLabel().c_str());

    m_dialog->syslog_device_id_interface_val->addItems(interfaces);

    if (syslogDeviceIdSupported)
    {
        string s=fwoptions->getStr("pix_syslog_device_id_opt");
        string v=fwoptions->getStr("pix_syslog_device_id_val");
        if (s=="hostname")  m_dialog->syslog_device_id_hostname->setChecked(true);
        if (s=="interface")
        {
            m_dialog->syslog_device_id_interface->setChecked(true);
            m_dialog->syslog_device_id_interface_val->setCurrentIndex(
                    m_dialog->syslog_device_id_interface_val->findText(v.c_str()));
        }
        if (s=="string")
        {
            m_dialog->syslog_device_id_string->setChecked(true);
            m_dialog->syslog_device_id_string_val->setText(v.c_str());
        }
    }

    data.registerOption( m_dialog->logging_timestamp, fwoptions,
                         "pix_logging_timestamp");

    data.registerOption( m_dialog->logging_buffered, fwoptions,
                         "pix_logging_buffered");

    m_dialog->logging_buffered_level->clear();
    m_dialog->logging_buffered_level->addItems(logLevels);
    data.registerOption( m_dialog->logging_buffered_level, fwoptions,
                         "pix_logging_buffered_level", logLevelMapping);

    data.registerOption( m_dialog->logging_console, fwoptions,
                         "pix_logging_console");

    m_dialog->logging_console_level->clear();
    m_dialog->logging_console_level->addItems(logLevels);
    data.registerOption( m_dialog->logging_console_level,fwoptions,
                         "pix_logging_console_level", logLevelMapping);


/* page Options */

    m_dialog->fragguard->setEnabled(
        Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/" + vers +
            "/pix_security_fragguard_supported"));

    m_dialog->route_dnat->setEnabled(
        Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/" + vers +
            "/pix_route_dnat_supported"));

    data.registerOption( m_dialog->fragguard, fwoptions, "pix_fragguard");
    data.registerOption( m_dialog->route_dnat, fwoptions, "pix_route_dnat");

    data.registerOption( m_dialog->resetinbound, fwoptions, "pix_resetinbound");
    data.registerOption( m_dialog->resetoutside, fwoptions, "pix_resetoutside");

    data.registerOption( m_dialog->connection_timewait, fwoptions,
                         "pix_connection_timewait");
    data.registerOption( m_dialog->floodguard, fwoptions, "pix_floodguard");
    data.registerOption( m_dialog->nodnsalias_inbound, fwoptions,
                         "pix_nodnsalias_inbound");
    data.registerOption( m_dialog->nodnsalias_outbound, fwoptions,
                         "pix_nodnsalias_outbound");

    data.registerOption( m_dialog->max_conns, fwoptions, "pix_max_conns");
    data.registerOption( m_dialog->emb_limit, fwoptions, "pix_emb_limit");

    data.loadAll();
    loadFixups();
    updateFixupCommandsDisplay();
    scriptACLModeChanged();

    m_dialog->tabWidget->setCurrentIndex(0);
}

pixAdvancedDialog::~pixAdvancedDialog()
{
    delete m_dialog;
}

/*
 * items in the switch_widget (QComboBox) | values in FirewallOptions object
 *                                        |
 * Skip (item 0)                          |  2
 * Enable (item 1)                        |  0
 * Disable (item 2)                       |  1
 *
 * this strange mapping is historical.
 *
 * ip options switch has the following items:
 *
 * skip
 * allow
 * drop
 * clear
 *
 * The last item is just added at the bottom and is mapped to FirewallOptions
 * value "3"
 * 
 */
static int fixupOpt2Widget[] = { 1, 2, 0, 3 };
static int fixupWidget2Opt[] = { 2, 0, 1, 3 };

int pixAdvancedDialog::translateFixupSwitchFromOptionToWidget(int o)
{
    return fixupOpt2Widget[o];
}

int pixAdvancedDialog::translateFixupSwitchFromWidgetToOption(int w)
{
    return fixupWidget2Opt[w];
}

void pixAdvancedDialog::enableAllFixups()  { changeAllFixups(0); }
void pixAdvancedDialog::disableAllFixups() { changeAllFixups(1); }
void pixAdvancedDialog::skipAllFixups()    { changeAllFixups(2); }

void pixAdvancedDialog::changeAllFixups(int state)
{
    for (list<fixupControl>::iterator fi=allFixups.begin(); fi!=allFixups.end(); fi++)
    {
        if (!fi->active) continue;

        fi->switch_widget->setCurrentIndex(
            translateFixupSwitchFromOptionToWidget(state) );
    }
    updateFixupCommandsDisplay();
}

void pixAdvancedDialog::loadFixups()
{
    FWOptions *options=(Firewall::cast(obj))->getOptionsObject();
    assert(options!=nullptr);

    for (list<fixupControl>::iterator fi=allFixups.begin(); fi!=allFixups.end(); fi++)
    {
        if (!fi->active) continue;
        string f = options->getStr(fi->fwoption.toLatin1().constData());
        if (!f.empty())
        {
// "0" means "fixup" or "enable" in a pop-down menu (historical)
// "1" means "no fixup" or "disable" in a pop-down menu (historical)
            int    sw;

            int    p1,p2;   // two port numbers
            string arg3n;   // option name
            bool   arg3v;   // option state (on/off)

            istringstream str(f);

            str >> sw >> p1 >> p2 >> arg3n >> arg3v;

            if (arg3n=="nil")    arg3n="";

            fi->switch_widget->setCurrentIndex(
                translateFixupSwitchFromOptionToWidget(sw) );

/* if values are 0 in the data file, we stick with defaults. Defaults
 * are preconfigured in the GUI
 */
            if (fi->arg1 && p1!=0) fi->arg1->setValue(p1);
            if (fi->arg2 && p2!=0) fi->arg2->setValue(p2);
            if (fi->arg3) fi->arg3->setChecked(arg3v);

        } else
        {
            fi->switch_widget->setCurrentIndex(0);
        }
    }
}

void pixAdvancedDialog::saveFixups(FWOptions *options)
{
    for (list<fixupControl>::iterator fi=allFixups.begin(); fi!=allFixups.end(); fi++)
    {
        string name = fi->fwoption.toLatin1().constData();
        int sw = translateFixupSwitchFromWidgetToOption(
            fi->switch_widget->currentIndex());

        int    p1  =(fi->arg1)?fi->arg1->value():0;
        int    p2  =(fi->arg2)?fi->arg2->value():0;
        string on  =(name=="ftp_fixup")?"strict":"nil";
        bool   ov  =(fi->arg3)?fi->arg3->isChecked():false;

        if (!fi->active) sw=2;

        ostringstream str;
        str << sw << " " << p1 << " " << p2 << " " << on << " " << int(ov);

        options->setStr( fi->fwoption.toLatin1().constData(), str.str() );

        if (fwbdebug)
            qDebug() << "pixAdvancedDialog::saveFixups()"
                     << name.c_str()
                     << str.str().c_str();
    }
}

void pixAdvancedDialog::displayCommands()
{
    m_dialog->pix_generated_fixup->setText("");

/*
 * need to copy information from widgets that control fixups into
 * firewall object's options, so that when we dump the database into
 * memory buffer, we get updated info
 *
 * This creates a problem however: since we save changes into the
 * actual object here, the undo/redo commands don't work later on in
 * accept() because we do not detect any changes and undo command is
 * not placed on undo stack. Need to save FWOptions object, save fixup
 * parameters into it, generate commands and then restore FWOptions
 * object back
 */
    FWOptions *options = (Firewall::cast(obj))->getOptionsObject();
    assert(options!=nullptr);

    FWOptions *backup_options = new FWOptions();
    backup_options->duplicate(options, false);

    saveFixups(options);

    CompilerDriver_pix driver(obj->getRoot());
    driver.setTargetId(FWObjectDatabase::getStringId(obj->getId()));
    string inspectors = driver.protocolInspectorCommands();
    m_dialog->pix_generated_fixup->setText(inspectors.c_str());

    options->duplicate(backup_options, false);
    delete backup_options;
}

void pixAdvancedDialog::updateFixupCommandsDisplay()
{
    displayCommands();
}

void pixAdvancedDialog::fixupCmdChanged()
{
    updateFixupCommandsDisplay();
}

/*
 * store all data in the object
 */
void pixAdvancedDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt = (Firewall::cast(new_state))->getManagementObject();
    assert(mgmt!=nullptr);

    data.saveAll(fwoptions);

    saveFixups(fwoptions);

    // find first interface marked as "management"
    const InetAddr *mgmt_addr = Firewall::cast(new_state)->getManagementAddress();
    if (mgmt_addr)
        mgmt->setAddress(*mgmt_addr);

    if (syslogDeviceIdSupported)
    {
        QString s,v;
        s="";
        v="";
        if (m_dialog->syslog_device_id_hostname->isChecked())  s="hostname";
        if (m_dialog->syslog_device_id_interface->isChecked())
        {
            s="interface";
            v=m_dialog->syslog_device_id_interface_val->currentText();
        }
        if (m_dialog->syslog_device_id_string->isChecked())
        {
            s="string";
            v=m_dialog->syslog_device_id_string_val->text();
        }

        fwoptions->setStr("pix_syslog_device_id_opt",s.toLatin1().constData());
        fwoptions->setStr("pix_syslog_device_id_val",v.toLatin1().constData());
    }

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();
    pis->setCommand( m_dialog->installScript->text().toLatin1().constData() );
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData() );

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void pixAdvancedDialog::reject()
{
    QDialog::reject();
}

void pixAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->pix_prolog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->pix_prolog_script->setText( edt.text() );
}

void pixAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->pix_epilog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->pix_epilog_script->setText( edt.text() );
}

void pixAdvancedDialog::setDefaultTimeoutValue(const QString &option)
{
    string platform = obj->getStr("platform");   // could be 'pix' or 'fwsm'
#ifndef NDEBUG
    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);
#endif
    string vers="version_"+obj->getStr("version");

    if (option=="uauth_abs" || option=="uauth_inact")
        data.setWidgetValue(option.toLatin1().constData(),
                        Resources::platform_res[platform]->getResourceBool(
     "/FWBuilderResources/Target/options/"+vers+"/timeouts/"+option.toLatin1().constData()));
    else
        data.setWidgetValue(option.toLatin1().constData(),
                        Resources::platform_res[platform]->getResourceInt(
     "/FWBuilderResources/Target/options/"+vers+"/timeouts/"+option.toLatin1().constData()));
}

void pixAdvancedDialog::defaultTimeouts()
{
#ifndef NDEBUG
    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);
#endif
    string vers="version_"+obj->getStr("version");

    setDefaultTimeoutValue("xlate_hh"       );
    setDefaultTimeoutValue("xlate_mm"       );
    setDefaultTimeoutValue("xlate_ss"       );
    setDefaultTimeoutValue("conn_hh"        );
    setDefaultTimeoutValue("conn_mm"        );
    setDefaultTimeoutValue("conn_ss"        );
    setDefaultTimeoutValue("udp_hh"         );
    setDefaultTimeoutValue("udp_mm"         );
    setDefaultTimeoutValue("udp_ss"         );
    setDefaultTimeoutValue("rpc_hh"         );
    setDefaultTimeoutValue("rpc_mm"         );
    setDefaultTimeoutValue("rpc_ss"         );
    setDefaultTimeoutValue("h323_hh"        );
    setDefaultTimeoutValue("h323_mm"        );
    setDefaultTimeoutValue("h323_ss"        );
    setDefaultTimeoutValue("sip_hh"         );
    setDefaultTimeoutValue("sip_mm"         );
    setDefaultTimeoutValue("sip_ss"         );
    setDefaultTimeoutValue("sip_media_hh"   );
    setDefaultTimeoutValue("sip_media_mm"   );
    setDefaultTimeoutValue("sip_media_ss"   );
    setDefaultTimeoutValue("half-closed_hh" );
    setDefaultTimeoutValue("half-closed_mm" );
    setDefaultTimeoutValue("half-closed_ss" );
    setDefaultTimeoutValue("uauth_hh"       );
    setDefaultTimeoutValue("uauth_mm"       );
    setDefaultTimeoutValue("uauth_abs"      );
    setDefaultTimeoutValue("uauth_inact"    );

    setDefaultTimeoutValue("telnet_timeout" );
    setDefaultTimeoutValue("ssh_timeout"    );
}

void pixAdvancedDialog::regenerateFixups()
{
}

void pixAdvancedDialog::scriptACLModeChanged()
{
    m_dialog->pix_acl_temp_lbl->setEnabled(m_dialog->pix_acl_substitution->isChecked());
    m_dialog->pix_acl_temp_addr->setEnabled(m_dialog->pix_acl_substitution->isChecked());
}



