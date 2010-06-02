#-*- mode: makefile; tab-width: 4; -*-
#
FWBUILDER_SRC_DIR = ../../..
FWBUILDER_GUI_DIR = $$FWBUILDER_SRC_DIR/gui/

include($$FWBUILDER_SRC_DIR/../qmake.inc)

QT += gui network 
TEMPLATE = lib
SOURCES += ../../ProjectPanel.cpp \
    ../../ProjectPanel_events.cpp \
    ../../ProjectPanel_file_ops.cpp \
    ../../ProjectPanel_state_ops.cpp \
    ../../FWWindow.cpp \
    ../../FWWindow_editor.cpp \
    ../../FWWindow_single_rule_compile.cpp \
    ../../FWWindow_wrappers.cpp \
    ../../FWWindowPrint.cpp \
    ../../TextEditWidget.cpp \
    ../../ObjectEditorDockWidget.cpp \
    ../../ssh_wrappers.cpp \
    ../../utils.cpp \
    ../../utils_no_qt.cpp \
    ../../Importer.cpp \
    ../../IOSImporter.cpp \
    ../../IOSImporterRun.cpp \
    ../../IPTImporter.cpp \
    ../../IPTImporterRun.cpp \
    ../../SSHSession.cpp \
    ../../SSHUnx.cpp \
    ../../SSHCisco.cpp \
    ../../SSHPIX.cpp \
    ../../SSHIOS.cpp \
    ../../debugDialog.cpp \
    ../../findDialog.cpp \
    ../../longTextDialog.cpp \
    ../../newGroupDialog.cpp \
    ../../filePropDialog.cpp \
    ../../IconSetter.cpp \
    ../../DialogData.cpp \
    ../../SimpleTextEditor.cpp \
    ../../SimpleIntEditor.cpp \
    ../../FWBSettings.cpp \
    ../../FWBTree.cpp \
    ../../RCS.cpp \
    ../../RCSFilePreview.cpp \
    ../../FWObjectClipboard.cpp \
    ../../platforms.cpp \
    ../../printerStream.cpp \
    ../../PrintingProgressDialog.cpp \
    ../../PrintingController.cpp \
    ../../FWObjectPropertiesFactory.cpp \
    ../../ObjectManipulator.cpp \
    ../../ObjectManipulator_ops.cpp \
    ../../ObjectManipulator_tree_ops.cpp \
    ../../ObjectManipulator_create_new.cpp \
    ../../ObjectManipulator_slots.cpp \
    ../../ObjectEditor.cpp \
    ../../FWObjectDrag.cpp \
    ../../ObjectTreeView.cpp \
    ../../ObjectListView.cpp \
    ../../ObjectIconView.cpp \
    ../../DialogFactory.cpp \
    ../../HostDialog.cpp \
    ../../FirewallDialog.cpp \
    ../../ClusterDialog.cpp \
    ../../ClusterGroupDialog.cpp \
    ../../InterfaceDialog.cpp \
    ../../AddressRangeDialog.cpp \
    ../../AddressTableDialog.cpp \
    ../../IPv4Dialog.cpp \
    ../../IPv6Dialog.cpp \
    ../../PhysicalAddressDialog.cpp \
    ../../NetworkDialog.cpp \
    ../../NetworkDialogIPv6.cpp \
    ../../UserDialog.cpp \
    ../../LibraryDialog.cpp \
    ../../CustomServiceDialog.cpp \
    ../../IPServiceDialog.cpp \
    ../../ICMPServiceDialog.cpp \
    ../../TCPServiceDialog.cpp \
    ../../UDPServiceDialog.cpp \
    ../../GroupObjectDialog.cpp \
    ../../TimeDialog.cpp \
    ../../RuleSetDialog.cpp \
    ../../FWObjectSelectionModel.cpp \
    ../../ColDesc.cpp \
    ../../RuleNode.cpp \
    ../../RuleSetModel.cpp \
    ../../RuleSetView.cpp \
    ../../RuleSetViewDelegate.cpp \
    ../../iptAdvancedDialog.cpp \
    ../../ipcopAdvancedDialog.cpp \
    ../../ipfAdvancedDialog.cpp \
    ../../ipfwAdvancedDialog.cpp \
    ../../pfAdvancedDialog.cpp \
    ../../pixAdvancedDialog.cpp \
    ../../pixosAdvancedDialog.cpp \
    ../../iosaclAdvancedDialog.cpp \
    ../../iosAdvancedDialog.cpp \
    ../../ipcoposAdvancedDialog.cpp \
    ../../linux24AdvancedDialog.cpp \
    ../../linksysAdvancedDialog.cpp \
    ../../freebsdAdvancedDialog.cpp \
    ../../openbsdAdvancedDialog.cpp \
    ../../solarisAdvancedDialog.cpp \
    ../../macosxAdvancedDialog.cpp \
    ../../secuwallAdvancedDialog.cpp \
    ../../secuwallosAdvancedDialog.cpp \
    ../../secuwallIfaceOptsDialog.cpp \
    ../../procurveaclAdvancedDialog.cpp \
    ../../vlanOnlyIfaceOptsDialog.cpp \
    ../../linux24IfaceOptsDialog.cpp \
    ../../pixosIfaceOptsDialog.cpp \
    ../../openbsdIfaceOptsDialog.cpp \
    ../../clusterMembersDialog.cpp \
    ../../CompilerOutputPanel.cpp \
    ../../CompilerDriverFactory.cpp \
    ../../RuleOptionsDialog.cpp \
    ../../RoutingRuleOptionsDialog.cpp \
    ../../NATRuleOptionsDialog.cpp \
    ../../LibExportDialog.cpp \
    ../../PrefsDialog.cpp \
    ../../instConf.cpp \
    ../../instDialog.cpp \
    ../../instDialog_ui_ops.cpp \
    ../../instDialog_compile.cpp \
    ../../instDialog_installer.cpp \
    ../../FirewallInstaller.cpp \
    ../../FirewallInstallerCisco.cpp \
    ../../FirewallInstallerUnx.cpp \
    ../../newFirewallDialog.cpp \
    ../../newFirewallDialog_from_template.cpp \
    ../../newClusterDialog.cpp \
    ../../newClusterDialog_create.cpp \
    ../../newHostDialog.cpp \
    ../../ObjConflictResolutionDialog.cpp \
    ../../ColorLabelMenuItem.cpp \
    ../../TagServiceDialog.cpp \
    ../../ActionsDialog.cpp \
    ../../SimpleTextView.cpp \
    ../../DiscoveryDruid.cpp \
    ../../BlankDialog.cpp \
    ../../DNSNameDialog.cpp \
    ../../ObjectTreeViewItem.cpp \
    ../../InstallFirewallViewItem.cpp \
    ../../instOptionsDialog.cpp \
    ../../instBatchOptionsDialog.cpp \
    ../../FilterDialog.cpp \
    ../../FindObjectWidget.cpp \
    ../../FWObjectDropArea.cpp \
    ../../CommentEditorPanel.cpp \
    ../../MetricEditorPanel.cpp \
    ../../FindWhereUsedWidget.cpp \
    ../../ConfirmDeleteObjectDialog.cpp \
    ../../FakeWizard.cpp \
    ../../AskLibForCopyDialog.cpp \
    ../../ObjectListViewItem.cpp \
    ../../Help.cpp \
    ../../HttpGet.cpp \
    ../../StartTipDialog.cpp \
    ../../FWBAboutDialog.cpp \
    ../../vrrpOptionsDialog.cpp \
    ../../carpOptionsDialog.cpp \
    ../../pixFailoverOptionsDialog.cpp \
    ../../conntrackOptionsDialog.cpp \
    ../../pfsyncOptionsDialog.cpp \
    ../../heartbeatOptionsDialog.cpp \
    ../../openaisOptionsDialog.cpp \
    ../../AutocompletedComboBox.cpp \
    ../../InterfaceEditorWidget.cpp \
    ../../FWCmdBasic.cpp \
    ../../FWCmdChange.cpp \
    ../../FWCmdAddObject.cpp \
    ../../FWCmdDeleteObject.cpp \
    ../../FWCmdMoveObject.cpp \
    ../../InterfacesTabWidget.cpp \
    ../../FirewallSelectorWidget.cpp \
    ../../ClusterInterfacesSelectorWidget.cpp \
    ../../ClusterInterfaceWidget.cpp \
    ../../FWCmdRule.cpp \
    ../../UsageResolver.cpp \
    ../../TutorialDialog.cpp \
    ../../MDIEventFilter.cpp \
    ../../UserWorkflow.cpp \
    ../../FWBApplication.cpp

HEADERS += ../../../../config.h \
    ../../../../build_num \
    ../../events.h \
    ../../FWWindow.h \
    ../../ProjectPanel.h \
    ../../BaseObjectDialog.h \
    ../../TextEditWidget.h \
    ../../utils.h \
    ../../utils_no_qt.h \
    ../../Importer.h \
    ../../IOSImporter.h \
    ../../IPTImporter.h \
    ../../SSHSession.h \
    ../../SSHUnx.h \
    ../../SSHCisco.h \
    ../../SSHPIX.h \
    ../../SSHIOS.h \
    ../../debugDialog.h \
    ../../findDialog.h \
    ../../longTextDialog.h \
    ../../newGroupDialog.h \
    ../../filePropDialog.h \
    ../../IconSetter.h \
    ../../DialogData.h \
    ../../SimpleTextEditor.h \
    ../../SimpleIntEditor.h \
    ../../FWBSettings.h \
    ../../FWBTree.h \
    ../../RCS.h \
    ../../RCSFilePreview.h \
    ../../FWObjectClipboard.h \
    ../../platforms.h \
    ../../global.h \
    ../../printerStream.h \
    ../../PrintingProgressDialog.h \
    ../../PrintingController.h \
    ../../FWObjectPropertiesFactory.h \
    ../../ObjectManipulator.h \
    ../../ObjectEditor.h \
    ../../ObjectEditorDockWidget.h \
    ../../FWObjectDrag.h \
    ../../ObjectTreeView.h \
    ../../ObjectListView.h \
    ../../ObjectIconView.h \
    ../../ObjectTreeViewItem.h \
    ../../InstallFirewallViewItem.h \
    ../../DialogFactory.h \
    ../../HostDialog.h \
    ../../FirewallDialog.h \
    ../../ClusterDialog.h \
    ../../ClusterGroupDialog.h \
    ../../InterfaceDialog.h \
    ../../AddressRangeDialog.h \
    ../../AddressTableDialog.h \
    ../../IPv4Dialog.h \
    ../../IPv6Dialog.h \
    ../../PhysicalAddressDialog.h \
    ../../NetworkDialog.h \
    ../../NetworkDialogIPv6.h \
    ../../UserDialog.h \
    ../../RuleSetDialog.h \
    ../../LibraryDialog.h \
    ../../CustomServiceDialog.h \
    ../../IPServiceDialog.h \
    ../../ICMPServiceDialog.h \
    ../../TCPServiceDialog.h \
    ../../UDPServiceDialog.h \
    ../../GroupObjectDialog.h \
    ../../ObjectIconViewItem.h \
    ../../TimeDialog.h \
    ../../ColDesc.h \
    ../../FWObjectSelectionModel.h \
    ../../RuleNode.h \
    ../../RuleSetModel.h \
    ../../RuleSetView.h \
    ../../RuleSetViewDelegate.h \
    ../../iptAdvancedDialog.h \
    ../../ipcopAdvancedDialog.h \
    ../../ipfAdvancedDialog.h \
    ../../ipfwAdvancedDialog.h \
    ../../pfAdvancedDialog.h \
    ../../pixAdvancedDialog.h \
    ../../pixosAdvancedDialog.h \
    ../../iosaclAdvancedDialog.h \
    ../../iosAdvancedDialog.h \
    ../../ipcoposAdvancedDialog.h \
    ../../linux24AdvancedDialog.h \
    ../../linksysAdvancedDialog.h \
    ../../freebsdAdvancedDialog.h \
    ../../openbsdAdvancedDialog.h \
    ../../solarisAdvancedDialog.h \
    ../../macosxAdvancedDialog.h \
    ../../secuwallAdvancedDialog.h \
    ../../secuwallosAdvancedDialog.h \
    ../../secuwallIfaceOptsDialog.h \
    ../../procurveaclAdvancedDialog.h \
    ../../vlanOnlyIfaceOptsDialog.h \
    ../../linux24IfaceOptsDialog.h \
    ../../pixosIfaceOptsDialog.h \
    ../../openbsdIfaceOptsDialog.h \
    ../../clusterMembersDialog.h \
    ../../CompilerOutputPanel.h \
    ../../CompilerDriverFactory.h \
    ../../RuleOptionsDialog.h \
    ../../RoutingRuleOptionsDialog.h \
    ../../NATRuleOptionsDialog.h \
    ../../LibExportDialog.h \
    ../../PrefsDialog.h \
    ../../instConf.h \
    ../../instDialog.h \
    ../../FirewallInstaller.h \
    ../../FirewallInstallerCisco.h \
    ../../FirewallInstallerUnx.h \
    ../../newFirewallDialog.h \
    ../../newClusterDialog.h \
    ../../newHostDialog.h \
    ../../ObjConflictResolutionDialog.h \
    ../../ColorLabelMenuItem.h \
    ../../TagServiceDialog.h \
    ../../ActionsDialog.h \
    ../../SimpleTextView.h \
    ../../DiscoveryDruid.h \
    ../../BlankDialog.h \
    ../../DNSNameDialog.h \
    ../../instOptionsDialog.h \
    ../../instBatchOptionsDialog.h \
    ../../FilterDialog.h \
    ../../FindObjectWidget.h \
    ../../FWObjectDropArea.h \
    ../../CommentEditorPanel.h \
    ../../MetricEditorPanel.h \
    ../../FindWhereUsedWidget.h \
    ../../ConfirmDeleteObjectDialog.h \
    ../../FakeWizard.h \
    ../../AskLibForCopyDialog.h \
    ../../FWBAboutDialog.h \
    ../../Help.h \
    ../../HttpGet.h \
    ../../StartTipDialog.h \
    ../../vrrpOptionsDialog.h \
    ../../carpOptionsDialog.h \
    ../../pixFailoverOptionsDialog.h \
    ../../conntrackOptionsDialog.h \
    ../../heartbeatOptionsDialog.h \
    ../../openaisOptionsDialog.h \
    ../../pfsyncOptionsDialog.h \
    ../../check_update_url.h \
    ../../startup_tip_url.h \
    ../../AutocompletedComboBox.h \
    ../../InterfaceEditorWidget.h \
    ../../FWCmdBasic.h \
    ../../FWCmdChange.h \
    ../../FWCmdAddObject.h \
    ../../FWCmdDeleteObject.h \
    ../../FWCmdMoveObject.h \
    ../../InterfacesTabWidget.h \
    ../../FirewallSelectorWidget.h \
    ../../ClusterInterfacesSelectorWidget.h \
    ../../ClusterInterfaceWidget.h \
    ../../FWCmdRule.h \
    ../../UsageResolver.h \
    ../../TutorialDialog.h \
    ../../MDIEventFilter.h \
    ../../UserWorkflow.h \
    ../../FWBApplication.h


INCLUDEPATH += ../../.ui
RESOURCES += ../../MainRes.qrc
OBJECTS_DIR = ../../.obj
MOC_DIR = ../../.moc

INCLUDEPATH += $$FWBUILDER_GUI_DIR/
INCLUDEPATH += $$FWBUILDER_GUI_DIR/.ui
INCLUDEPATH += $$FWBUILDER_SRC_DIR/compiler_lib
INCLUDEPATH += $$FWBUILDER_SRC_DIR/iptlib
INCLUDEPATH += $$FWBUILDER_SRC_DIR/.. \
               $$FWBUILDER_SRC_DIR/cisco_lib \
               $$FWBUILDER_SRC_DIR/pflib \
               $$FWBUILDER_SRC_DIR

DEPENDPATH = $$FWBUILDER_SRC_DIR/common

INCLUDEPATH += $$ANTLR_INCLUDEPATH
LIBS += $$FWBUILDER_GUI_DIR/$$FWBPARSER_LIB $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES


# fwtransfer lib. Add this before adding -lQtDBus to LIBS below
LIBS += $$FWBUILDER_GUI_DIR/$$FWTRANSFER_LIB
contains( HAVE_QTDBUS, 1 ):unix {
    !macx:QT += network \
        dbus
    macx:LIBS += -framework \
        QtDBus
}

LIBS += $$LIBS_FWCOMPILER

CONFIG += staticlib

TARGET = guilib

INSTALLS -= target
