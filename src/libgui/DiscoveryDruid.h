/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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


#ifndef __DISCOVERYDRUID_H_
#define __DISCOVERYDRUID_H_

#include <ui_discoverydruid_q.h>

#include <qobject.h>
#include <qthread.h>
#include <qhostinfo.h>

#include "fwbuilder/Interface.h"
#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/dns.h"
#include "fwbuilder/snmp.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Logger.h"

#include "FilterDialog.h"
#include "fakeWizard.h"

#include <vector>
#include <string>

#include <QDialog>
#include <QTextCharFormat>


using namespace std;
using namespace libfwbuilder;

class Importer;

#define IMPORT_IOS 0
#define IMPORT_PIX 1
#define IMPORT_IPT 2


// ---------------- OBJECT DESCRIPTOR ------------------ //

class ObjectDescriptor 
{
    public:

    bool    have_snmpd ;
    string  descr, contact, location, sysname    ;
    string  type;
    bool isSelected;
    

    map<int, libfwbuilder::InterfaceData> interfaces ;

    string                   MAC_addr ;
    libfwbuilder::HostEnt    dns_info ;
    libfwbuilder::InetAddr   addr     ;
    libfwbuilder::InetAddr   netmask  ;

    
    ObjectDescriptor();
    ObjectDescriptor(const ObjectDescriptor& od);

    std::string toString()
    {
        ostringstream ost;
        ost << sysname;
        //if(interfaces.size()>1)
        //   ost <<" [" <<interfaces.size() <<"]";
        ost <<" (" << addr.toString() <<")";
        return ost.str();
    }
    
#ifdef HAVE_LIBSNMP
    ObjectDescriptor(const libfwbuilder::CrawlerFind& cf);
#endif

    virtual ~ObjectDescriptor();

    ObjectDescriptor& operator=(const ObjectDescriptor& od);

};



// -------------------- EVENTS --------------------- //
enum {ProgressEv = 1001, DoneEv = 1002}; 
class ProgressEvent : public QEvent
{
    public:
        ProgressEvent():QEvent(static_cast<QEvent::Type>(ProgressEv)) {value=0;}
        int value;
};

class DoneEvent : public QEvent
{
    public:
        DoneEvent():QEvent(static_cast<QEvent::Type>(DoneEv)) {}
};


// ---------------- WORKER THREAD ------------------ //

typedef enum {BT_NONE,BT_HOSTS,BT_DNS,BT_SNMP,BT_IMPORT} BackgroundTask;

class WorkerThread : public QThread
{
    QWidget *Widget;

protected:
    QString last_error;
    
public:
    Logger *Log;

    void setProgress(int p);
    void done();
    void setTargetWidget(QWidget *w) {Widget=w;}
    QString getError();
    WorkerThread();
    virtual ~WorkerThread();

    virtual void run();
};

class HostsFileImport : public WorkerThread
{
    QString file_name;

public:
    vector<ObjectDescriptor>  hosts;

    HostsFileImport(const QString &f);

    virtual void run();
};

class ConfigImport : public WorkerThread
{
    std::string *buffer;
    Importer    *imp;
    std::string  platform;
    std::string  fwname;
        
public:
    ConfigImport(std::string *buffer, const std::string &platform, const std::string &fwname);
    virtual ~ConfigImport();

    virtual void run();
    Importer* getImporterObject() { return imp; }
};

// ---------------- DISCOVERY DRUID ------------------ //

class DiscoveryDruid : public QDialog, public FakeWizard
{
    Q_OBJECT;

private:
    WorkerThread *thread;
    BackgroundTask current_task;
    Filter * flt_obj;
    Filter * flt_last;
    Filter * flt_net;
    FilterDialog * flt_obj_d;
    FilterDialog * flt_last_d;
    FilterDialog * flt_net_d;
    Ui::DiscoveryDruid_q * m_dialog;
    QButtonGroup * dm_method;
    QTextCharFormat normal_format;
    QTextCharFormat error_format;
    QTextCharFormat warning_format;

    bool init;
    bool isSeedHostOK;
    bool isSNMPInclNetOK;
    bool userIsTyping;
    
    //QueueLogger * logger;
    Logger * logger;
    BackgroundOp *bop;

    QHostInfo *dns;
    
    int FromPage;
    QMap<QString,ObjectDescriptor> Objects;
    QMap<QString,ObjectDescriptor> Networks;
    QMap<QString,InetAddr> NameServers;
    vector<libfwbuilder::InetAddrMask>  include_networks;

    QTimer* timer;
    QTimer* prg_timer;
    int unProg;
    QProgressBar *unBar;
    QLabel *errMessage;
    QString HostName;

    libfwbuilder::Firewall *discovered_fw;
    
    void setDiscoveryMethod_file();
    void setDiscoveryMethod_DNS();
    void setDiscoveryMethod_SNMP();
    void setDiscoveryMethod_Import();

    void startBackgroundProcess();
    void DataFromCrawler();
    int monitorOperation();
    void restore();
    void save();
    QString guessOS(const string &sysDescr);

    void rearrangeInterfaces(
        const QString &os,
        std::map<int,libfwbuilder::InterfaceData> &interfaces,
        std::list<libfwbuilder::InterfaceData*> &interface_tree);
    FWObject* addInterface(libfwbuilder::FWObject *parent,
                           libfwbuilder::InterfaceData *in,
                           bool skip_ip_address_check);
    void addToLog(const QString &buf);

    std::string selectedPlatform();
    
public:

    DiscoveryDruid(QWidget *parent, bool start_with_import=false);
    virtual ~DiscoveryDruid();
    void fillListOfObjects();
    void fillTypeChangingList();
    void fillObjects();
    void fillNetworks();
    void loadDataFromFile();
    void loadDataFromImporter();
    void loadDataFromCrawler();
    void loadDataFromDNS();
    void fillListOfNetworks();
    void fillNetworkZones();
    void createRealObjects();
//    void stripObjects();
    void getNameServers();
    InetAddr getNS();
    InetAddr getSeedHostAddress();
    bool isInetAddr(const QString s);
    QString testInetAddr(const QString s);

    virtual void customEvent(QEvent *event);
    

public slots:
    virtual void changedSelected( const int &page );
    virtual void changedDiscoveryMethod(int);
    virtual void browseHostsFile();
    virtual void browseForImport();
    virtual void saveScanLog();
    virtual void startHostsScan();
    virtual void startDNSScan();
    virtual void startSNMPScan();
    virtual void startConfigImport();
    virtual void importPlatformChanged(int cp);

    virtual void changedDomainName();
    virtual void changedHostsFileName();
    virtual void changedSNMPOptions();
    virtual void changedSeedHost();
    virtual void changedInclNet();
    virtual void stopBackgroundProcess();
    virtual void addNetwork();
    virtual void removeNetwork();
    virtual void setNetworkFilter();
    virtual void removeNetworkFilter();
    virtual void setLastFilter();
    virtual void removeLastFilter();
    virtual void addObject();
    virtual void removeObject();
    virtual void setObjectFilter();
    virtual void removeObjectFilter();
    virtual void updateLog();
    virtual void updatePrg();
    virtual void checkHostName();
    virtual void checkSNMPCommunity();
    virtual void selectAllResNets();
    virtual void selectAllNets();
    virtual void selectAllResObjs();
    virtual void selectAllObjs();
    virtual void selectAllLast();
    virtual void unselectAllLast();
    virtual void changeTargetObject(const QString &buf);
    virtual void typeAddress();
    virtual void typeHost();
    virtual void typeFirewall();
    virtual void dnsFinish(QHostInfo);
    virtual void changedNameServer();
    virtual void typedCustomNS();
//    virtual void createObjects(const QString &buf);
    
    virtual void nextClicked();
    virtual void backClicked();
    virtual void cancelClicked();
    virtual void finishClicked();

    virtual void objNameChanged(QString name);
 signals:
    
    
};

#define CHOOSE_METHOD_PAGE 0
#define READ_HOSTS_FILE_PAGE 1
#define IMPORT_CONFIG_PAGE 2
#define IMPORT_DNS_ZONE_PAGE 3
#define NAME_SERVER_PAGE 4
#define SNMP_DISCOVERY_PAGE 5
#define NETWORK_SCAN_OPTIONS_PAGE 6
#define SNMP_PARAMETERS_PAGE 7
#define BACKGROUND_PROCESS_PAGE 8
#define CHOOSE_NETWORKS_PAGE 9
#define CHOOSE_OBJECTS_PAGE 10
#define ADJUST_OBJECT_TYPES_PAGE 11
#define TARGET_LIB_PAGE 12
#define CREATE_OBJECTS_PAGE 13
#define NETWORK_ZONES_PAGE 14

const int  WIZARD_PAGES = 15;
const bool WIZARD_FILE_PAGES[] =   {1,1,0,0,0,0,0,0,1,0,1,0,1,1,0};
const bool WIZARD_DNS_PAGES[]  =   {1,0,0,1,1,0,0,0,1,0,1,0,1,1,0};
const bool WIZARD_SNMP_PAGES[] =   {1,0,0,0,0,1,1,1,1,1,1,1,1,1,0};
const bool WIZARD_IMPORT_PAGES[] = {1,0,1,0,0,0,0,0,1,0,0,0,0,0,1};



#endif 
