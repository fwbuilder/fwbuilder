/*

                          Firewall Builder

                 Copyright (C) 2003, 2006 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"

#include <ui_FWBMainWindow_q.h>

#include "FWWindow.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"

#include <QtDebug>
#include <QTimer>


using namespace libfwbuilder;

/*
 * when ProjectPanel is created, it send bunch of stateChange events
 * which leads to calls to ProjectPanel::changeEvent() method. This
 * method checks if editr is visible and contains modified data. Since
 * we create defaulr ProjectPanel object in the constructor of
 * FWWindow, the first call to ProjectPanel::changeEvent() comes when
 * object editor has not been create yet. This is why we need to check
 * for oe != NULL here and below.
 */
bool FWWindow::isEditorVisible()
{
    return oe != NULL && m_mainWindow->editorDockWidget->isVisible(); // editor
}

bool FWWindow::isEditorModified()
{
    return oe != NULL && oe->isModified();
}

void FWWindow::showEditor()
{
    m_mainWindow->objectEditorStack->setCurrentIndex(oe->getCurrentDialogIndex());
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_EDITOR_TAB);
    m_mainWindow->editorDockWidget->show(); // editor
}

void FWWindow::hideEditor()
{
    closeEditorPanel();
}

void FWWindow::closeEditor()
{
    m_mainWindow->editorDockWidget->close(); // editor
}

void FWWindow::openEditor(FWObject *o)
{
    QSize old_size = m_mainWindow->objectEditorStack->size();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_EDITOR_TAB);
    m_mainWindow->editorDockWidget->show(); // editor
    oe->open(o);
    m_mainWindow->objectEditorStack->setCurrentIndex(oe->getCurrentDialogIndex());
    //m_mainWindow->objectEditorFrame->show();
    m_mainWindow->objectEditorStack->resize(old_size);
}

void FWWindow::openOptEditor(FWObject *o, ObjectEditor::OptType t)
{
    QSize old_size = m_mainWindow->objectEditorStack->size();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_EDITOR_TAB);
    m_mainWindow->editorDockWidget->show(); // editor
    oe->openOpt(o, t);
    m_mainWindow->objectEditorStack->setCurrentIndex(oe->getCurrentDialogIndex());
    //m_mainWindow->objectEditorFrame->show();
    m_mainWindow->objectEditorStack->resize(old_size);
}

void FWWindow::blankEditor()
{
    oe->blank();
}

FWObject* FWWindow::getOpenedEditor()
{
    return oe->getOpened();
}

ObjectEditor::OptType FWWindow::getOpenedOptEditor()
{
    return oe->getOpenedOpt();
}

void FWWindow::selectObjectInEditor(FWObject *o)
{
    oe->selectObject(o);
}

void FWWindow::actionChangedEditor(FWObject *o)
{
    oe->actionChanged(o);
}

bool FWWindow::validateAndSaveEditor()
{
    return oe->validateAndSave();
}

void FWWindow::closeEditorPanel()
{
    //m_mainWindow->objectEditorFrame->hide();
    m_mainWindow->editorDockWidget->hide(); // editor
}

void FWWindow::openEditorPanel()
{
    //m_mainWindow->objectEditorFrame->show();

}

void FWWindow::findObject(FWObject *o)
{
    findWhereUsedWidget->hide();
    findObjectWidget->attachToProjectWindow(activeProject());
    findObjectWidget->findObject(o);
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
    findObjectWidget->show();
    m_mainWindow->editorDockWidget->show();
}

void FWWindow::search()
{
    if (activeProject())
    {
        m_mainWindow->actionEditor_panel->setChecked(true);
        findWhereUsedWidget->hide();
        m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
        findObjectWidget->attachToProjectWindow(activeProject());
        findObjectWidget->show();
        m_mainWindow->editorDockWidget->show();
    }
}

void FWWindow::findWhereUsed(FWObject * obj, ProjectPanel *pp)
{
    if (fwbdebug)
        qDebug() << "FWWindow::findWhereUsed findWhereUsedWidget="
                 << findWhereUsedWidget
                 << " project panel: " 
                 << pp;

    findWhereUsedWidget->attachToProjectWindow(pp);
    findObjectWidget->hide();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
    findWhereUsedWidget->show();
    findWhereUsedWidget->find(obj);
    m_mainWindow->editorDockWidget->show();
}

void FWWindow::rollBackSelectionSameWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(true);
}

void FWWindow::rollBackSelectionDifferentWidget()
{
    editorOwner->setFocus();
    emit restoreSelection_sign(false);
}

void FWWindow::releaseEditor()
{
    disconnect( SIGNAL(restoreSelection_sign(bool)) );
}

void FWWindow::connectEditor(QWidget *w)
{
    connect(this, SIGNAL(restoreSelection_sign(bool)),
            w, SLOT(restoreSelection(bool)));
}

bool FWWindow::requestEditorOwnership(QWidget *w,
                                      FWObject *obj,
                                      ObjectEditor::OptType otype,
                                      bool validate)
{
    if (!isEditorVisible()) return false;

    if(obj == getOpenedEditor() &&
       otype == getOpenedOptEditor() &&
       w == editorOwner )
    {
        releaseEditor();
        editorOwner = w;
        connectEditor(editorOwner);
        return true;
    }

    if (validate && !validateAndSaveEditor())
    {
        /*
         * roll back selection in the widget that currently
         * owns the editor. Signal restoreSelection_sign
         * is still connected to the previous owner
         */
        if (w == editorOwner )
            QTimer::singleShot(0, this, SLOT(rollBackSelectionSameWidget()));
        else
            QTimer::singleShot(0,this,SLOT(rollBackSelectionDifferentWidget()));
        return false;
    }

    if (w)
    {
        releaseEditor();
        editorOwner = w;
        connectEditor(editorOwner);
    }
    return true;
}

