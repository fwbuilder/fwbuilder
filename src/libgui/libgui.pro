# -*- mode: makefile; tab-width: 4; -*-

QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
include(../../qmake.inc)

exists(qmake.inc):include( qmake.inc)

TEMPLATE = lib

CONFIG += staticlib

TARGET = gui

INSTALLS -= target

# This makes gcc compile this header file and store result in
# .obj/fwbuilder.gch/c++. Generated Makefile will also add
# "-include .obj/fwbuilder " to the command line for the compiler
# for every module. There is no need to #include file fwbuilder_ph.h
# in each module since it will be included by means of -include option.
# PRECOMPILED_HEADER = fwbuilder_ph.h
# control whether we build debug or release in ../../qmake.inc
# CONFIG += precompile_header
HEADERS += ../../config.h \
    events.h \
    FWWindow.h \
    ProjectPanel.h \
    BaseObjectDialog.h \
    TextEditWidget.h \
    utils.h \
    utils_no_qt.h \
    SSHSession.h \
    SSHUnx.h \
    SSHCisco.h \
    SSHPIX.h \
    SSHIOS.h \
    SSHNXOS.h \
    SSHJUNOS.h \
    SSHProcurve.h \
    debugDialog.h \
    findDialog.h \
	listOfLibrariesModel.h \
    longTextDialog.h \
    newGroupDialog.h \
    filePropDialog.h \
    DialogData.h \
    SimpleTextEditor.h \
    SimpleIntEditor.h \
	TextFileEditor.h \
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
    ObjectEditorDockWidget.h \
    FWObjectDrag.h \
    ObjectTreeView.h \
    ObjectListView.h \
    ObjectIconView.h \
    ObjectTreeViewItem.h \
	ObjectSelectorWidget.h \
    InstallFirewallViewItem.h \
    DialogFactory.h \
    HostDialog.h \
    FirewallDialog.h \
    ClusterDialog.h \
    ClusterGroupDialog.h \
    InterfaceDialog.h \
    AddressRangeDialog.h \
    AddressRangeIPv6Dialog.h \
    AddressTableDialog.h \
    IPv4Dialog.h \
    IPv6Dialog.h \
    PhysicalAddressDialog.h \
	AttachedNetworksDialog.h \
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
    ColDesc.h \
    FWObjectSelectionModel.h \
    RuleNode.h \
    RuleSetModel.h \
    RuleSetView.h \
    RuleSetViewDelegate.h \
    iptAdvancedDialog.h \
    ipcopAdvancedDialog.h \
    ipfAdvancedDialog.h \
    ipfwAdvancedDialog.h \
    pfAdvancedDialog.h \
    pixAdvancedDialog.h \
    pixosAdvancedDialog.h \
    iosaclAdvancedDialog.h \
    iosAdvancedDialog.h \
    nxosaclAdvancedDialog.h \
    nxosAdvancedDialog.h \
    junosaclAdvancedDialog.h \
    junosAdvancedDialog.h \
    ipcoposAdvancedDialog.h \
    linux24AdvancedDialog.h \
    linksysAdvancedDialog.h \
    freebsdAdvancedDialog.h \
    openbsdAdvancedDialog.h \
    procurveaclAdvancedDialog.h \
    solarisAdvancedDialog.h \
    macosxAdvancedDialog.h \
    secuwallAdvancedDialog.h \
    secuwallosAdvancedDialog.h \
    secuwallIfaceOptsDialog.h \
    vlanOnlyIfaceOptsDialog.h \
    linux24IfaceOptsDialog.h \
    pixosIfaceOptsDialog.h \
    bsdIfaceOptsDialog.h \
    clusterMembersDialog.h \
    CompilerOutputPanel.h \
    CompilerDriverFactory.h \
    RuleOptionsDialog.h \
    RoutingRuleOptionsDialog.h \
    NATRuleOptionsDialog.h \
    LibExportDialog.h \
    PrefsDialog.h \
    instConf.h \
    instDialog.h \
    FirewallInstaller.h \
    FirewallInstallerCisco.h \
    FirewallInstallerJuniper.h \
    FirewallInstallerProcurve.h \
    FirewallInstallerUnx.h \
    newFirewallDialog.h \
    newClusterDialog.h \
    newHostDialog.h \
    ObjConflictResolutionDialog.h \
    ColorLabelMenuItem.h \
    TagServiceDialog.h \
    ActionsDialog.h \
    SimpleTextView.h \
    BlankDialog.h \
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
    Help.h \
    HttpGet.h \
    StartTipDialog.h \
    vrrpOptionsDialog.h \
    carpOptionsDialog.h \
    pixFailoverOptionsDialog.h \
    conntrackOptionsDialog.h \
    heartbeatOptionsDialog.h \
    openaisOptionsDialog.h \
    pfsyncOptionsDialog.h \
    check_update_url.h \
    startup_tip_url.h \
    InterfaceEditorWidget.h \
    FWCmdBasic.h \
    FWCmdChange.h \
    FWCmdAddObject.h \
    FWCmdDeleteObject.h \
    FWCmdMoveObject.h \
    InterfacesTabWidget.h \
    FirewallSelectorWidget.h \
    ClusterInterfacesSelectorWidget.h \
    ClusterInterfaceWidget.h \
    FWCmdRule.h \
    UsageResolver.h \
    IconSetter.h \
    TutorialDialog.h \
    MDIEventFilter.h \
    FWBApplication.h \
    WorkflowIcons.h \
    FirewallCodeViewer.h \
	networkZoneManager.h  \
	KeywordsDialog.h \
	CommentKeywords.h \
	DynamicGroupDialog.h \
	FilterLineEdit.h \
\
	ObjectDescriptor.h \
	QThreadLogger.h \
\
	importAddressListWizard/ChooseObjectsPage.h \
	importAddressListWizard/CreateObjectsPage.h \
	importAddressListWizard/FileNamePage.h \
	importAddressListWizard/SelectLibraryPage.h \
	importAddressListWizard/ImportAddressListWizard.h \
	importAddressListWizard/HostsFile.h \
\
	snmpNetworkDiscoveryWizard/ND_ChooseNetworksPage.h \
	snmpNetworkDiscoveryWizard/ND_ChooseObjectsPage.h \
	snmpNetworkDiscoveryWizard/ND_ChooseObjectTypePage.h \
	snmpNetworkDiscoveryWizard/ND_CreateObjectsPage.h \
	snmpNetworkDiscoveryWizard/ND_DiscoveryParametersPage.h \
	snmpNetworkDiscoveryWizard/ND_ProgressPage.h \
	snmpNetworkDiscoveryWizard/ND_SelectLibraryPage.h \
	snmpNetworkDiscoveryWizard/ND_SetupPage.h \
	snmpNetworkDiscoveryWizard/ND_SNMPParametersPage.h \
	snmpNetworkDiscoveryWizard/SNMPNetworkDiscoveryWizard.h \
	snmpNetworkDiscoveryWizard/SNMPCrawlerThread.h \
\
	importFirewallConfigurationWizard/IC_FileNamePage.h \
	importFirewallConfigurationWizard/IC_FirewallNamePage.h \
	importFirewallConfigurationWizard/IC_PlatformWarningPage.h \
	importFirewallConfigurationWizard/IC_ProgressPage.h \
	importFirewallConfigurationWizard/IC_NetworkZonesPage.h \
	importFirewallConfigurationWizard/ImportFirewallConfigurationWizard.h \
	importFirewallConfigurationWizard/ImporterThread.h \
    	RuleSetDiffDialog.h \
	RuleSetDiffDelegate.h \
    	RuleSetDiffModel.h \
    	BackgroundCompileInfoWidget.h \
    	temporarydir.h 


SOURCES += ProjectPanel.cpp \
    ProjectPanel_events.cpp \
    ProjectPanel_file_ops.cpp \
    ProjectPanel_state_ops.cpp \
    BaseObjectDialog.cpp \
    FWWindow.cpp \
    FWWindow_editor.cpp \
    FWWindow_single_rule_compile.cpp \
    FWWindow_wrappers.cpp \
    FWWindowPrint.cpp \
    TextEditWidget.cpp \
    ObjectEditorDockWidget.cpp \
    ssh_wrappers.cpp \
    utils.cpp \
    utils_no_qt.cpp \
    SSHSession.cpp \
    SSHUnx.cpp \
    SSHCisco.cpp \
    SSHPIX.cpp \
    SSHIOS.cpp \
    SSHNXOS.cpp \
    SSHJUNOS.cpp \
    SSHProcurve.cpp \
    debugDialog.cpp \
    findDialog.cpp \
	listOfLibrariesModel.cpp \
    longTextDialog.cpp \
    newGroupDialog.cpp \
    filePropDialog.cpp \
    DialogData.cpp \
    SimpleTextEditor.cpp \
    SimpleIntEditor.cpp \
	TextFileEditor.cpp \
    FWBSettings.cpp \
    FWBTree.cpp \
    RCS.cpp \
    RCSFilePreview.cpp \
    FWObjectClipboard.cpp \
    platforms.cpp \
    printerStream.cpp \
    PrintingProgressDialog.cpp \
    PrintingController.cpp \
    FWObjectPropertiesFactory.cpp \
    ObjectManipulator.cpp \
    ObjectManipulator_ops.cpp \
    ObjectManipulator_tree_ops.cpp \
    ObjectManipulator_create_new.cpp \
    ObjectManipulator_slots.cpp \
    ObjectManipulator_new_object_checks.cpp \
    ObjectEditor.cpp \
    FWObjectDrag.cpp \
    ObjectTreeView.cpp \
    ObjectListView.cpp \
    ObjectIconView.cpp \
	ObjectSelectorWidget.cpp \
    DialogFactory.cpp \
    HostDialog.cpp \
    FirewallDialog.cpp \
    ClusterDialog.cpp \
    ClusterGroupDialog.cpp \
    InterfaceDialog.cpp \
    AddressRangeDialog.cpp \
    AddressRangeIPv6Dialog.cpp \
    AddressTableDialog.cpp \
    IPv4Dialog.cpp \
    IPv6Dialog.cpp \
    PhysicalAddressDialog.cpp \
AttachedNetworksDialog.cpp \
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
    FWObjectSelectionModel.cpp \
    ColDesc.cpp \
    RuleNode.cpp \
    RuleSetModel.cpp \
    RuleSetView.cpp \
    RuleSetViewDelegate.cpp \
    iptAdvancedDialog.cpp \
    ipcopAdvancedDialog.cpp \
    ipfAdvancedDialog.cpp \
    ipfwAdvancedDialog.cpp \
    pfAdvancedDialog.cpp \
    pixAdvancedDialog.cpp \
    pixosAdvancedDialog.cpp \
    iosaclAdvancedDialog.cpp \
    iosAdvancedDialog.cpp \
    nxosaclAdvancedDialog.cpp \
    nxosAdvancedDialog.cpp \
    junosaclAdvancedDialog.cpp \
    junosAdvancedDialog.cpp \
    ipcoposAdvancedDialog.cpp \
    linux24AdvancedDialog.cpp \
    linksysAdvancedDialog.cpp \
    freebsdAdvancedDialog.cpp \
    openbsdAdvancedDialog.cpp \
    procurveaclAdvancedDialog.cpp \
    solarisAdvancedDialog.cpp \
    macosxAdvancedDialog.cpp \
    secuwallAdvancedDialog.cpp \
    secuwallosAdvancedDialog.cpp \
    secuwallIfaceOptsDialog.cpp \
    vlanOnlyIfaceOptsDialog.cpp \
    linux24IfaceOptsDialog.cpp \
    pixosIfaceOptsDialog.cpp \
    bsdIfaceOptsDialog.cpp \
    clusterMembersDialog.cpp \
    CompilerOutputPanel.cpp \
    CompilerDriverFactory.cpp \
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
    FirewallInstallerJuniper.cpp \
    FirewallInstallerProcurve.cpp \
    FirewallInstallerUnx.cpp \
    newFirewallDialog.cpp \
    newFirewallDialog_from_template.cpp \
    newClusterDialog.cpp \
    newClusterDialog_create.cpp \
    newHostDialog.cpp \
    ObjConflictResolutionDialog.cpp \
    ColorLabelMenuItem.cpp \
    TagServiceDialog.cpp \
    ActionsDialog.cpp \
    SimpleTextView.cpp \
    BlankDialog.cpp \
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
    StartTipDialog.cpp \
    FWBAboutDialog.cpp \
    vrrpOptionsDialog.cpp \
    carpOptionsDialog.cpp \
    pixFailoverOptionsDialog.cpp \
    conntrackOptionsDialog.cpp \
    pfsyncOptionsDialog.cpp \
    heartbeatOptionsDialog.cpp \
    openaisOptionsDialog.cpp \
    InterfaceEditorWidget.cpp \
    FWCmdBasic.cpp \
    FWCmdChange.cpp \
    FWCmdAddObject.cpp \
    FWCmdDeleteObject.cpp \
    FWCmdMoveObject.cpp \
    InterfacesTabWidget.cpp \
    FirewallSelectorWidget.cpp \
    ClusterInterfacesSelectorWidget.cpp \
    ClusterInterfaceWidget.cpp \
    FWCmdRule.cpp \
    IconSetter.cpp \
    UsageResolver.cpp \
    TutorialDialog.cpp \
    MDIEventFilter.cpp \
    FWBApplication.cpp \
    WorkflowIcons.cpp \
    FirewallCodeViewer.cpp \
	networkZoneManager.cpp \
	KeywordsDialog.cpp \
	CommentKeywords.cpp \
	DynamicGroupDialog.cpp \
	FilterLineEdit.cpp \
\
	ObjectDescriptor.cpp \
	QThreadLogger.cpp \
\
	importAddressListWizard/ChooseObjectsPage.cpp \
	importAddressListWizard/CreateObjectsPage.cpp \
	importAddressListWizard/FileNamePage.cpp \
	importAddressListWizard/SelectLibraryPage.cpp \
	importAddressListWizard/ImportAddressListWizard.cpp \
	importAddressListWizard/HostsFile.cpp \
\
	snmpNetworkDiscoveryWizard/ND_ChooseNetworksPage.cpp \
	snmpNetworkDiscoveryWizard/ND_ChooseObjectsPage.cpp \
	snmpNetworkDiscoveryWizard/ND_ChooseObjectTypePage.cpp \
	snmpNetworkDiscoveryWizard/ND_CreateObjectsPage.cpp \
	snmpNetworkDiscoveryWizard/ND_DiscoveryParametersPage.cpp \
	snmpNetworkDiscoveryWizard/ND_ProgressPage.cpp \
	snmpNetworkDiscoveryWizard/ND_SelectLibraryPage.cpp \
	snmpNetworkDiscoveryWizard/ND_SetupPage.cpp \
	snmpNetworkDiscoveryWizard/ND_SNMPParametersPage.cpp \
	snmpNetworkDiscoveryWizard/SNMPNetworkDiscoveryWizard.cpp \
	snmpNetworkDiscoveryWizard/SNMPCrawlerThread.cpp \
\
	importFirewallConfigurationWizard/IC_FileNamePage.cpp \
	importFirewallConfigurationWizard/IC_FirewallNamePage.cpp \
	importFirewallConfigurationWizard/IC_PlatformWarningPage.cpp \
	importFirewallConfigurationWizard/IC_ProgressPage.cpp \
	importFirewallConfigurationWizard/IC_NetworkZonesPage.cpp \
	importFirewallConfigurationWizard/ImportFirewallConfigurationWizard.cpp \
	importFirewallConfigurationWizard/ImporterThread.cpp \
    	RuleSetDiffDialog.cpp \
    	RuleSetDiffDelegate.cpp \
    	RuleSetDiffModel.cpp \
    	BackgroundCompileInfoWidget.cpp \
    	temporarydir.cpp  

FORMS = FWBMainWindow_q.ui \
    compileroutputpanel_q.ui \
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
    addressrangeipv6dialog_q.ui \
    addresstabledialog_q.ui \
	attachednetworksdialog_q.ui \
    networkdialog_q.ui \
    networkdialogipv6_q.ui \
    userdialog_q.ui \
    hostdialog_q.ui \
    firewalldialog_q.ui \
    clusterdialog_q.ui \
    clustergroupdialog_q.ui \
    interfacedialog_q.ui \
    physaddressdialog_q.ui \
    timedialog_q.ui \
    rcsfilepreview_q.ui \
    rcsfilesavedialog_q.ui \
    iptadvanceddialog_q.ui \
    ipcopadvanceddialog_q.ui \
    ipcoposadvanceddialog_q.ui \
    objectmanipulator_q.ui \
    prefsdialog_q.ui \
    pixadvanceddialog_q.ui \
    pixosadvanceddialog_q.ui \
    iosacladvanceddialog_q.ui \
    iosadvanceddialog_q.ui \
    nxosacladvanceddialog_q.ui \
    nxosadvanceddialog_q.ui \
    junosacladvanceddialog_q.ui \
    junosadvanceddialog_q.ui \
    procurveacladvanceddialog_q.ui \
    simpletexteditor_q.ui \
    simpleinteditor_q.ui \
	textfileeditor_q.ui \
    aboutdialog_q.ui \
    libexport_q.ui \
    ruleoptionsdialog_q.ui \
    routingruleoptionsdialog_q.ui \
    instdialog_q.ui \
    objconflictresolutiondialog_q.ui \
    newfirewalldialog_q.ui \
    newclusterdialog_q.ui \
    finddialog_q.ui \
    ipfadvanceddialog_q.ui \
    ipfwadvanceddialog_q.ui \
    pfadvanceddialog_q.ui \
    linux24advanceddialog_q.ui \
    solarisadvanceddialog_q.ui \
    freebsdadvanceddialog_q.ui \
    openbsdadvanceddialog_q.ui \
    macosxadvanceddialog_q.ui \
    secuwalladvanceddialog_q.ui \
    secuwallosadvanceddialog_q.ui \
    secuwallifaceoptsdialog_q.ui \
    clustermembersdialog_q.ui \
    bsdifaceoptsdialog_q.ui \
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
    blankdialog_q.ui \
    dnsnamedialog_q.ui \
    tagservicedialog_q.ui \
    actionsdialog_q.ui \
    simpletextview_q.ui \
    helpview_q.ui \
    filterdialog_q.ui \
    natruleoptionsdialog_q.ui \
    instoptionsdialog_q.ui \
    findobjectwidget_q.ui \
    fwobjectdroparea_q.ui \
    commenteditorpanel_q.ui \
    metriceditorpanel_q.ui \
    findwhereusedwidget_q.ui \
    confirmdeleteobjectdialog_q.ui \
    projectpanel_q.ui \
    asklibforcopydialog_q.ui \
    starttipdialog_q.ui \
    vrrpoptionsdialog_q.ui \
    carpoptionsdialog_q.ui \
    pixfailoveroptionsdialog_q.ui \
    conntrackoptionsdialog_q.ui \
    heartbeatoptionsdialog_q.ui \
    openaisoptionsdialog_q.ui \
    pfsyncoptionsdialog_q.ui \
    vlanonlyifaceoptsdialog_q.ui \
    linux24ifaceoptsdialog_q.ui \
    pixosifaceoptsdialog_q.ui \
    InterfaceEditorWidget.ui \
    InterfacesTabWidget.ui \
    ClusterInterfaceWidget.ui \
    TutorialDialog.ui \
    WorkflowIcons.ui \
    FirewallCodeViewer.ui \
	objectselectorwidget_q.ui \
	keywordsdialog_q.ui \
	commentkeywords_q.ui \
	dynamicgroupdialog_q.ui \
\
	importAddressListWizard/chooseobjectspage_q.ui \
	importAddressListWizard/createobjectspage_q.ui \
	importAddressListWizard/filenamepage_q.ui \
	importAddressListWizard/selectlibrarypage_q.ui \
\
	snmpNetworkDiscoveryWizard/nd_choosenetworkspage_q.ui \
	snmpNetworkDiscoveryWizard/nd_chooseobjectspage_q.ui \
	snmpNetworkDiscoveryWizard/nd_chooseobjecttypepage_q.ui \
	snmpNetworkDiscoveryWizard/nd_createobjectspage_q.ui \
	snmpNetworkDiscoveryWizard/nd_discoveryparameterspage_q.ui \
	snmpNetworkDiscoveryWizard/nd_progresspage_q.ui \
	snmpNetworkDiscoveryWizard/nd_selectlibrarypage_q.ui \
	snmpNetworkDiscoveryWizard/nd_setuppage_q.ui \
	snmpNetworkDiscoveryWizard/nd_snmpparameterspage_q.ui \
\
	importFirewallConfigurationWizard/ic_filenamepage_q.ui \
	importFirewallConfigurationWizard/ic_firewallnamepage_q.ui \
	importFirewallConfigurationWizard/ic_platformwarningpage_q.ui \
	importFirewallConfigurationWizard/ic_progresspage_q.ui \
	importFirewallConfigurationWizard/ic_networkzonespage_q.ui \
    rulesetdiffdialog_q.ui 


# fwtransfer stuff.
# HEADERS += transferDialog.h
# SOURCES += transferDialog.cpp
# FORMS += transferdialog_q.ui

# !macx:LIBS += -lQtDBus # workaround for QT += dbus not working with Qt < 4.4.0

INCLUDEPATH += \
	../.. \
    ../import \
    ../iptlib \
    ../pflib \
    ../cisco_lib \
    ../juniper_lib \
    ../compiler_lib \
	../libfwbuilder/src

DEPENDPATH += \
    ../import \
    ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../juniper_lib \
    ../compiler_lib \
	../libfwbuilder/src

RESOURCES += MainRes.qrc

win32 {
    # add "." to include path to make sure #include works when we
    # compile modules in subdirectories, such as
    # importAddressListWizard and other wizards. This seems to only be
    # necessary on Windows
    INCLUDEPATH += .
}
