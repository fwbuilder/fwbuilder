/*
 * ClusterDialog.h - Cluster object view
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

#ifndef __CLUSTERDIALOG_H_
#define __CLUSTERDIALOG_H_

#include <ui_clusterdialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "utils.h"

#include "fwbuilder/FWObject.h"

class ProjectPanel;


class ClusterDialog : public BaseObjectDialog
{
    Q_OBJECT;

    Ui::ClusterDialog_q *m_dialog;

    void resetSingleClusterGroupType(libfwbuilder::FWObject *grp,
                                     std::list<QStringPair> &allowed_types);
    void resetClusterGroupTypes();
    void updateTimeStamps();
    
public:
    ClusterDialog(QWidget *parent);
    ~ClusterDialog();

private:
    /** flag to indicate host os, platform changes */
    bool config_changed;

public slots:
    virtual void applyChanges();
    virtual void platformChanged();
    virtual void hostOSChanged();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
};

#endif /* __CLUSTERDIALOG_H_ */

