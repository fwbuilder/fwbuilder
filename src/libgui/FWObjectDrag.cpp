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

#include "global.h"

#include "FWObjectDrag.h"

#include <fwbuilder/FWObject.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QWidget>

using namespace std;
using namespace libfwbuilder;



/*****************************************************************************
 *
 * Class FWObjectDrag
 *
 *****************************************************************************/

QString FWObjectDrag::FWB_MIME_TYPE="x-fwobject/pointer";

FWObjectDrag::FWObjectDrag(list<FWObject*> ol,
                           QWidget *dragSource,
                           const char* /*name*/)
    : QDrag( dragSource )
{
    objlist=ol;
    QMimeData *mime = new QMimeData;
    mime->setData(FWB_MIME_TYPE, encodedData());
    setMimeData(mime);
}

FWObjectDrag::~FWObjectDrag()
{
}

QByteArray FWObjectDrag::encodedData() const
{
    QByteArray a;

    //if (QString(mime)==FWB_MIME_TYPE)

    QDataStream stream(&a, QIODevice::WriteOnly);

    int   n   = objlist.size();
    stream << n;
    for (list<FWObject*>::const_iterator i=objlist.begin(); i!=objlist.end(); ++i)
    {
        FWObject *o = *i;
        stream.writeRawData( (const char*)(&o) , sizeof(FWObject*) );
    }

#if 0
        a.resize( sizeof(FWObject*) * objlist.size() + sizeof(int) );
        void *dst = a.data();
        int   n   = objlist.size();
        memcpy( dst, &n, sizeof(int) );
        dst += sizeof(int);
        for (list<FWObject*>::iterator i=objlist.begin(); i!=objlist.end(); ++i)
        {
            FWObject *o = *i;
            memcpy( dst, &o, sizeof(FWObject*) );
            dst += sizeof(FWObject*);
        }
#endif

    return a;
}

bool FWObjectDrag::decode( QDropEvent *ev, list<FWObject*> &ol)
{
    QByteArray rawdata = ev->mimeData()->data(FWB_MIME_TYPE);

    ol.clear();
    QDataStream stream(&rawdata, QIODevice::ReadOnly);

    int     n = 0;
    stream >> n;

    for (int i=0; i<n; i++)
    {
        FWObject *o;
        stream.readRawData( (char*)(&o) , sizeof(FWObject*) );
        ol.push_back(o);
    }

    ev->accept();
    return true;
}

bool FWObjectDrag::decode( QDragEnterEvent *ev, list<FWObject*> &ol)
{
    QByteArray rawdata = ev->mimeData()->data(FWB_MIME_TYPE);

    ol.clear();
    QDataStream stream(&rawdata, QIODevice::ReadOnly);

    int     n = 0;
    stream >> n;

    for (int i=0; i<n; i++)
    {
        FWObject *o;
        stream.readRawData( (char*)(&o) , sizeof(FWObject*) );
        ol.push_back(o);
    }

    ev->accept();
    return true;
}

Qt::DropAction FWObjectDrag::start(Qt::DropActions action)
{
    if (fwbdebug)  qDebug("FWObjectDrag::start"/*, action*/);
    return QDrag::start(action);
}
