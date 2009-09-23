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

#include "../../config.h"
#include <ui_clustergroupdialog_q.h>

#include "fwbuilder/FWObject.h"

class ProjectPanel;

namespace libfwbuilder {
    class Cluster;
}

class ClusterGroupDialog : public QWidget
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;
    libfwbuilder::Cluster *cluster;
    Ui::ClusterGroupDialog_q *m_dialog;
    bool init;
    bool enable_master_column;
    
    ProjectPanel *m_project;

    void saveGroupType();

public:
    ClusterGroupDialog(ProjectPanel *project, QWidget *parent);
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
    virtual void discardChanges();
    virtual void validate(bool*);
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void openClusterConfDialog();
    virtual void closeEvent(QCloseEvent *e);
    virtual void getHelpName(QString*);

    void openObject(QTreeWidgetItem *item);
    void objectChanged();
    void openParametersEditor();
    
signals:
    /**
     * This signal is emitted from closeEvent, ObjectEditor connects
     * to this signal to make checks before the object editor can be closed
     * and to store its position on the screen
     */
    void close_sign(QCloseEvent *e);
    void changed_sign();
    void notify_changes_applied_sign();
};

#endif /* __CLUSTERDIALOG_H_ */

