/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: rcs_test.cpp 1445 2007-11-20 17:41:22Z alek $

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


#include "config.h"
#include "global.h"
#include "../../gui/RCS.h"

#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QWidget>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

QApplication      *app        = NULL;
int                fwbdebug   = 0;

QString            test_file = "zu.fwb";
QString            rlog_unit_test_log_file = "rlog_unit_test.log";

int main( int argc, char ** argv )
{
    app = new QApplication( argc, argv );
    QWidget *w = new QWidget;
    //app->setMainWidget(w);
    w->show();

    RCS *rcs = new RCS(test_file);

    QString reverse_engineered_rlog;

    QList<Revision>::iterator i;
    for (i=rcs->begin(); i!=rcs->end(); ++i)
    {
        reverse_engineered_rlog += "---------------------------------\n";
        reverse_engineered_rlog += "revision: " + (*i).rev + "\n";
        reverse_engineered_rlog += "date: " + (*i).date + "\n";
        reverse_engineered_rlog += "author: " + (*i).author + "\n";
        reverse_engineered_rlog += "locked by: " + (*i).locked_by + "\n";
        reverse_engineered_rlog += "log: " + (*i).log + "\n";
    }

    QFile rlog_test_file(rlog_unit_test_log_file);
    if (rlog_test_file.open( QIODevice::ReadOnly ))
    {
        QTextStream strm( &rlog_test_file );
        QString test_str = strm.readAll();
        rlog_test_file.close();

        if (test_str != reverse_engineered_rlog)
        {
            cout << "Test failed. Generated log (incorrect):" << endl;
            cout << reverse_engineered_rlog.toAscii().constData();
//            cerr << "#############################################" << endl;
//            cerr << test_str << endl;
            return(2);
        }

    } else
    {
        cout << "Could not open test file " << rlog_unit_test_log_file.toAscii().constData() << endl;
        return(2);
    }

    return(0);
}
