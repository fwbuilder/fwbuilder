#-*- mode: makefile; tab-width: 4; -*-
#
# $Id$
#
TEMPLATE = app
LANGUAGE = C++
QT += network

TARGET = fwbuilder

include(../../qmake.inc)

exists(qmake.inc){
    include( qmake.inc)
}

# This makes gcc compile this header file and store result in
# .obj/fwbuilder.gch/c++. Generated Makefile will also add
# "-include .obj/fwbuilder " to the command line for the compiler
# for every module. There is no need to #include file fwbuilder_ph.h
# in each module since it will be included by means of -include option.
#
# PRECOMPILED_HEADER = fwbuilder_ph.h

# control whether we build debug or release in ../../qmake.inc
# CONFIG += precompile_header

HEADERS += ../../config.h \
    FWWindow.h \
    ProjectPanel.h \
	utils.h \
	utils_no_qt.h \
    Importer.h \
    IOSImporter.h \
	IPTImporter.h \
	SSHSession.h \
	SSHUnx.h \
	SSHPIX.h \
	SSHIOS.h \
	debugDialog.h \
	findDialog.h \
	longTextDialog.h \
	newGroupDialog.h \
	filePropDialog.h \
	DialogData.h \
	SimpleTextEditor.h \
	SimpleIntEditor.h \
	FWBSettings.h \
	FWBTree.h \
	RCS.h \
	RCSFilePreview.h \
	FWObjectClipboard.h \
	platforms.h \
	global.h \
	printerStream.h \
	PrintingProgressDialog.h \
	PrintingController.h \
	FWObjectPropertiesFactory.h \
	ObjectManipulator.h \
	ObjectEditor.h \
	FWObjectDrag.h \
	ObjectTreeView.h \
	ObjectListView.h \
	ObjectIconView.h \
	ObjectTreeViewItem.h \
	InstallFirewallViewItem.h \
	DialogFactory.h \
	HostDialog.h \
	FirewallDialog.h \
	InterfaceDialog.h \
	AddressRangeDialog.h \
	AddressTableDialog.h \
	IPv4Dialog.h \
	IPv6Dialog.h \
	PhysicalAddressDialog.h \
	NetworkDialog.h \
	NetworkDialogIPv6.h \
	UserDialog.h \
	RuleSetDialog.h \
	LibraryDialog.h \
	CustomServiceDialog.h \
	IPServiceDialog.h \
	ICMPServiceDialog.h \
	TCPServiceDialog.h \
	UDPServiceDialog.h \
	GroupObjectDialog.h \
	ObjectIconViewItem.h \
	TimeDialog.h \
	RuleSetView.h \
	iptAdvancedDialog.h \
	ipfAdvancedDialog.h \
	ipfwAdvancedDialog.h \
	pfAdvancedDialog.h \
	pixAdvancedDialog.h \
	pixosAdvancedDialog.h \
	iosaclAdvancedDialog.h \
	iosAdvancedDialog.h \
	linux24AdvancedDialog.h \
	linksysAdvancedDialog.h \
	freebsdAdvancedDialog.h \
	openbsdAdvancedDialog.h \
	solarisAdvancedDialog.h \
	macosxAdvancedDialog.h \
	RuleOptionsDialog.h \
	RoutingRuleOptionsDialog.h \
	NATRuleOptionsDialog.h \
	LibExportDialog.h \
	PrefsDialog.h \
	instConf.h \
	instDialog.h \
	FirewallInstaller.h \
	FirewallInstallerCisco.h \
	FirewallInstallerUnx.h \
	newFirewallDialog.h \
	newHostDialog.h \
	ObjConflictResolutionDialog.h \
	listOfLibraries.h \
	ColorLabelMenuItem.h \
	TagServiceDialog.h \
	ActionsDialog.h \
	SimpleTextView.h \
	DiscoveryDruid.h \
	DNSNameDialog.h \
	instOptionsDialog.h \
	instBatchOptionsDialog.h \
	FilterDialog.h \
	FindObjectWidget.h \
	FWObjectDropArea.h \
	CommentEditorPanel.h \
	MetricEditorPanel.h \
	FindWhereUsedWidget.h \
	ConfirmDeleteObjectDialog.h \
	FakeWizard.h \
	AskLibForCopyDialog.h \
	FWBAboutDialog.h \
	RuleRowInfo.h \
	Help.h \
	HttpGet.h \
	StartTipDialog.h \
	check_update_url.h

SOURCES += ProjectPanel.cpp \
	ProjectPanel_file_ops.cpp \
	ProjectPanel_state_ops.cpp \
    FWWindow.cpp \
    main.cpp \
	utils.cpp \
	utils_no_qt.cpp \
    Importer.cpp \
    IOSImporter.cpp \
    IOSImporterRun.cpp \
	IPTImporter.cpp \
	IPTImporterRun.cpp \
	SSHSession.cpp \
	SSHUnx.cpp \
	SSHPIX.cpp \
	SSHIOS.cpp \
	debugDialog.cpp \
	findDialog.cpp \
	longTextDialog.cpp \
	newGroupDialog.cpp \
	filePropDialog.cpp \
	DialogData.cpp \
	SimpleTextEditor.cpp \
	SimpleIntEditor.cpp \
	FWBSettings.cpp \
	FWBTree.cpp \
	RCS.cpp \
	RCSFilePreview.cpp \
	FWObjectClipboard.cpp \
	platforms.cpp \
	FWWindowPrint.cpp \
	printerStream.cpp \
	PrintingProgressDialog.cpp \
	PrintingController.cpp \
	FWObjectPropertiesFactory.cpp \
	ObjectManipulator.cpp \
	ObjectEditor.cpp \
	FWObjectDrag.cpp \
	ObjectTreeView.cpp \
	ObjectListView.cpp \
	ObjectIconView.cpp \
	DialogFactory.cpp \
	HostDialog.cpp \
	FirewallDialog.cpp \
	InterfaceDialog.cpp \
	AddressRangeDialog.cpp \
	AddressTableDialog.cpp \
	IPv4Dialog.cpp \
	IPv6Dialog.cpp \
	PhysicalAddressDialog.cpp \
	NetworkDialog.cpp \
	NetworkDialogIPv6.cpp \
	UserDialog.cpp \
	LibraryDialog.cpp \
	CustomServiceDialog.cpp \
	IPServiceDialog.cpp \
	ICMPServiceDialog.cpp \
	TCPServiceDialog.cpp \
	UDPServiceDialog.cpp \
	GroupObjectDialog.cpp \
	TimeDialog.cpp \
	RuleSetDialog.cpp \
	RuleSetView.cpp \
	iptAdvancedDialog.cpp \
	ipfAdvancedDialog.cpp \
	ipfwAdvancedDialog.cpp \
	pfAdvancedDialog.cpp \
	pixAdvancedDialog.cpp \
	pixosAdvancedDialog.cpp \
	iosaclAdvancedDialog.cpp \
	iosAdvancedDialog.cpp \
	linux24AdvancedDialog.cpp \
	linksysAdvancedDialog.cpp \
	freebsdAdvancedDialog.cpp \
	openbsdAdvancedDialog.cpp \
	solarisAdvancedDialog.cpp \
	macosxAdvancedDialog.cpp \
	RuleOptionsDialog.cpp \
	RoutingRuleOptionsDialog.cpp \
	NATRuleOptionsDialog.cpp \
	LibExportDialog.cpp \
	PrefsDialog.cpp \
	instConf.cpp \
	instDialog.cpp \
	instDialog_ui_ops.cpp \
	instDialog_compile.cpp \
	instDialog_installer.cpp \
	FirewallInstaller.cpp \
	FirewallInstallerCisco.cpp \
	FirewallInstallerUnx.cpp \
	newFirewallDialog.cpp \
	newHostDialog.cpp \
	ObjConflictResolutionDialog.cpp \
	listOfLibraries.cpp \
	ColorLabelMenuItem.cpp \
	TagServiceDialog.cpp \
	ActionsDialog.cpp \
	SimpleTextView.cpp \
	DiscoveryDruid.cpp \
	DNSNameDialog.cpp \
	ObjectTreeViewItem.cpp \
	InstallFirewallViewItem.cpp \
	instOptionsDialog.cpp \
	instBatchOptionsDialog.cpp \
	FilterDialog.cpp \
	FindObjectWidget.cpp \
	FWObjectDropArea.cpp \
	CommentEditorPanel.cpp \
	MetricEditorPanel.cpp \
	FindWhereUsedWidget.cpp \
	ConfirmDeleteObjectDialog.cpp \
	FakeWizard.cpp \
    AskLibForCopyDialog.cpp \
	ObjectListViewItem.cpp \
	Help.cpp \
	HttpGet.cpp \
	StartTipDialog.cpp

FORMS = FWBMainWindow_q.ui \
	customservicedialog_q.ui \
	ipservicedialog_q.ui \
	icmpservicedialog_q.ui \
	tcpservicedialog_q.ui \
	udpservicedialog_q.ui \
	groupobjectdialog_q.ui \
	librarydialog_q.ui \
	ipv4dialog_q.ui \
	ipv6dialog_q.ui \
	rulesetdialog_q.ui \
	addressrangedialog_q.ui \
	addresstabledialog_q.ui \
	networkdialog_q.ui \
	networkdialogipv6_q.ui \
	userdialog_q.ui \
	hostdialog_q.ui \
	firewalldialog_q.ui \
	interfacedialog_q.ui \
	physaddressdialog_q.ui \
	timedialog_q.ui \
	rcsfilepreview_q.ui \
	rcsfilesavedialog_q.ui \
	iptadvanceddialog_q.ui \
	objectmanipulator_q.ui \
	prefsdialog_q.ui \
	pixadvanceddialog_q.ui \
	pixosadvanceddialog_q.ui \
	iosacladvanceddialog_q.ui \
	iosadvanceddialog_q.ui \
	simpletexteditor_q.ui \
	simpleinteditor_q.ui \
	aboutdialog_q.ui \
	libexport_q.ui \
	ruleoptionsdialog_q.ui \
	routingruleoptionsdialog_q.ui \
	instdialog_q.ui \
	objconflictresolutiondialog_q.ui \
	newfirewalldialog_q.ui \
	finddialog_q.ui \
	ipfadvanceddialog_q.ui \
	ipfwadvanceddialog_q.ui \
	pfadvanceddialog_q.ui \
	linux24advanceddialog_q.ui \
	solarisadvanceddialog_q.ui \
	freebsdadvanceddialog_q.ui \
	openbsdadvanceddialog_q.ui \
	macosxadvanceddialog_q.ui \
	colorlabelmenuitem_q.ui \
	debugdialog_q.ui \
	filepropdialog_q.ui \
	askrulenumberdialog_q.ui \
	newgroupdialog_q.ui \
	newhostdialog_q.ui \
	longtextdialog_q.ui \
	linksysadvanceddialog_q.ui \
	printingprogressdialog_q.ui \
	pagesetupdialog_q.ui \
	dnsnamedialog_q.ui \
	tagservicedialog_q.ui \
	actionsdialog_q.ui \
	simpletextview_q.ui \
	discoverydruid_q.ui \
	filterdialog_q.ui \
	natruleoptionsdialog_q.ui \
	instoptionsdialog_q.ui \
	findobjectwidget_q.ui \
	fwobjectdroparea_q.ui \
	commenteditorpanel_q.ui \
	metriceditorpanel_q.ui \
	findwhereusedwidget_q.ui \
	confirmdeleteobjectdialog_q.ui\
    projectpanel_q.ui\
    asklibforcopydialog_q.ui \
	starttipdialog_q.ui

contains( HAVE_ANTLR_RUNTIME, 1 ){
    INCLUDEPATH += $$ANTLR_INCLUDEPATH
    LIBS += $$FWBPARSER_LIB  $$ANTLR_LIBS 
    DEFINES += $$ANTLR_DEFINES
}

#!win32 {
#    QMAKE_INSTALL_PROGRAM = ../../install.sh -m 0755 -s
#}

RESOURCES += MainRes.qrc

TRANSLATIONS = fwbuilder_ru.ts fwbuilder_ja.ts

ja.path = $$PKGLOCALEDIR
ja.files = fwbuilder_ja.qm

ru.path = $$PKGLOCALEDIR
ru.files = fwbuilder_ru.qm

INSTALLS += ja ru

