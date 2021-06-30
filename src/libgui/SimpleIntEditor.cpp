/*

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/



#include "global.h"

#include "SimpleIntEditor.h"
#include "FWBSettings.h"

#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <fstream>

using namespace std;

SimpleIntEditor::SimpleIntEditor(int minValue, int maxValue, int value, const QString &title): QDialog()
{
    m_dialog = new Ui::SimpleIntEditor_q;
    m_dialog->setupUi(static_cast<QDialog*>(this));

    if (!title.isEmpty()) setWindowTitle(title);

    m_dialog->spin_box->setMinimum( minValue);
    m_dialog->spin_box->setMaximum( maxValue);
    m_dialog->spin_box->setValue( value);
}

int SimpleIntEditor::value()
{
    return m_dialog->spin_box->value();
}

