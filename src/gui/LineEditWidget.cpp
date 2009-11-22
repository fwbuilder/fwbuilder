/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#include "LineEditWidget.h"

LineEditWidget::LineEditWidget(QWidget *parent) : QLineEdit(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(dataReady()));
}

/*
 * QLineEdit emits signal editingFinished when user hits Return or
 * when widget loses focus. Unfortunately it emits this signal even
 * when there were no changes in the text but widget lost focus. Check
 * if its own undo stack has anything to emit our signal
 * newTextAvailable only if the user made some changes
 */
void LineEditWidget::dataReady()
{
    if (isUndoAvailable()) emit newTextAvailable();
}

