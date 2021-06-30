/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "FWObjectDropArea.h"
#include "BaseObjectDialog.h"
#include "CommentKeywords.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>

void BaseObjectDialog::connectSignalsOfAllWidgetsToSlotChange()
{
    QList<QWidget*> all_dialog_widgets = findChildren<QWidget*>(QString());
    foreach(QWidget* w, all_dialog_widgets)
    {
        FWObjectDropArea *da = dynamic_cast<FWObjectDropArea*>(w);
        if (da)
        {
            connect(da, SIGNAL(objectDeleted()), this, SLOT(changed()));
            connect(da, SIGNAL(objectInserted()), this, SLOT(changed()));
            continue;
        }

        QLineEdit *le = qobject_cast<QLineEdit*>(w);
        if (le)
        {
            connect(le, SIGNAL(editingFinished()), this, SLOT(changed()));
            continue;
        }

        QCheckBox *cb = qobject_cast<QCheckBox*>(w);
        if (cb)
        {
            connect(cb, SIGNAL(stateChanged(int)), this, SLOT(changed()));
            continue;
        }

        QRadioButton *rb = qobject_cast<QRadioButton*>(w);
        if (rb)
        {
            connect(rb, SIGNAL(toggled(bool)), this, SLOT(changed()));
            continue;
        }

        QComboBox *cmb = qobject_cast<QComboBox*>(w);
        if (cmb)
        {
            connect(cmb, SIGNAL(activated(int)), this, SLOT(changed()));
            continue;
        }

        QSpinBox *sb = qobject_cast<QSpinBox*>(w);
        if (sb)
        {
            connect(sb, SIGNAL(editingFinished()), this, SLOT(changed()));
            continue;
        }

        QTextEdit *textEdit = qobject_cast<QTextEdit*>(w);
        if (textEdit)
        {
            connect(textEdit, SIGNAL(textChanged()), this, SLOT(changed()));
            continue;
        }

        CommentKeywords *ck = dynamic_cast<CommentKeywords*>(w);
        if (ck)
        {
            connect(ck, SIGNAL(changed()), this, SLOT(changed()));
            continue;
        }
    }
}

void BaseObjectDialog::changed()
{
    QWidget *s = dynamic_cast<QWidget*>(sender());
    if (fwbdebug)
        qDebug() << "BaseObjectDialog::changed()  from " << s;

    if (!init)
    {
        emit changed_sign();
    }
}


