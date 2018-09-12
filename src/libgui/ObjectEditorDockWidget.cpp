/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  vadim@fwbuilder.org

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


#include "ObjectEditorDockWidget.h"
#include "ObjectEditor.h"

#include <QCoreApplication>
#include <QtDebug>


ObjectEditorDockWidget::ObjectEditorDockWidget(const QString &title,
                                               QWidget *parent,
                                               Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
    editor = nullptr;
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(topLevelChanged(bool)));
}

ObjectEditorDockWidget::ObjectEditorDockWidget(QWidget *parent,
                                               Qt::WindowFlags flags) :
    QDockWidget(parent, flags)
{
    editor = nullptr;
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(topLevelChanged(bool)));
}

void ObjectEditorDockWidget::setupEditor(ObjectEditor *ed) { editor = ed; }

void ObjectEditorDockWidget::closeEvent(QCloseEvent *event)
{
    if (fwbdebug) qDebug() << "ObjectEditorDockWidget::closeEvent()";

    if (!editor->validate())
    {
        editor->load();  // bad changes in the editor, reset
        event->ignore();
        return;
    }

    event->accept();

    if (fwbdebug) qDebug() << "ObjectEditorDockWidget::closeEvent() done";
}

void ObjectEditorDockWidget::topLevelChanged(bool topLevel)
{
    if (topLevel) adjustSize();
}
