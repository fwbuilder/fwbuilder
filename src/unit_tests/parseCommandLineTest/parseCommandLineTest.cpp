/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "parseCommandLineTest.h"

#include "utils.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include <QProcess>
#include <QRegExp>
#include <QtDebug>

using namespace std;
using namespace libfwbuilder;

void parseCommandLineTest::parseCommandLines()
{
    QStringList argv;

    parseCommandLine(
        "/usr/local/bin/ssh -i identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "/usr/local/bin/ssh");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "identity.key");
    CPPUNIT_ASSERT(argv[3] == "-o");
    CPPUNIT_ASSERT(argv[4] == "arg1=val1");
    CPPUNIT_ASSERT(argv[5] == "-o");
    CPPUNIT_ASSERT(argv[6] == "arg2");
    CPPUNIT_ASSERT(argv[7] == "val2");
    CPPUNIT_ASSERT(argv[8] == "foo");
    CPPUNIT_ASSERT(argv[9] == "bar");

    argv.clear();
    parseCommandLine(
        "/usr/local/bin/ssh   -i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "/usr/local/bin/ssh");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "identity.key");
    CPPUNIT_ASSERT(argv[3] == "-o");
    CPPUNIT_ASSERT(argv[4] == "arg1=val1");
    CPPUNIT_ASSERT(argv[5] == "-o");
    CPPUNIT_ASSERT(argv[6] == "arg2");
    CPPUNIT_ASSERT(argv[7] == "val2");
    CPPUNIT_ASSERT(argv[8] == "foo");
    CPPUNIT_ASSERT(argv[9] == "bar");



    argv.clear();
    parseCommandLine(
        "-i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "-i");
    CPPUNIT_ASSERT(argv[1] == "identity.key");
    CPPUNIT_ASSERT(argv[2] == "-o");
    CPPUNIT_ASSERT(argv[3] == "arg1=val1");
    CPPUNIT_ASSERT(argv[4] == "-o");
    CPPUNIT_ASSERT(argv[5] == "arg2");
    CPPUNIT_ASSERT(argv[6] == "val2");
    CPPUNIT_ASSERT(argv[7] == "foo");
    CPPUNIT_ASSERT(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        " -i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "-i");
    CPPUNIT_ASSERT(argv[1] == "identity.key");
    CPPUNIT_ASSERT(argv[2] == "-o");
    CPPUNIT_ASSERT(argv[3] == "arg1=val1");
    CPPUNIT_ASSERT(argv[4] == "-o");
    CPPUNIT_ASSERT(argv[5] == "arg2");
    CPPUNIT_ASSERT(argv[6] == "val2");
    CPPUNIT_ASSERT(argv[7] == "foo");
    CPPUNIT_ASSERT(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        "-i   \"identity.key\" -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "-i");
    CPPUNIT_ASSERT(argv[1] == "identity.key");
    CPPUNIT_ASSERT(argv[2] == "-o");
    CPPUNIT_ASSERT(argv[3] == "arg1=val1");
    CPPUNIT_ASSERT(argv[4] == "-o");
    CPPUNIT_ASSERT(argv[5] == "arg2");
    CPPUNIT_ASSERT(argv[6] == "val2");
    CPPUNIT_ASSERT(argv[7] == "foo");
    CPPUNIT_ASSERT(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        "/usr/local/bin/program -arg1 \"val1 'val2 val3' val4\" -o arg1=val1 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "/usr/local/bin/program");
    CPPUNIT_ASSERT(argv[1] == "-arg1");
    CPPUNIT_ASSERT(argv[2] == "val1 'val2 val3' val4");
    CPPUNIT_ASSERT(argv[3] == "-o");
    CPPUNIT_ASSERT(argv[4] == "arg1=val1");
    CPPUNIT_ASSERT(argv[5] == "foo");
    CPPUNIT_ASSERT(argv[6] == "bar");



    argv.clear();
    parseCommandLine(
        "/usr/local/bin/program -arg1 \"val1 'val2 \"val3 val4\" val5' val6\" -o arg1=val1 foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "/usr/local/bin/program");
    CPPUNIT_ASSERT(argv[1] == "-arg1");
    CPPUNIT_ASSERT(argv[2] == "val1 'val2 \"val3 val4\" val5' val6");
    CPPUNIT_ASSERT(argv[3] == "-o");
    CPPUNIT_ASSERT(argv[4] == "arg1=val1");
    CPPUNIT_ASSERT(argv[5] == "foo");
    CPPUNIT_ASSERT(argv[6] == "bar");



    argv.clear();
    parseCommandLine(
        "c:\\putty\\plink.exe -i identity.key -q foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "c:\\putty\\plink.exe");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "identity.key");
    CPPUNIT_ASSERT(argv[3] == "-q");
    CPPUNIT_ASSERT(argv[4] == "foo");
    CPPUNIT_ASSERT(argv[5] == "bar");

    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i identity.key -q foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "c:\\Program Files\\plink.exe");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "identity.key");
    CPPUNIT_ASSERT(argv[3] == "-q");
    CPPUNIT_ASSERT(argv[4] == "foo");
    CPPUNIT_ASSERT(argv[5] == "bar");


    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i \"c:\\Documents and Settings\\firewall\\identity.key\" -q foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "c:\\Program Files\\plink.exe");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "c:\\Documents and Settings\\firewall\\identity.key");
    CPPUNIT_ASSERT(argv[3] == "-q");
    CPPUNIT_ASSERT(argv[4] == "foo");
    CPPUNIT_ASSERT(argv[5] == "bar");


    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i 'c:\\Documents and Settings\\firewall\\identity.key' -q foo bar",
        argv);
    qDebug() << argv;

    CPPUNIT_ASSERT(argv[0] == "c:\\Program Files\\plink.exe");
    CPPUNIT_ASSERT(argv[1] == "-i");
    CPPUNIT_ASSERT(argv[2] == "c:\\Documents and Settings\\firewall\\identity.key");
    CPPUNIT_ASSERT(argv[3] == "-q");
    CPPUNIT_ASSERT(argv[4] == "foo");
    CPPUNIT_ASSERT(argv[5] == "bar");


}
