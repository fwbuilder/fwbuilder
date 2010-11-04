/*
 * transfer_secuwall.cpp - secunet wall config transfer agent
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

#include "../../../config.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#ifdef _WIN32
#include <getopt.h>
#else
//#include <stdlib.h>
#endif
#endif

#include "fwbuilder/FWException.h"

#include "fwtransfer/TransferDevice.h"

// tarball base name and suffix (e.g. config.tar.gz)
#define BASENAME "config"
#define SUFFIX   "tar"

using namespace std;
using namespace fwtransfer;
using namespace libfwbuilder;

int fwbdebug = 0;

// object database filename: not used at the moment
static string filename     = "";
// workdir of firewall to export
static string workdir      = "";
// templates dir to use
static string tmpldir      = "";
// volumeid (e.g. /dev/sdc1) to use as transfer partition
static string volumeid     = "";
// object name of firewall to export (e.g. fw1)
static string fwobjectname = "";

// append fwobjectname to transfer tarball
static bool appendname     = false;

/**
 * init portable usb device list.
 *
 * @param devices device list to init, only portable devices are valid.
 * @return true if successful, false if not
 */
bool init_usbdisks(TransferDeviceList &devices)
{
    try
    {
        devices.init();
    }
    catch (FWException &ex)
    {
        cout << "Could not init list of usbdisks!" << endl;
        cout << "Error: " << ex.toString() << endl;
        return false;
    }

    TransferDeviceList::const_iterator it;
    it = devices.begin();
    if (it == devices.end())
    {
        cout << "No usable transfer volumes found! " << endl;
        return false;
    }

    return true;
}

/**
 * display transfer_secuwall usage.
 */
void usage(const char *name)
{
    cout << "Firewall Builder: config transfer agent for "
        "secunet wall host OS" << endl;
    cout << ("Version : ") << VERSION << endl;
    cout << ("Usage   : ") << name <<
        " [-l] [-h] [-n] -v volumeid [-f filename.xml] [-d workdir] "
        "[-a templatedir] firewall_object_name" << endl;
}

/**
 * list all available transfer volumes.
 */
void list_volumes()
{
    TransferDeviceList devices;

    if (!init_usbdisks(devices))
    {
        exit(EXIT_FAILURE);
    }

    TransferDeviceList::const_iterator it;
    it = devices.begin();
    cout << endl;
    for (; it != devices.end(); it++)
    {
        TransferDevice dev = *it;
        dev.dump();
    }

    exit(EXIT_SUCCESS);
}

/**
 * copy file specified by src to dst
 *
 * @param src source filepath
 * @param dst destination filepath
 * @return true if successfully copied, false if not
 */
bool copy_file(const string &src, const string &dst)
{
    std::ifstream infile(src.c_str(), std::ios_base::binary);
    std::ofstream outfile(dst.c_str(), std::ios_base::binary);

    if (!infile)
    {
        cout << "File not found: " << src << endl;
        return false;
    }

    if (!outfile)
    {
        cout << "Could not create: " << dst << endl;
        return false;
    }

    try
    {
        outfile << infile.rdbuf();
    }
    catch (...)
    {
        cout << "Error while writing to: " << dst << endl;
        infile.close();
        outfile.close();
        return false;
    }
    infile.close();
    outfile.close();
    return true;
}

/**
 * secunet wall config transfer agent
 *
 * used to export fw config tarball to a portable usb medium.
 */
int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int opt;

    while ((opt = getopt(argc, argv, "lhnv:f:d:a:")) != EOF)
    {
        switch (opt)
        {
        case 'd':
            workdir = string(optarg);
            break;
        case 'a':
            tmpldir = string(optarg);
            break;
        case 'f':
            filename = string(optarg);
            break;
        case 'v':
            volumeid = string(optarg);
            break;
        case 'l':
            list_volumes();
            break;
        case 'h':
            usage(argv[0]);
            exit(EXIT_FAILURE);
        case 'n':
            appendname = true;
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if ((argc - 1) != optind)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    fwobjectname = string(argv[optind++]);

    if (workdir.empty())
    {
        workdir = "./";
    }

    if (filename.empty())
    {
        filename = workdir + fwobjectname + ".fwb";
    }

    if (fwbdebug)
    {
        cout << "Volume\t: "  << volumeid << endl;
        cout << "Object\t: "  << fwobjectname << endl;
        cout << "Workdir\t: " << workdir << endl;
        cout << "DB\t: "      << filename << endl;
    }

    if (
#ifdef _WIN32
        _chdir(workdir.c_str())
#else
        chdir(workdir.c_str())
#endif
    )
    {
        cout << "Can't change directory to: " << workdir << endl;
        exit(EXIT_FAILURE);
    }

    // check for existence of fwobjectname subdir
    struct stat buffer;
    if (stat(fwobjectname.c_str(), &buffer))
    {
        cout << "Config subdir not found for " << fwobjectname << endl;
        exit(EXIT_FAILURE);
    }

    // check templates directory (with -a flag only)
    if (!tmpldir.empty())
    {
        if (stat(tmpldir.c_str(), &buffer))
        {
            cout << "Templates directory '" << tmpldir << "' not found" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // try to mount volume
    TransferDeviceList devices;
    if (!init_usbdisks(devices))
    {
        exit(EXIT_FAILURE);
    }

    TransferDeviceList::const_iterator it;
    it = devices.getDeviceByName(volumeid);
    if (it == devices.end())
    {
        cout << "Could not find volume " << volumeid << endl;
        exit(EXIT_FAILURE);
    }
    TransferDevice dev = *it;

    if (!dev.isMounted())
    {
        try
        {
            dev.mount();
        }
        catch (FWException &ex)
        {
            cout << "Could not mount volume " << volumeid << endl;
            cout << "Error: " << ex.toString() << endl;
            exit(EXIT_FAILURE);
        }
    }
    string mountpoint = dev.getMountpoint().toStdString();
    cout << "Device mounted to " << mountpoint << endl;

    // copy firewall script to fwobjectname subdir, we assume the script name
    // to be: 'fwobjectname + fw suffix'. the file will be copied to 'sysconfig'
    // subdirectory.
    string fwscriptname = fwobjectname + ".fw";
    string fwscriptdst = fwobjectname + "/sysconfig/" + fwscriptname;
    if (!copy_file(fwscriptname, fwscriptdst))
    {
        cout << "Firewall config script file not found for '"
             << fwobjectname << "'" << endl;
        exit(EXIT_FAILURE);
    }

    // construct tarball name depending on appendname flag
    string tarball;
    if (appendname)
    {
        tarball = string(BASENAME) + "-" + fwobjectname + "." + string(SUFFIX);
    }
    else
    {
        tarball = string(BASENAME) + "." + string(SUFFIX);
    }

    // tell tar to change into fwobjectname subdir and create tarball
    // force file ownership to root:root
    string cmd = "tar cCf " + fwobjectname + " " + tarball + " --owner=0 --group=0 .";
    if (system(cmd.c_str()) != 0)
    {
        cout << "Could not create tarball '" << tarball << "'" << endl;
        exit(EXIT_FAILURE);
    }

    // append templates to tarball
    if (!tmpldir.empty())
    {
        cout << "Adding templates from '" << tmpldir << "'" << endl;
        cmd = "tar rCf " + tmpldir + " " + tarball + " --owner=0 --group=0 .";
        if (system(cmd.c_str()) != 0)
        {
            cout << "Could not append templates to tarball '" << tarball
                 << "'" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // compress tarball
    cmd = "gzip -f " + tarball;
    if (system(cmd.c_str()) != 0)
    {
        cout << "Could not compress tarball '" << tarball << "'" << endl;
        cout << "Missing gzip binary?" << endl;
        exit(EXIT_FAILURE);
    }

    // copy compressed archive to volume
    string compressed = tarball + ".gz";
    string outpath = mountpoint + "/" + compressed;
    if (!copy_file(compressed, outpath))
    {
        cout << "Could not copy '" << compressed << "' to '"
             << outpath << "'" << endl;
        exit(EXIT_FAILURE);
    }

#ifndef _WIN32
    sync();
#endif

    cout << "Exported '" << fwobjectname << "' config to " << outpath << endl;
    exit(EXIT_SUCCESS);
}
