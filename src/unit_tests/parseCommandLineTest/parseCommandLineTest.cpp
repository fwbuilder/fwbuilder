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

#include <QTest>
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

    QVERIFY(argv[0] == "/usr/local/bin/ssh");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "identity.key");
    QVERIFY(argv[3] == "-o");
    QVERIFY(argv[4] == "arg1=val1");
    QVERIFY(argv[5] == "-o");
    QVERIFY(argv[6] == "arg2");
    QVERIFY(argv[7] == "val2");
    QVERIFY(argv[8] == "foo");
    QVERIFY(argv[9] == "bar");

    argv.clear();
    parseCommandLine(
        "/usr/local/bin/ssh   -i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "/usr/local/bin/ssh");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "identity.key");
    QVERIFY(argv[3] == "-o");
    QVERIFY(argv[4] == "arg1=val1");
    QVERIFY(argv[5] == "-o");
    QVERIFY(argv[6] == "arg2");
    QVERIFY(argv[7] == "val2");
    QVERIFY(argv[8] == "foo");
    QVERIFY(argv[9] == "bar");



    argv.clear();
    parseCommandLine(
        "-i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "-i");
    QVERIFY(argv[1] == "identity.key");
    QVERIFY(argv[2] == "-o");
    QVERIFY(argv[3] == "arg1=val1");
    QVERIFY(argv[4] == "-o");
    QVERIFY(argv[5] == "arg2");
    QVERIFY(argv[6] == "val2");
    QVERIFY(argv[7] == "foo");
    QVERIFY(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        " -i   identity.key -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "-i");
    QVERIFY(argv[1] == "identity.key");
    QVERIFY(argv[2] == "-o");
    QVERIFY(argv[3] == "arg1=val1");
    QVERIFY(argv[4] == "-o");
    QVERIFY(argv[5] == "arg2");
    QVERIFY(argv[6] == "val2");
    QVERIFY(argv[7] == "foo");
    QVERIFY(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        "-i   \"identity.key\" -o arg1=val1 -o arg2 val2 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "-i");
    QVERIFY(argv[1] == "identity.key");
    QVERIFY(argv[2] == "-o");
    QVERIFY(argv[3] == "arg1=val1");
    QVERIFY(argv[4] == "-o");
    QVERIFY(argv[5] == "arg2");
    QVERIFY(argv[6] == "val2");
    QVERIFY(argv[7] == "foo");
    QVERIFY(argv[8] == "bar");



    argv.clear();
    parseCommandLine(
        "/usr/local/bin/program -arg1 \"val1 'val2 val3' val4\" -o arg1=val1 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "/usr/local/bin/program");
    QVERIFY(argv[1] == "-arg1");
    QVERIFY(argv[2] == "val1 'val2 val3' val4");
    QVERIFY(argv[3] == "-o");
    QVERIFY(argv[4] == "arg1=val1");
    QVERIFY(argv[5] == "foo");
    QVERIFY(argv[6] == "bar");



    argv.clear();
    parseCommandLine(
        "/usr/local/bin/program -arg1 \"val1 'val2 \"val3 val4\" val5' val6\" -o arg1=val1 foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "/usr/local/bin/program");
    QVERIFY(argv[1] == "-arg1");
    QVERIFY(argv[2] == "val1 'val2 \"val3 val4\" val5' val6");
    QVERIFY(argv[3] == "-o");
    QVERIFY(argv[4] == "arg1=val1");
    QVERIFY(argv[5] == "foo");
    QVERIFY(argv[6] == "bar");



    argv.clear();
    parseCommandLine(
        "c:\\putty\\plink.exe -i identity.key -q foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "c:\\putty\\plink.exe");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "identity.key");
    QVERIFY(argv[3] == "-q");
    QVERIFY(argv[4] == "foo");
    QVERIFY(argv[5] == "bar");

    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i identity.key -q foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "c:\\Program Files\\plink.exe");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "identity.key");
    QVERIFY(argv[3] == "-q");
    QVERIFY(argv[4] == "foo");
    QVERIFY(argv[5] == "bar");


    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i \"c:\\Documents and Settings\\firewall\\identity.key\" -q foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "c:\\Program Files\\plink.exe");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "c:\\Documents and Settings\\firewall\\identity.key");
    QVERIFY(argv[3] == "-q");
    QVERIFY(argv[4] == "foo");
    QVERIFY(argv[5] == "bar");


    argv.clear();
    parseCommandLine(
        "c:\\Program Files\\plink.exe -i 'c:\\Documents and Settings\\firewall\\identity.key' -q foo bar",
        argv);
    qDebug() << argv;

    QVERIFY(argv[0] == "c:\\Program Files\\plink.exe");
    QVERIFY(argv[1] == "-i");
    QVERIFY(argv[2] == "c:\\Documents and Settings\\firewall\\identity.key");
    QVERIFY(argv[3] == "-q");
    QVERIFY(argv[4] == "foo");
    QVERIFY(argv[5] == "bar");


}
