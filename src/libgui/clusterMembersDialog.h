/*
 * cluster members configuration dialog
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

#ifndef __CLUSTERMEMBERDIALOG_H_
#define __CLUSTERMEMBERDIALOG_H_

#include <ui_clustermembersdialog_q.h>

#include "fwbuilder/FWObject.h"

namespace libfwbuilder
{
    class Firewall;
    class Interface;
};

typedef std::list<libfwbuilder::Interface *> t_ifaceList;
typedef std::map<QString, libfwbuilder::Interface *> t_ifaceMap;

/**
 * @class ClusterMember
 *
 * @brief Simple helper class to store relevant info about cluster members.
 *
 * The ClusterMember class is used to store temporary data for the configuration
 * of potential cluster members of a specific cluster.
 */
class ClusterMember
{
public:
    /** pointer to firewall object */
    libfwbuilder::FWObject *fwobj;
    /** list of pointers to all interfaces of this firewall */
    t_ifaceList iface_list;
    /** mapping from iface name to interface pointer */
    t_ifaceMap iface_map;
    /** selected as cluster interface */
    libfwbuilder::Interface *iface_cluster;
    /** flag for cluster master */
    bool is_master;
};

typedef std::list<ClusterMember *> t_memberList;

class clusterMembersDialog : public QDialog
{
    Q_OBJECT

public:
    clusterMembersDialog(QWidget *parent, libfwbuilder::FWObject *o);
    ~clusterMembersDialog();

private:
    libfwbuilder::FWObject *obj;
    bool enable_master_column;

    Ui::clusterMembersDialog_q *m_dialog;

    /** host OS of current cluster */
    QString host_os;

    /** platform of current cluster */
    QString platform;

    /** list of all still available firewalls */
    t_memberList available;

    /** list of cluster members */
    t_memberList selected;

    /** flag to indicate selected table update */
    bool table_update;

    /** init lists of already selected firewalls */
    void getSelectedMembers();

    /**
     * init lists of available firewalls
     *
     * For this type of cluster, a valid firewall must fulfill the
     * following criterias:
     * 1. host_OS and platform must match cluster settings
     * 2. the firewall must at least have one interface attached
     */
    void getPossibleMembers();

    /** update view of selected members */
    void updateSelectedTable();

    /** update view of available firewalls */
    void updateAvailableTree();

    /**
     * create a new cluster member object from fw pointer
     *
     * @param fw pointer to firewall object
     * @param cluster_iface pointer to selected cluster iface
     * @param master flag to indicate cluster master
     * @return pointer to new cluster object
     */
    ClusterMember* createMember(libfwbuilder::Firewall *fw,
                                libfwbuilder::Interface *cluster_iface = nullptr,
                                bool master = false);

    /**
     * swap firewall cluster member info from one list to another.
     *
     * @param from list to remove firewall from
     * @param to list to add firewall
     * @param fwname firewall name as identifier
     * @param iface optional interface to set as cluster interface
     * @param master optional: set master flag
     * @return true if successfully swapped, false if not
     */
    bool swap(t_memberList &from, t_memberList &to, const QString fwname,
              const QString iface, bool master = false);

    /**
     * set master firewall status in selected list
     *
     * @param fw name of firewall to set state
     * @param checked status to set (checked / unchecked)
     */
    void setMaster(QString fw, bool checked = true);

    /**
     * returns fwobject specific pixmap to caller
     *
     * @param o object to get pixmap for
     * @return object's QPixmap
     */
    QPixmap getIcon(libfwbuilder::FWObject *o);

    /** update all views */
    void invalidate();

protected slots:
    virtual void accept();
    virtual void reject();
    virtual void help();
    void availableClicked(QTreeWidgetItem *item, int);
    void selectedClicked(int row, int column);
    void masterSelected(int row, int column);
    void firewallAdd();
    void firewallRemove();

signals:
    void membersChanged();
};

/**
 * @class PredFindFw
 *
 * @brief Predicate class to find firewalls by name
 *
 * PredFindFw can be used e.g. as parameter in find_if() function to compare
 * ClusterMembers in available and selected lists with a given fwname
 * specified by setSearchString().
 */
class PredFindFw
{
protected:
    QString search_string;
public:
    PredFindFw() {};

    bool operator()(const ClusterMember *member) const
    {
        QString fwname = member->fwobj->getName().c_str();
        return (fwname == search_string);
    }

    void setSearchString(const QString &string)
    {
        search_string = string;
    }
};

#endif /* __SECUWALLCLUSTERCONFDIALOG_H_ */

