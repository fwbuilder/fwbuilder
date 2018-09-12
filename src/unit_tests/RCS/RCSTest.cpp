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

#include "RCSTest.h"

//#include "../../global.h"
#include "../../libgui/RCS.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include <QProcess>
#include <QRegExp>
#include <QDebug>

using namespace std;
using namespace libfwbuilder;

int                fwbdebug   = 0;

QString            test_file = "zu.fwb,v";
QString            rlog_unit_test_log_file = "rlog_unit_test.log";

void RCSTest::verifyRevisions()
{
    RCS::init();

    RCS *rcs = new RCS(test_file);

    QList<Revision> rcsrevs;
    for (QList<Revision>::iterator i=rcs->begin(); i!=rcs->end(); ++i)
    {
        Revision rev = *i;
        QStringList log = rev.log.split("\n");
        log.removeFirst();
        rev.log = log.join("\n");
        rcsrevs.append(rev);
    }
    QProcess rlog;
    rlog.start("rlog", QStringList() << test_file);

    rlog.waitForFinished();

    QList<Revision> realrevs;
    QRegExp revlock("revision\\s+([\\.\\d]+)(\\s+locked by: (\\w+);)?\\n");
    revlock.setPatternSyntax(QRegExp::RegExp2);
    QRegExp dateauth("date: (\\d\\d\\d\\d/\\d\\d/\\d\\d \\d\\d\\:\\d\\d\\:\\d\\d);\\s+author\\: (\\w+);\\s+state\\: (\\w+);(\\s+lines: \\+(\\d+) \\-(\\d+))?\\n");

    QMap <QString, Revision> realrevsmap;

    QString line;

    while (!rlog.atEnd() && line != "----------------------------\n") // skip header
        line = rlog.readLine();
    while (!rlog.atEnd())
    {
        QStringList lines;
        while (!rlog.atEnd())
        {
            line = rlog.readLine();
            if (line != "----------------------------\n" &&
                line != "=============================================================================\n")
                lines.append(line);
            else
                break;
        }
        QStringList comment;
        for (int i = 2; i< lines.size(); i++)
            comment.append(lines.at(i));
        Revision rev;
        rev.log = comment.join("");
        revlock.indexIn(lines[0]);
        rev.locked_by = revlock.capturedTexts()[3];
        rev.rev = revlock.capturedTexts()[1];
        dateauth.indexIn(lines[1]);
        rev.date = dateauth.capturedTexts()[1].replace("/", "-");
        rev.author = dateauth.capturedTexts()[2];
        rev.filename = test_file;

        realrevs.insert(0, rev);
        realrevsmap[rev.rev] = rev;
    }

    CPPUNIT_ASSERT(realrevs.size() == rcsrevs.size());

    for (int i = 0; i < realrevs.size(); i++)
    {
        Revision rcsr = rcsrevs.at(i);
        Revision realr = realrevsmap[rcsr.rev];
/*
        qDebug() << realr.author << rcsr.author;
        qDebug() << realr.date << rcsr.date;
        qDebug() << realr.filename << rcsr.filename;
        qDebug() << realr.locked_by << rcsr.locked_by;
        qDebug() << realr.log << rcsr.log;
        qDebug() << realr.rev << rcsr.rev;
        qDebug() << "----------";
        qDebug() << (realr == rcsr);
*/
        CPPUNIT_ASSERT (realr == rcsr);
    }

}
