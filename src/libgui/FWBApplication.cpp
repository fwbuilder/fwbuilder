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


#include "global.h"
#include "FWBSettings.h"
#include "FWBApplication.h"
#include "FWWindow.h"

#include "fwbuilder/FWException.h"

#include <QtDebug>
#include <QTimer>

using namespace libfwbuilder;
using namespace std;


void FWBApplication::quit()
{
    if (fwbdebug) qDebug() << "FWBApplication::quit()";
    timeout = 0;

    if (mw->isVisible()) mw->hide();

    if (st->getCheckUpdates())
    {
        QTimer::singleShot(100, this, SLOT(delayedQuit()));
    } else
        delayedQuit();
}

void FWBApplication::delayedQuit()
{
    if (fwbdebug) qDebug() << "FWBApplication::delayedQuit()";

    QApplication::quit();
}

bool FWBApplication::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    } catch (const libfwbuilder::FWException &ex)
    {
        cerr << "Caught FWException: " << ex.toString() << std::endl;
        QCoreApplication::exit(1);
    } catch (const std::string &s) {
	cerr << s << std::endl;
        QCoreApplication::exit(1);
    } catch (const std::exception &ex) {
	cerr << ex.what() << std::endl;
        QCoreApplication::exit(1);
    }
    catch (...) {
	cerr << "Caught unsupported exception" << std::endl;
        QCoreApplication::exit(1);
    }
    return false;
}
