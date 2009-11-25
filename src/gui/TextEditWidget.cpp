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


#include "TextEditWidget.h"

#include <QFocusEvent>
#include <QKeyEvent>

TextEditWidget::TextEditWidget(QWidget *parent) : QTextEdit(parent)
{
    modified = false;
    connect(this, SIGNAL(undoAvailable(bool)), this, SLOT(dirty(bool)));
}

void TextEditWidget::dirty(bool f)
{
    modified = f;  // if undo is available, the widget has some changed text.
}

void TextEditWidget::focusOutEvent(QFocusEvent * event)
{
    QTextEdit::focusOutEvent(event);
    if (modified) emit textChanged(); // newTextAvailable();
}

void TextEditWidget::keyPressEvent(QKeyEvent* ev)
{
    QTextEdit::keyPressEvent(ev);
    if (ev->key()==Qt::Key_Enter && modified)  emit textChanged();
}

