/*
 * generatedScriptTestsSecuwall.h - secuwall unit tests
 *
 * Copyright (c) 2010 secunet Security Networks AG
 * Copyright (c) 2010 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2010 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef GENERATEDSCRIPTTESTS_SECUWALL_H
#define GENERATEDSCRIPTTESTS_SECUWALL_H

#include <QObject>

#include "fwbuilder/FWObjectDatabase.h"

class GeneratedScriptTest : public QObject
{
    Q_OBJECT

    libfwbuilder::FWObjectDatabase *objdb;

    void loadDataFile(const std::string &file_name);
    void runCompiler(const std::string &test_file,
                     const std::string &firewall_object_name,
                     const std::string &generate_file_name);
    void assertDirsEqual(const std::string &left_dir,
                         const std::string &right_dir);
    void assertFilesEqual(const std::string &left_filename,
                          const std::string &right_filename);

private slots:
    void init();
    void cleanup();
    void FilesGenerationTest();

};

#endif // GENERATEDSCRIPTTESTS_SECUWALL_H
