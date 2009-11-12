# -*- mode: makefile; tab-width: 4; -*-
# $Id$
TEMPLATE = app
LANGUAGE = C++
QT += network

# this should work, but it does not with 4.3.5, workaround below
# QT += network dbus
TARGET = fwbuilder
include(../../qmake.inc)
exists(qmake.inc):include( qmake.inc)
contains( HAVE_QTDBUS, 1 ):unix { 
    macx:LIBS += -framework \
        QtDBus
    !macx:LIBS += -lQtDBus # workaround for QT += dbus not working atm
}

# This makes gcc compile this header file and store result in
# .obj/fwbuilder.gch/c++. Generated Makefile will also add
# "-include .obj/fwbuilder " to the command line for the compiler
# for every module. There is no need to #include file fwbuilder_ph.h
# in each module since it will be included by means of -include option.
# PRECOMPILED_HEADER = fwbuilder_ph.h
# control whether we build debug or release in ../../qmake.inc
# CONFIG += precompile_header
HEADERS += ../../config.h \
    ../../build_num \
    events.h \
    FWWindow.h \
    ProjectPanel.h \
    BaseObjectDialog.h \
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
    ObjectEditorDockWidget.h \
    FWObjectDrag.h \
    ObjectTreeView.h \
    ObjectListView.h \
    ObjectIconView.h \
    ObjectTreeViewItem.h \
    InstallFirewallViewItem.h \
    DialogFactory.h \
    HostDialog.h \
    FirewallDialog.h \
    ClusterDialog.h \
    ClusterGroupDialog.h \
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
    ipcoposAdvancedDialog.h \
    linux24AdvancedDialog.h \
    linksysAdvancedDialog.h \
    freebsdAdvancedDialog.h \
    openbsdAdvancedDialog.h \
    solarisAdvancedDialog.h \
    macosxAdvancedDialog.h \
    secuwallAdvancedDialog.h \
    secuwallIfaceOptsDialog.h \
    vlanOnlyIfaceOptsDialog.h \
    linux24IfaceOptsDialog.h \
    openbsdIfaceOptsDialog.h \
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
    FirewallInstallerUnx.h \
    newFirewallDialog.h \
    newClusterDialog.h \
    newHostDialog.h \
    ObjConflictResolutionDialog.h \
    ColorLabelMenuItem.h \
    TagServiceDialog.h \
    ActionsDialog.h \
    SimpleTextView.h \
    DiscoveryDruid.h \
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
    AutocompletedComboBox.h \
    InterfaceEditorWidget.h \
    AddressEditor.h \
    FWCmdBasic.h \
    FWCmdChange.h \
    FWObjectState.h \
    InterfacesTabWidget.h
SOURCES += ProjectPanel.cpp \
    ProjectPanel_events.cpp \
    ProjectPanel_file_ops.cpp \
    ProjectPanel_state_ops.cpp \
    FWWindow.cpp \
    FWWindow_editor.cpp \
    FWWindow_single_rule_compile.cpp \
    FWWindow_wrappers.cpp \
    FWWindowPrint.cpp \
    ObjectEditorDockWidget.cpp \
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
    ClusterDialog.cpp \
    ClusterGroupDialog.cpp \
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
    ipcoposAdvancedDialog.cpp \
    linux24AdvancedDialog.cpp \
    linksysAdvancedDialog.cpp \
    freebsdAdvancedDialog.cpp \
    openbsdAdvancedDialog.cpp \
    solarisAdvancedDialog.cpp \
    macosxAdvancedDialog.cpp \
    secuwallAdvancedDialog.cpp \
    secuwallIfaceOptsDialog.cpp \
    vlanOnlyIfaceOptsDialog.cpp \
    linux24IfaceOptsDialog.cpp \
    openbsdIfaceOptsDialog.cpp \
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
    FirewallInstallerUnx.cpp \
    newFirewallDialog.cpp \
    newClusterDialog.cpp \
    newHostDialog.cpp \
    ObjConflictResolutionDialog.cpp \
    ColorLabelMenuItem.cpp \
    TagServiceDialog.cpp \
    ActionsDialog.cpp \
    SimpleTextView.cpp \
    DiscoveryDruid.cpp \
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
    AutocompletedComboBox.cpp \
    InterfaceEditorWidget.cpp \
    AddressEditor.cpp \
    FWCmdBasic.cpp \
    FWCmdChange.cpp \
    FWObjectState.cpp \
    InterfacesTabWidget.cpp
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
    addresstabledialog_q.ui \
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
    simpletexteditor_q.ui \
    simpleinteditor_q.ui \
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
    secuwallifaceoptsdialog_q.ui \
    clustermembersdialog_q.ui \
    openbsdifaceoptsdialog_q.ui \
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
    discoverydruid_q.ui \
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
    InterfaceEditorWidget.ui \
    AddressEditor.ui \
    InterfacesTabWidget.ui

# fwtransfer stuff
HEADERS += transferDialog.h
SOURCES += transferDialog.cpp
FORMS += transferdialog_q.ui
contains( HAVE_ANTLR_RUNTIME, 1 ) { 
    INCLUDEPATH += $$ANTLR_INCLUDEPATH
    LIBS += $$FWBPARSER_LIB \
        $$ANTLR_LIBS \
        $$FWTRANSFER_LIB
    DEFINES += $$ANTLR_DEFINES
}
LIBS += $$LIBS_FWCOMPILER
INCLUDEPATH += ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib/
DEPENDPATH = ../iptlib \
    ../pflib \
    ../cisco_lib/ \
    ../compiler_lib
win32:LIBS += ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a
win32:PRE_TARGETDEPS = ../iptlib/release/iptlib.lib \
    ../pflib/release/fwbpf.lib \
    ../cisco_lib/release/fwbcisco.lib \
    ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../iptlib/libiptlib.a \
    ../pflib/libfwbpf.a \
    ../cisco_lib/libfwbcisco.a \
    ../compiler_lib/libcompilerdriver.a
RESOURCES += MainRes.qrc
TRANSLATIONS = fwbuilder_ru.ts \
    fwbuilder_ja.ts
ja.path = $$PKGLOCALEDIR
ja.files = fwbuilder_ja.qm
ru.path = $$PKGLOCALEDIR
ru.files = fwbuilder_ru.qm
INSTALLS += ja \
    ru
