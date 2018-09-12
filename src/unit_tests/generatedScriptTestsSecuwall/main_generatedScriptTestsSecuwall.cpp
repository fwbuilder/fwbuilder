/*
 * generatedScriptTestsSecuwall.h - secuwall unit test runner
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


#include "generatedScriptTestsSecuwall.h"

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>

#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"

#include <QApplication>
#include <QTextCodec>

#include "../../../common/init.cpp"

using namespace std;
using namespace libfwbuilder;

int main(int argc, char **argv)
{
    QApplication app(argc, argv, false);

    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));

    init(argv);

    Resources res(Constants::getResourcesFilePath());


    CppUnit::TextUi::TestRunner runner;
    runner.addTest( GeneratedScriptTest::suite() );
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                         std::cerr ) );

    runner.run();
}
