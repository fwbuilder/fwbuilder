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
#include <QDebug>

TextEditWidget::TextEditWidget(QWidget *parent) : QTextEdit(parent)
{
    modified = false;
    showingDefault = false;
    connect(this, SIGNAL(undoAvailable(bool)), this, SLOT(dirty(bool)));
}

void TextEditWidget::dirty(bool f)
{
    modified = f;  // if undo is available, the widget has some changed text.
}

void TextEditWidget::focusInEvent(QFocusEvent * event)
{
    hasFocus = true;
    QTextEdit::focusInEvent(event);
    if (showingDefault && !isReadOnly()) {
        clear();
        showingDefault = false;
    }
}

void TextEditWidget::focusOutEvent(QFocusEvent * event)
{
    hasFocus = false;
    QTextEdit::focusOutEvent(event);
    if (modified) emit textChanged(); // newTextAvailable();
    if (toPlainText().isEmpty()) {
        showingDefault = true;
        setText(defaultText);
    }
}

void TextEditWidget::setTextDefault(const QString &text,
                                    const QString &theDefault)
{
    /* Sometimes the text area gets left in italics mode. */
    setFontItalic(false);
    defaultText = theDefault;
    if (text.isEmpty() && !hasFocus) {
        setText(theDefault);
        showingDefault = true;
    } else {
        /* We can get a setTextDefault call when the object is being
           reloaded, but after focus has been set on the text edit
           field.  If it's the same text we're setting (which it
           should be), we skip setting it so that we don't lose the
           proper place where the user clicked. */
        if (!hasFocus || getText() != text) {
            setText(text);
        }
        showingDefault = false;
    }
}


QString TextEditWidget::getText()
{
    if (!modified && showingDefault) return "";
    return toPlainText();
}
