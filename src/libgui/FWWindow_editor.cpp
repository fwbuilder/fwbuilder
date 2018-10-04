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

#include "global.h"
#include "utils.h"
#include "events.h"

#include <ui_FWBMainWindow_q.h>

#include "FWBTree.h"
#include "FWWindow.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"
#include "RuleSetModel.h"
#include "RuleSetView.h"
#include "platforms.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Library.h"

#include <QtDebug>
#include <QTimer>
#include <QMdiSubWindow>
#include <QPixmap>
#include <QLineEdit>


using namespace libfwbuilder;

/*
 * when ProjectPanel is created, it sends bunch of stateChange events
 * which lead to calls to ProjectPanel::changeEvent() method. This
 * method checks if editor is visible and contains modified data. Since
 * we create default ProjectPanel object in the constructor of
 * FWWindow, the first call to ProjectPanel::changeEvent() comes when
 * object editor has not been create yet. This is why we need to check
 * for oe != nullptr here and below.
 */
bool FWWindow::isEditorVisible()
{
    return oe != nullptr && m_mainWindow->editorDockWidget->isVisible() &&
        m_mainWindow->editorPanelTabWidget->currentIndex() == EDITOR_PANEL_EDITOR_TAB;
}

void FWWindow::editorPanelTabChanged(int idx)
{
    if (idx == EDITOR_PANEL_EDITOR_TAB)
    {
        ProjectPanel *pp = activeProject();
        if (pp)
        {
            oe->open(pp->getSelectedObject());
        }
    }
}

/*****************************************************************
 * there are too many functions that open, close and hide editor panel.
 * TODO: need to get rid of redundant ones
 */
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

void FWWindow::closeEditorPanel()
{
    m_mainWindow->editorDockWidget->hide(); // editor
}

void FWWindow::clearEditorAndSearchPanels()
{
    findWhereUsedWidget->clear();
    findObjectWidget->clear();
    if (oe != nullptr) oe->blank();
}

void FWWindow::openEditorPanel()
{
}

void FWWindow::openEditor(FWObject *obj)
{
    if (FWBTree().isSystem(obj)) return;

    attachEditorToProjectPanel(activeProject());

    /*
     * See #1060 when user finished editing one of the input fields
     * (QLineEdit) in the object editor and hit Tab, focus moved to
     * the next field but its contents did not get highlighted as
     * usual, but instead cusror was positioned after the last
     * character. This happened because FWCmdChange::notify() reloaded
     * contents of the editor. We can't avoid doing this because we
     * have to update the editor when user cycles through undo/redo
     * operations. It is hard to detect when redo() is called for the
     * first time when user just finished editing something (and even
     * if that was possible, what if they execute undo or redo some
     * time later while looking at the same object in the
     * editor). Will detect situation when editor is reloaded with the
     * same object and restore focus on the same input field. If this
     * field is QLineEdit, will also select contents to emulate
     * correct behavior when user moves between input fields using
     * Tab.
     */
    QWidget *current_focus_widget = QApplication::focusWidget();
    bool reopen = 
        (getOpenedEditor() == obj &&
         current_focus_widget &&
         m_mainWindow->editorDockWidget->isAncestorOf(current_focus_widget));
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(current_focus_widget);
    bool restore_line_edit_selection = line_edit != nullptr && line_edit->hasSelectedText();

    if (fwbdebug)
    {
        qDebug() << "FWWindow::openEditor "
                 << " obj: "
                 << " " << obj->getName().c_str()
                 << " " << obj->getTypeName().c_str()
                 << " reopening in the editor: " << reopen
                 << " current_focus_widget=" << current_focus_widget;
    }

    QString title_txt;
    QPixmap title_icon;
    buildEditorTitleAndIcon(
        obj, ObjectEditor::optNone,
        &title_txt, &title_icon,
        m_mainWindow->m_space->subWindowList(QMdiArea::StackingOrder).size() > 1);

    QSize old_size = m_mainWindow->objectEditorStack->size();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_EDITOR_TAB);
    m_mainWindow->editorDockWidget->setWindowTitle(title_txt);
    m_mainWindow->objectTypeIcon->setPixmap(title_icon);
    m_mainWindow->editorDockWidget->show(); // editor
    oe->open(obj);
    m_mainWindow->objectEditorStack->resize(old_size);

    // #2465 If the object we are about to open in the editor is a
    // firewall and if a ruleset visible in RuleSetView belongs to
    // another firewall, switch ruleset to the ruleset of the new
    // firewall which we looked at last time.
    //
    FWObject *parent_fw = Host::getParentHost(obj);

    if (fwbdebug)
        qDebug() << "parent firewall:" << parent_fw
                 << QString((parent_fw)? parent_fw->getName().c_str() : "");

    if (parent_fw != nullptr)  // this includes Cluster
    {
        RuleSetView* rsv = activeProject()->getCurrentRuleSetView();
        if (rsv)
        {
            RuleSet* current_ruleset = nullptr;
            RuleSetModel* md = nullptr;
            if (rsv)
            {
                md = (RuleSetModel*)rsv->model();
                current_ruleset = md->getRuleSet();
            }
            if (parent_fw != current_ruleset->getParent())
            {
                FWObject *old_rs =
                    activeProject()->m_panel->om->findRuleSetInHistoryByParentFw(
                        parent_fw);

                if (old_rs == nullptr)
                    old_rs = parent_fw->getFirstByType(Policy::TYPENAME);

                if (old_rs != nullptr)
                    QCoreApplication::postEvent(
                        activeProject(), new openRulesetImmediatelyEvent(
                            activeProject()->getFileName(), old_rs->getId()));
            }
        }
    }

    if (reopen)
    {
        if (fwbdebug)
        {
            qDebug() << "FWWindow::openEditor "
                     << "New widget about to get focus:"
                     << current_focus_widget;
        }
        if (current_focus_widget)
        {
            current_focus_widget->setFocus(Qt::TabFocusReason);
            if (restore_line_edit_selection) line_edit->selectAll();
        }
    }
}

void FWWindow::openOptEditor(FWObject *obj, ObjectEditor::OptType t)
{
    attachEditorToProjectPanel(activeProject());


    if (fwbdebug)
        qDebug() << "FWWindow::openOptEditor "
                 << " obj: "
                 << " " << obj->getName().c_str()
                 << " " << obj->getTypeName().c_str()
                 << " option: " << t;

    QString title_txt;
    QPixmap title_icon;
    buildEditorTitleAndIcon(
        obj, t,
        &title_txt, &title_icon,
        m_mainWindow->m_space->subWindowList(QMdiArea::StackingOrder).size() > 1);

    QSize old_size = m_mainWindow->objectEditorStack->size();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_EDITOR_TAB);
    m_mainWindow->editorDockWidget->setWindowTitle(title_txt);
    //m_mainWindow->editorDockWidget->setWindowIcon(title_icon);
    m_mainWindow->objectTypeIcon->setPixmap(title_icon);
    m_mainWindow->editorDockWidget->show(); // editor
    oe->openOpt(obj, t);
    m_mainWindow->objectEditorStack->resize(old_size);
    if (fwbdebug)
        qDebug() << "FWWindow::openOptEditor done";
}

void FWWindow::buildEditorTitleAndIcon(libfwbuilder::FWObject *obj,
                                       ObjectEditor::OptType t,
                                       QString *title_txt,
                                       QPixmap *pm,
                                       bool include_file_name)
{
    QList<QMdiSubWindow*> subwindows = m_mainWindow->m_space->subWindowList(
        QMdiArea::StackingOrder);
    QMdiSubWindow *top_subw = subwindows.last(); // last item is the topmost window
    ProjectPanel *top_pp = dynamic_cast<ProjectPanel*>(top_subw->widget());

    QStringList editor_title;
    FWObject *o = obj;
    Rule *rule = nullptr;
    FWObject *ruleset = nullptr;
    while (o)
    {
        if (Rule::cast(o))
        {
            rule = Rule::cast(o);
            editor_title.push_front(QString("rule #%1").arg(rule->getPosition()));
        } else
            editor_title.push_front(QString::fromUtf8(o->getName().c_str()));
        if (Library::cast(o)) break;
        if (RuleSet::cast(o)) ruleset = o;
        o = o->getParent();
    }

    if (include_file_name) editor_title.push_front(
        QString("[%1]").arg(top_pp->getFileName()));
    else
        editor_title.push_front(" "); // to force '/' in front of object path

    *title_txt = editor_title.join(" / ");

    if (pm)
    {
        FWObject *obj_for_icon = obj;
        if (ruleset) obj_for_icon = ruleset;
        if (rule && t == ObjectEditor::optAction)
        {
            QString icn = ":/Icons/" + getRuleAction(rule) + "/icon-big";
            LoadPixmap(icn, *pm);  // in utils.cpp
        } else
            doSetObjectIcon(obj_for_icon, pm, 2); // big icon
    }
}

void FWWindow::blankEditor()
{
    m_mainWindow->editorDockWidget->setWindowTitle("");
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

void FWWindow::findObject(FWObject *o)
{
    if (activeProject())
    {
        attachEditorToProjectPanel(activeProject());
        findWhereUsedWidget->hide();
        findObjectWidget->findObject(o);
        m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
        findObjectWidget->show();
        m_mainWindow->editorDockWidget->show();
    }
}

void FWWindow::search()
{
    if (activeProject())
    {
        attachEditorToProjectPanel(activeProject());
        m_mainWindow->actionEditor_panel->setChecked(true);
        findWhereUsedWidget->hide();
        m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
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

    attachEditorToProjectPanel(pp);
    findObjectWidget->hide();
    m_mainWindow->editorPanelTabWidget->setCurrentIndex(EDITOR_PANEL_SEARCH_TAB); // search tab
    findWhereUsedWidget->show();
    findWhereUsedWidget->find(obj);
    m_mainWindow->editorDockWidget->show();
}

/*
 * TODO: deprecate this
 */
bool FWWindow::requestEditorOwnership(QWidget*,
                                      FWObject*,
                                      ObjectEditor::OptType,
                                      bool)
{
    if (!isEditorVisible()) return false;

    return true;
}

