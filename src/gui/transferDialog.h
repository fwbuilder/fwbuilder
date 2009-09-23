/*
 * transferDialog.h - firewall config transfer dialog
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

#ifndef __TRANSFERDIALOG_H_
#define __TRANSFERDIALOG_H_

#include "../../config.h"
#include <ui_transferdialog_q.h>

#include "fwbuilder/Firewall.h"

#include <qprocess.h>

namespace fwtransfer
{
    class TransferDeviceList;
};

typedef std::set<libfwbuilder::Firewall *> t_fwSet;
typedef std::map<libfwbuilder::Firewall *, QTreeWidgetItem *> t_listMap;

/**
 * @class transferDialog
 *
 * @brief This class is the users view to config transfer operations.
 *
 * The transferDialog class/UI is used to display transfer information to the user.
 * The user can select an usb-storage volume to transfer specific firewall
 * configs. Progress about running transfer operations is displayed in a status
 * log.
 */
class transferDialog : public QDialog
{
    Q_OBJECT

private:
    /**
     * display all selected firewalls in tree widget.
     */
    void displayFirewalls();

    /**
     * display all available usb-storage based volumes. This function is
     * also called when a devicesChanged signal is received from the
     * transferDevices list to update the volumes view.
     */
    void updateVolumeView();

    /**
     * perform an transfer of a specific firewall.
     *
     * @param fw firewall to transfer
     * @param volume volume name to transfer config to
     * @return true if transfer was successful, false if not
     */
    bool runTransfer(libfwbuilder::Firewall *fw, const QString &volume);

    /**
     * prepare argument list for transfer agent call.
     *
     * @param args argument list as QStringList to prepare
     * @param fw firewall to build transfer agent argument list for
     * @param volume volume name used for transfer
     * @return true if suitable transfer agent found, false if not
     */
    bool prepareArgs(QStringList &args, libfwbuilder::Firewall *fw,
                     const QString &volume);

    /**
     * write a message to the log widget.
     *
     * @param message log message to write
     */
    void log(const QString message);

    /**
     * set status of a firewall given by fw to status 'status', this status
     * is displayed in the firewall tree widget.
     *
     * @param fw firewall to change status
     * @param status free-text status message
     */
    void setTreeStatus(libfwbuilder::Firewall *fw, const QString &status);

    /** selected volume id */
    QString volume_name;

    /** firewalls to transfer */
    t_fwSet reqFirewalls;

    /** Available transfer devices are stored here */
    fwtransfer::TransferDeviceList *transferDevices;

    /** external process handler */
    QProcess proc;

    /** mapping between tree items and firewalls */
    t_listMap opListMapping;

    /** transferDialog UI */
    Ui::transferDialog_q *m_dialog;

public:
    /** transferDialog ctor */
    transferDialog(QWidget *parent, t_fwSet firewalls);

    /** transferDialog dtor */
    ~transferDialog();

    /**
     * return chosen volume name to caller.
     *
     * @return selected volume name (e.g. /dev/sdc1)
     */
    QString getVolumeName() const;

protected slots:
    virtual void accept();
    virtual void saveLog();
    void selected();
    void readFromStdout();
    void processExited(int code);
    void findFirewallInLog(QTreeWidgetItem*);
    void updateDeviceList();
};

#endif /* __TRANSFERDIALOG_H_ */

