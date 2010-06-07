/*
 * TransferDevice.cpp - QDBus based config transfer library
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

#ifndef __TRANSFER_DEVICE_HH__
#define __TRANSFER_DEVICE_HH__

#include <string>
#include <QString>
#include <QObject>

#include "../../config.h"
#include "fwbuilder/FWException.h"

namespace fwtransfer
{

    class DeviceObserver;

    /**
     * @class TransferDevice
     *
     * @brief This class represents a device for firewall config transfer.
     *
     * The TransferDevice class can be used to store information about transfer
     * volumes/devices. TransferDevices provide different setter/getter functions
     * to set/get information about devices and volumes. TransferDevice objects
     * provides a dump() function to dump the currently stored info and also a
     * mount() function to actually mount the volume.
     */
    class TransferDevice
    {
    public:
        TransferDevice() :
                volume_udi(QString()),
                volume_fs(QString()),
                volume_size(0),
                device_udi(QString()),
                device_name(QString()),
                device_bus(QString()),
                device_type(QString()),
                mountpoint(QString()),
                is_mounted(false),
                is_removable(false),
                is_hotpluggable(false) {};

        /**
         * set volume UDI for transfer device.
         *
         * @param udi volume UDI
         */
        void setVolumeUDI(QString udi) { volume_udi = udi; };

        /**
         * get volume UDI for transfer device.
         *
         * @return volume UDI
         */
        QString getVolumeUDI() const { return volume_udi; };

        /**
         * set filesystem type for transfer device (e.g. "ext3").
         *
         * @param fs filesystem type
         */
        void setVolumeFS(QString fs) { volume_fs = fs; };

        /**
         * get filesystem type of transfer device.
         *
         * @return filesystem type
         */
        QString getVolumeFS() const { return volume_fs; };

        /**
         * set volume size for transfer device.
         *
         * @param fs filesystem type
         */
        void setVolumeSize(qlonglong size) { volume_size = size; };

        /**
         * get volume size for volume of transfer device as qlonglong.
         *
         * @return filesystem size in bytes
         */
        qlonglong getVolumeSize() const { return volume_size; };

        /**
         * get volume size for volume of transfer device as QString.
         *
         * @return filesystem size in human readable string format
         */
        QString getVolumeSizeStr() const;

        /**
         * set device UDI for transfer device.
         *
         * @param udi device UDI
         */
        void setDeviceUDI(QString udi) { device_udi = udi; };

        /**
         * get device UDI for transfer device.
         *
         * @return device UDI
         */
        QString getDeviceUDI() const { return device_udi; };

        /**
         * set device name for transfer device (e.g. /dev/sdc1).
         *
         * @param name device name
         */
        void setDeviceName(QString name) { device_name = name; };

        /**
         * get device name of transfer device.
         *
         * @return device name
         */
        QString getDeviceName() const { return device_name; };

        /**
         * set bus type used by transfer device (e.g. "usb").
         *
         * @param bus type of bus device is attached to (pci, usb, ...)
         */
        void setDeviceBus(QString bus) { device_bus = bus; };

        /**
         * get bus type of transfer device.
         *
         * @return bus type
         */
        QString getDeviceBus() const { return device_bus; };

        /**
         * set transfer device type (e.g. "disk").
         *
         * @param type transfer device storage type
         */
        void setDeviceType(QString type) { device_type = type; };

        /**
         * get storage type of this transfer device.
         *
         * @return storage device type
         */
        QString getDeviceType() const { return device_type; };

        /**
         * set mount point for transfer device (e.g. "/media/disk").
         *
         * @param path mount point of transfer device
         */
        void setMountpoint(QString path) { mountpoint = path; };

        /**
         * get mount point path of transfer device.
         *
         * @return mount point path
         */
        QString getMountpoint() const { return mountpoint; };

        /**
         * set mounted flag to true or false depending on whether transfer
         * device volume is mounted or not.
         *
         * @param flag mounted status of volume
         */
        void setMounted(bool flag) { is_mounted = flag; };

        /**
         * determines whether transfer device volume is already mounted or not.
         *
         * @return true if mounted, false if not
         */
        bool isMounted() const { return is_mounted; };

        /**
         * set removable flag to true or false depending on whether device can
         * be removed or not.
         *
         * @param flag removable status flag of device
         */
        void setRemovable(bool flag) { is_removable = flag; };

        /**
         * determines whether device can be removed or not.
         *
         * @return true if portable, false if not
         */
        bool isRemovable() const { return is_removable; };

        /**
         * set hotpluggable flag to true or false depending on whether device is
         * hotpluggable.
         *
         * @param flag hotpluggable status flag of device
         */
        void setHotpluggable(bool flag) { is_hotpluggable = flag; };

        /**
         * determines whether device is hotpluggable.
         *
         * @return true if yes, false if not
         */
        bool isHotpluggable() const { return is_hotpluggable; };

        /**
         * debug function to dump transfer device settings.
         */
        void dump() const;

        /**
         * mount transfer device
         *
         * @throw libfwbuilder::FWException could not mount exception
         */
        void mount() throw(libfwbuilder::FWException);

    private:
        QString   volume_udi;
        QString   volume_fs;
        qlonglong volume_size;
        QString   device_udi;
        QString   device_name;
        QString   device_bus;
        QString   device_type;
        QString   mountpoint;
        bool      is_mounted;
        bool      is_removable;
        bool      is_hotpluggable;
    };

    /**
     * @class TransferDeviceList
     *
     * @brief An TransferDeviceList is used to manage transfer devices of a system.
     *
     * An TransferDeviceList stores all available transfer volumes. It also
     * provides a devicesChanged signal which can be used to track list changes.
     * This signal is emitted when HAL detects a new device (DeviceAdded) or
     * a device has vanished (DeviceRemoved). Users of an TransferDeviceList
     * object can connect a slot to this signal to react to this event.
     */
    class TransferDeviceList : public QObject, public std::list<TransferDevice>
    {
        Q_OBJECT

    public:
        /** TransferDeviceList ctor */
        TransferDeviceList(QObject * parent = 0) : QObject(parent) {};

        /** TransferDeviceList dtor */
        virtual ~TransferDeviceList() {};

        /**
         * init list of volumes/devices. only usb based, portable volumes
         * are added to the list. Previously added transfer devices will be
         * cleared from the list before adding new ones.
         *
         * @throw libfwbuilder::FWException DBus not available exception
         */
        void init() throw(libfwbuilder::FWException);

        /**
         * return specific TransferDevice identified by volume name.
         *
         * @param volumeid id of volume as QString (e.g. /dev/sdc1)
         * @return iterator pointing to requested TransferDevice
         *
         */
        TransferDeviceList::const_iterator
        getDeviceByName(const QString &name) const;

        /**
         * return specific TransferDevice identified by volume name.
         *
         * @param volumeid id of volume as string (e.g. /dev/sdc1)
         * @return iterator pointing to requested TransferDevice
         *
         */
        TransferDeviceList::const_iterator
        getDeviceByName(const std::string &name) const;

        /**
         * dump data of all managed transfer devices.
         */
        void dump() const;

    private:

#ifdef HAVE_QTDBUS
        /**
         * check DBus availability.
         */
        bool checkDBus() const;

        /**
         * add a new transfer volume with given volume UDI.
         *
         * @param udi UDI of the volume to add.
         * @return true if successfully added, false if not
         */
        bool addNewVolume(const QString &udi);

        /**
         * remove an existing volume identified by UDI from the list.
         *
         * @param udi UDI of the volume to add.
         * @return true if successfully removed, false if not found
         */
        bool removeVolume(const QString &udi);

    private slots:
        void newDeviceDetected(const QString &udi);
        void deviceRemovedDetected(const QString &udi);

    signals:
        void devicesChanged();

#endif
    };

    /**
     * @class PredFindName
     *
     * @brief Predicate class to find device name in TransferDevice.
     *
     * PredFindName can be used e.g. as parameter in find_if() function to
     * compare TransferDevices in TransferDeviceList with a given device name
     * specified by setSearchString().
     */
    class PredFindName
    {
    protected:
        QString search_string;
    public:
        PredFindName() {};

        bool operator()(const TransferDevice dev) const
        {
            return (dev.getDeviceName() == search_string);
        }

        void setSearchString(const QString &string)
        {
            search_string = string;
        }
    };

    /**
     * @class PredFindVolumeUDI
     *
     * @brief Predicate class to find volume UDI in TransferDevice.
     *
     * PredFindVolumeUDI can be used e.g. as parameter in find_if() function to
     * compare TransferDevices in TransferDeviceList with a given volume UDI
     * specified by setSearchString().
     */
    class PredFindVolumeUDI
    {
    protected:
        QString search_string;
    public:
        PredFindVolumeUDI() {};

        bool operator()(const TransferDevice dev) const
        {
            return (dev.getVolumeUDI() == search_string);
        }

        void setSearchString(const QString &string)
        {
            search_string = string;
        }
    };
}

#endif /* __TRANSFER_DEVICE_HH__ */

