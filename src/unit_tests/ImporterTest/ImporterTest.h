/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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

#ifndef IMPORTERTEST_H
#define IMPORTERTEST_H

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Logger.h"

#include <map>

#include <QObject>
#include <QString>


class ImporterTest : public QObject
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

private slots:
    void init();
    void IOSImporterTest();
    void IPTImporterTest();
    void IPTImporterNoNatTest();
    void IPTImporterParseVersionsTest();

};

#endif // IMPORTERTEST_H
