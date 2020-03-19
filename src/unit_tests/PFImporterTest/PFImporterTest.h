/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef PFIMPORTERTEST_H
#define PFIMPORTERTEST_H

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Logger.h"

#include <map>
#include <fstream>
#include <iostream>

#include <QObject>
#include <QString>


class PFImporterTest : public QObject
{
    Q_OBJECT

    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::Library *lib;
    libfwbuilder::QueueLogger *logger;
    int predictable_id_tracker;
    std::map<std::string, std::string> id_mapping;

    void compareResults(libfwbuilder::QueueLogger* logger,
                        QString expected_result_file_name,
                        QString obtained_result_file_name);
    void compareFwbFiles(QString expected_result_file_name,
                         QString obtained_result_file_name);

    std::string openTestFile(const QString &file_name);

private slots:
    void init();

    void macrosTest();
    void hostsMatchTest();
    void blockReturnTest();
    void icmpMatchTest();
    void interfaceMatchTest();
    void portMatchTest();
    void setCommandsTest();
    void stateMatchTest();
    void tcpFlagsMatchTest();
    void natCommands();
    void rdrCommands();
    void setTimeoutCommands();
    void scrubCommandsOld();
    void scrubCommandsNew();
    void tableDefinitions();
    void userGroupMatches();
    void routeToTest();
    void routeTo47Test();

};

#endif // PFIMPORTERTEST_H
