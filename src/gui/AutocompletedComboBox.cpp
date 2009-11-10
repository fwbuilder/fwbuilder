/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  a2k@codeminders.com

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

#include "AutocompletedComboBox.h"

AutocompletedComboBox::AutocompletedComboBox(QWidget* parent)
{
    this->setParent(parent);
    model.setStringList(words);
    completer = new QCompleter(&model, this);
    this->setCompleter(this->completer);
    connect(this, SIGNAL(editTextChanged(QString)),
            this, SLOT(filterUpdate(QString)));
    this->setModel(&model);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout ()), this, SLOT(addWord()));
}

void AutocompletedComboBox::filterUpdate(QString text)
{
    timer.stop();
    timer.start(1000);
}

void AutocompletedComboBox::addWord()
{
    if (this->currentText().length() < 2) return;
    if (words.contains(this->currentText())) return;
    QString text = this->currentText();
    words << text;
    this->model.setStringList(this->words);
    this->setEditText(text);
}
