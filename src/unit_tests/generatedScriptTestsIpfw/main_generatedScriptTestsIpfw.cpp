/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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
#include <QTest>

#include "generatedScriptTestsIpfw.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"

#include <QApplication>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    #include <QTextCodec>
#endif

#include "common/init.cpp"

using namespace std;
using namespace libfwbuilder;


int main(int argc, char **argv)
{
    QApplication app(argc, argv, false);

    // compilers always write file names into manifest in Utf8
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));
#endif

    init(argv);

    Resources res(Constants::getResourcesFilePath());

    return QTest::qExec(new GeneratedScriptTest());
}
