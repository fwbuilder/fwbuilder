/*
 * TransferDevice.cpp - fwtransfer library implementation
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

#include <cmath>
#include <algorithm>
#include <string>

#include <sys/types.h>

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QtDBus/QtDBus>

#include "TransferDevice.h"

using namespace fwtransfer;
using namespace libfwbuilder;

#ifdef HAVE_QTDBUS

QVariant getProperty(QDBusInterface &interface, QString prop)
    throw(FWException)
{
    QDBusMessage msg = interface.call("GetProperty", prop);
    if (msg.type() == QDBusMessage::ErrorMessage)
    {
        throw FWException(interface.path().toStdString() +
                          ": could not get property => " + prop.toStdString());
    }
    QVariant ret = msg.arguments()[0];
    return ret;
}

QString TransferDevice::getVolumeSizeStr() const
{
    // get size in bytes
    qlonglong size = getVolumeSize();
    QString size_str;
    QString unit = "KB";
    QVariant size_v;

    if (size != 0)
    {
        // smallest unit is kb
        size = size / 1024;
        size_v = size;
        if (size > 1024)
        {
            size = size / 1024;
            size_v = size;
            unit = "MB";
        }
        if (size > 1024)
        {
            float f_size = float(size) / 1024;
            size_v = floor(f_size * 100.0 + 0.5) / 100;
            unit = "GB";
        }
        size_str = size_v.toString() + " " + unit;
    }
    else
    {
        size_str = "0";
    }
    return size_str;
}

void TransferDevice::dump() const
{
    // header
    qDebug() << "( dump of transfer device " << getDeviceName() << " )";

    // actual dump
    QString mounted = is_mounted ? "yes" : "no";
    QString removable = is_removable ? "yes" : "no";
    QString hotpluggable = is_hotpluggable ? "yes" : "no";

    qDebug() << "volume UDI\t:\t"   + getVolumeUDI();
    qDebug() << "fstype\t\t:\t"     + getVolumeFS();
    qDebug() << "size (MB)\t:\t"    + getVolumeSizeStr();
    qDebug() << "device UDI\t:\t"   + getDeviceUDI();
    qDebug() << "device\t\t:\t"     + getDeviceName();
    qDebug() << "bus\t\t:\t"        + getDeviceBus();
    qDebug() << "type\t\t:\t"       + getDeviceType();
    qDebug() << "mountpoint\t:\t"   + getMountpoint();
    qDebug() << "mounted\t:\t"      + mounted;
    qDebug() << "removable\t:\t"    + removable;
    qDebug() << "hotpluggable\t:\t" + hotpluggable;
    qDebug();
}

void TransferDevice::mount() throw(FWException)
{
    // ignore request if already mounted
    if (is_mounted)
    {
        return;
    }

    QDBusConnection conn = QDBusConnection::systemBus();

    QDBusInterface mountiface("org.freedesktop.Hal", getVolumeUDI(),
                              "org.freedesktop.Hal.Device.Volume", conn);

    QStringList options;

#ifndef WIN32
    // special mount options for vfat filesystems
    if (getVolumeFS() == "vfat")
    {
        QVariant user_v = getuid();
        QString user = "uid=" + user_v.toString();

        options << "quiet" << "shortname=mixed" << user << "umask=077";
    }
#endif

    // send mount DBus message
    QDBusMessage replyMsg = mountiface.call("Mount", "", "", options);
    if (replyMsg.type() == QDBusMessage::ErrorMessage)
    {
        qDebug() << "Could not mount : " <<
        replyMsg.errorMessage() << ", " <<
        replyMsg.errorName();
        throw FWException(replyMsg.errorName().toStdString() + " : " +
                          replyMsg.errorMessage().toStdString());
    }
    else
    {
        // re-read mountpoint
        QDBusInterface volumeiface("org.freedesktop.Hal", getVolumeUDI(),
                                   "org.freedesktop.Hal.Device", conn);
        setMountpoint(getProperty(volumeiface, "volume.mount_point").toString());
    }
}

void TransferDeviceList::init() throw(FWException)
{
    if (!checkDBus())
    {
        throw FWException("Cannot connect to the D-BUS system bus.");
    }

    QDBusInterface hal("org.freedesktop.Hal",
                       "/org/freedesktop/Hal/Manager",
                       "org.freedesktop.Hal.Manager",
                       QDBusConnection::systemBus());

    // clear 'old' devices
    clear();

    // get all volumes from HAL
    QDBusMessage msg = hal.call("FindDeviceByCapability", "volume");

    QList<QVariant> volumes = msg.arguments();

    foreach (QVariant name, volumes)
    {
        QStringList volume_list = name.toStringList();
        foreach (QString vol, volume_list)
        {
            addNewVolume(vol);
        }
    }

    bool success;
    // connect HAL signals to our observer slots
    success = hal.connection().connect("org.freedesktop.Hal",
                                       "/org/freedesktop/Hal/Manager",
                                       "org.freedesktop.Hal.Manager",
                                       "DeviceAdded", this,
                                       SLOT(newDeviceDetected(const QString &)));

    if (!success)
    {
        throw FWException("Cannot subscribe to HAL 'DeviceAdded' signal.");
    }

    success = hal.connection().connect("org.freedesktop.Hal",
                                       "/org/freedesktop/Hal/Manager",
                                       "org.freedesktop.Hal.Manager",
                                       "DeviceRemoved", this,
                                       SLOT(deviceRemovedDetected(const QString &)));

    if (!success)
    {
        throw FWException("Cannot subscribe to HAL 'DeviceRemoved' signal.");
    }
}

void TransferDeviceList::dump() const
{
    TransferDeviceList::const_iterator it;
    for (it = this->begin(); it != this->end(); it++)
    {
        (*it).dump();
    }
}

bool TransferDeviceList::checkDBus() const
{
    if (!QDBusConnection::systemBus().isConnected())
    {
        fprintf(stderr, "Cannot connect to the D-BUS system bus.\n");
        return false;
    }
    return true;
}

bool TransferDeviceList::addNewVolume(const QString &udi)
{
    QDBusConnection conn = QDBusConnection::systemBus();
    QDBusInterface volume("org.freedesktop.Hal", udi,
                          "org.freedesktop.Hal.Device", conn);

    // only consider volumes for addition
    QVariant is_volume;
    try
    {
        // getProperty throws an exception if property is not there
        is_volume = getProperty(volume, "block.is_volume");
    }
    catch (FWException &ex)
    {
        return false;
    }
    // property is there, but still it is not a volume
    if (!is_volume.toBool())
    {
        return false;
    }

    // read in new volume/device
    TransferDevice new_device;
    new_device.setVolumeUDI(udi);


    // get physical device UDI and path for volume
    new_device.setDeviceUDI(getProperty(volume,
                                        "block.storage_device").toString());
    new_device.setDeviceName(getProperty(volume,
                                         "block.device").toString());

    // check if its already mounted
    new_device.setMounted(getProperty(volume,
                                      "volume.is_mounted").toBool());
    // if mounted, read mountpoint
    if (new_device.isMounted())
    {
        new_device.setMountpoint(getProperty(volume,
                                             "volume.mount_point").toString());
    }

    // volume filesystem type
    new_device.setVolumeFS(getProperty(volume,
                                       "volume.fstype").toString());

    // volume size
    new_device.setVolumeSize(getProperty(volume,
                                         "volume.size").toLongLong());

    // get properties for storage device
    QDBusInterface device("org.freedesktop.Hal",
                          new_device.getDeviceUDI(),
                          "org.freedesktop.Hal.Device", conn);

    new_device.setDeviceBus(getProperty(device,
                                        "storage.bus").toString());
    new_device.setRemovable(getProperty(device,
                                        "storage.removable").toBool());
    new_device.setDeviceType(getProperty(device,
                                         "storage.drive_type").toString());
    new_device.setHotpluggable(getProperty(device,
                                           "storage.hotpluggable").toBool());

    // only store portable usb based volumes
    if (new_device.isHotpluggable() &&
        new_device.isRemovable() &&
        new_device.getDeviceBus() == "usb" &&
        new_device.getDeviceType() == "disk")
    {
        push_back(new_device);
    }
    return true;
}

bool TransferDeviceList::removeVolume(const QString &udi)
{
    TransferDeviceList::iterator it;
    PredFindVolumeUDI pred;
    pred.setSearchString(udi);
    it = find_if(begin(), end(), pred);

    // not found
    if (it == end())
    {
        return false;
    }
    // remove volume
    erase(it);
    return true;
}

TransferDeviceList::const_iterator
TransferDeviceList::getDeviceByName(const QString &name) const
{
    TransferDeviceList::const_iterator it;
    PredFindName pred;
    pred.setSearchString(name);
    it = find_if(begin(), end(), pred);
    return it;
}

TransferDeviceList::const_iterator
TransferDeviceList::getDeviceByName(const std::string &name) const
{
    return getDeviceByName(QString(name.c_str()));
}

void TransferDeviceList::newDeviceDetected(const QString &udi)
{
    // add this volume/device to the list
    if (addNewVolume(udi))
    {
        // qDebug() << "TransferDeviceList : DeviceAdded : " << udi;
        emit devicesChanged();
    }
}

void TransferDeviceList::deviceRemovedDetected(const QString &udi)
{
    if (removeVolume(udi))
    {
        // qDebug() << "TransferDeviceList : DeviceRemoved : " << udi;
        emit devicesChanged();
    }
}

#else

#ifndef _WIN32
#warning "QT D-BUS support not available!"
#endif

QString TransferDevice::getVolumeSizeStr() const { return ""; }

void TransferDevice::dump() const {}

void TransferDevice::mount() throw(FWException)
{
    throw FWException("Cannot connect to the D-BUS system bus.");
}

void TransferDeviceList::init() throw(FWException)
{
    throw FWException("Cannot connect to the D-BUS system bus.");
}

TransferDeviceList::const_iterator
TransferDeviceList::getDeviceByName(const QString&) const
{
    return end();
}

TransferDeviceList::const_iterator
TransferDeviceList::getDeviceByName(const std::string&) const
{
    return end();
}

void TransferDeviceList::dump() const
{}

#endif
