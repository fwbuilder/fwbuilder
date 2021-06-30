/*
 * ClusterGroupDialog.h - ClusterGroup object view
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

#ifndef __CLUSTERGROUPDIALOG_H_
#define __CLUSTERGROUPDIALOG_H_

#include <ui_clustergroupdialog_q.h>
#include "BaseObjectDialog.h"

#include "fwbuilder/FWObject.h"

class ProjectPanel;

namespace libfwbuilder {
    class Cluster;
}

class ClusterGroupDialog : public BaseObjectDialog
{
    Q_OBJECT;

    libfwbuilder::Cluster *cluster;
    Ui::ClusterGroupDialog_q *m_dialog;
    bool enable_master_column;
    
    void saveGroupType(libfwbuilder::FWObject *group);

public:
    ClusterGroupDialog(QWidget *parent);
    ~ClusterGroupDialog();

private:
    /** flag to indicate that a reload of the obj takes place */
    bool reload;

    /**
     * add fw/interface link icon to the fwMemberTree
     *
     * @param o fwobject to add a link for
     * @param set_background if true, a different background color is used
     */
    void addIcon(libfwbuilder::FWObject *o, bool set_background = false);

public slots:
    virtual void changed();
    virtual void applyChanges();
    virtual void validate(bool*);
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void openClusterConfDialog();

    void openObject(QTreeWidgetItem *item);
    void objectChanged();
    void openParametersEditor();
};

#endif /* __CLUSTERDIALOG_H_ */

