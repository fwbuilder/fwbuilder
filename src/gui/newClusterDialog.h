/*
 * newClusterDialog.h - new cluster wizard dialog
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef __NEWCLUSTERDIALOG_H_
#define __NEWCLUSTERDIALOG_H_

#include <ui_newclusterdialog_q.h>

#include "fakeWizard.h"

namespace libfwbuilder
{
    class FWObject;
    class FWObjectDatabase;
    class ObjectGroup;
    class Cluster;
    class Interface;
};

class newClusterDialog : public QDialog, public FakeWizard
{
    Q_OBJECT;

    libfwbuilder::Cluster *ncl;
    libfwbuilder::ObjectGroup *fwlist;
    libfwbuilder::FWObject *parent;
    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::FWObjectDatabase *tmpldb;
    std::map<QListWidgetItem*, libfwbuilder::FWObject*> templates;
    bool unloadTemplatesLib;
    Ui::newClusterDialog_q *m_dialog;

    bool validateAddressAndMask(const QString &addr, const QString &netm);

    void fillInterfaceData(libfwbuilder::Interface *intf, QTextBrowser *qte);
    void getInterfaceAttributes(QString *addr, QString *netmask,
                                QString *protocol,
                                QString *secret, QString *vrid);
    void shrinkListOfPlatforms();
    void shrinkListOfOS();
    QMap<QRadioButton*, libfwbuilder::Firewall*> radios;
    QMap<int, bool> visited;
    
public:
    newClusterDialog(libfwbuilder::FWObject *parent);
    virtual ~newClusterDialog();

    libfwbuilder::Cluster* getNewCluster() { return ncl; };

    virtual bool appropriate(const int page) const;
    void showPage(const int page);

public slots:
    virtual void addInterface();
    virtual void updateInterface();
    virtual void deleteInterface();
    virtual void selectedInterface(QTreeWidgetItem*,QTreeWidgetItem*);
    virtual void templateSelected(QListWidgetItem *itm);
    void browseTemplate();
    void useStandartTemplate();
    void showHideTemplatePanel();
    void failoverProtocolChanged();
    void platformChanged();
    void changed();

    
protected slots:
    virtual void finishClicked();
    virtual void cancelClicked();
    virtual void nextClicked();
    virtual void backClicked();
};

#endif /* __NEWCLUSTERDIALOG_H */

