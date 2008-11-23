/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com
           refactoring and bugfixes: vadim@fwbuilder.org

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
#include "utils.h"
#include "utils_no_qt.h"

#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "RCS.h"
#include "RuleSetView.h"

#include <QMdiSubWindow>
#include <QMdiArea>

#include "fwbuilder/Library.h"
#include "fwbuilder/RuleSet.h"


using namespace Ui;
using namespace libfwbuilder;
using namespace std;

// this slot is called when user hits "maximize" or "minimize" buttons
// on the title bar of the internal window. Need to restore window
// geometry and splitter position when window becomes normal (not maximized).
void ProjectPanel::stateChanged(Qt::WindowStates oldState,
                                Qt::WindowStates newState)
{
    if (fwbdebug) qDebug("ProjectPanel::stateChanged");

#if 0
// vk 09/16
    bool is_maximized = ((newState & Qt::WindowMaximized) != 0);
    bool was_maximized = ((oldState & Qt::WindowMaximized) != 0);
    st->setInt("Window/maximized", is_maximized);
    if (!was_maximized && is_maximized) saveState();
#endif

}



void ProjectPanel::saveState()
{
    QString FileName ;
    if (rcs!=NULL) FileName = rcs->getFileName();
    if (fwbdebug) qDebug("ProjectPanel::saveState %p title=%s "
                         "FileName=%s ready=%d",
                         this,
                         getPageTitle().toAscii().constData(),
                         FileName.toAscii().data(),
                         ready);

    if (!ready) return;

    st->setInt("Window/maximized", mdiWindow->isMaximized());

    if (!mdiWindow->isMaximized())
    {
        st->setInt("Window/" + FileName + "/x", mdiWindow->x());
        st->setInt("Window/" + FileName + "/y", mdiWindow->y());
        st->setInt("Window/" + FileName + "/width", mdiWindow->width ());
        st->setInt("Window/" + FileName + "/height", mdiWindow->height ());
    }

    // Save position of splitters regardless of the window state
    QList<int> sl = m_panel->mainSplitter->sizes();
    QString arg = QString("%1,%2").arg(sl[0]).arg(sl[1]);
    if (sl[0] || sl[1])
        st->setStr("Window/" + FileName + "/MainWindowSplitter", arg );

    if (fwbdebug)
    {
        QString out1 = " save Window/" + FileName + "/MainWindowSplitter";
        out1+= " " + arg;
        qDebug(out1.toAscii().data());
    }

    sl = m_panel->objInfoSplitter->sizes();
    arg = QString("%1,%2").arg(sl[0]).arg(sl[1]);
    if (sl[0] || sl[1])
        st->setStr("Window/" + FileName + "/ObjInfoSplitter", arg );

    m_panel->om->saveExpandedTreeItems();

    saveLastOpenedLib();
    saveOpenedRuleSet();

    if (fwbdebug) qDebug("ProjectPanel::saveState() done");
}

void ProjectPanel::loadState(bool open_objects)
{
    int w1 = 0;
    int w2 = 0;
    QString w1s, w2s;
    bool ok = false;

    if (rcs==NULL) return;
    QString filename = rcs->getFileName();

    if (fwbdebug)
    {
        qDebug("ProjectPanel::loadState filename=%s isMaximized=%d",
               filename.toAscii().data(), isMaximized());
        qDebug("mdiWindow=%p", mdiWindow);
        qDebug("ready=%d", ready);
    }

    if (!ready) return;

    int maximized_status = st->getInt("Window/maximized");
    if (!maximized_status && mdiWindow)
    {
        if (fwbdebug) qDebug("ProjectPanel::loadState  show normal");
        setWindowState(0);
        int x = st->getInt("Window/"+filename+"/x");
        int y = st->getInt("Window/"+filename+"/y");
        int width = st->getInt("Window/"+filename+"/width");
        int height = st->getInt("Window/"+filename+"/height");
        if (width==0 || height==0)
        {
            x = 10;
            y = 10;
            width = 600;
            height= 600;
        }
        if (fwbdebug)
            qDebug("ProjectPanel::loadState  set geometry: %d %d %d %d",
                   x,y,width,height);
        
        mdiWindow->setGeometry(x,y,width,height);
    }

    QString h_splitter_setting = "Window/" + filename + "/MainWindowSplitter";
    QString val = st->getStr(h_splitter_setting);
    
    w1s = val.split(',')[0];
    ok = false;
    w1 = w1s.toInt(&ok, 10);
    if (!ok || w1 == 0) w1 = DEFAULT_H_SPLITTER_POSITION;

    w2 = mdiWindow->width() - w1;

    if (fwbdebug)
        qDebug(QString("%1: %2x%3").arg(h_splitter_setting).
               arg(w1).arg(w2).toAscii().data());

    setMainSplitterPosition(w1, w2);

    if (fwbdebug) qDebug("Restore info window splitter position");

    QString v_splitter_setting = "Window/" + filename + "/ObjInfoSplitter";
    val = st->getStr(v_splitter_setting);

    w1s = val.split(',')[0];
    ok = false;
    w1 = w1s.toInt(&ok, 10);
    if (!ok || w1 == 0) w1 = DEFAULT_V_SPLITTER_POSITION;
    w2 = mdiWindow->height() - w1;

    if (fwbdebug)
        qDebug(QString("%1: %2x%3").arg(v_splitter_setting).
               arg(w1).arg(w2).toAscii().data());

    setObjInfoSplitterPosition(w1, w2);

    m_panel->om->loadExpandedTreeItems();

    loadLastOpenedLib();
    loadOpenedRuleSet();

    time_t last_modified = db()->getTimeLastModified();
    if (fwbdebug)
        qDebug("ProjectPanel::loadState(): done: "
               "dirty=%d last_modified=%s",
               db()->isDirty(), ctime(&last_modified));
}

void ProjectPanel::setMainSplitterPosition(int w1, int w2)
{
    if (w1 && w2)
    {
        QList<int> sl;
        sl.push_back(w1);
        sl.push_back(w2);
        if (fwbdebug) qDebug("Setting main splitter position: %d,%d", w1, w2);
        m_panel->mainSplitter->setSizes( sl );
    }
}

void ProjectPanel::setObjInfoSplitterPosition(int w1, int w2)
{
    if (w1 && w2) 
    {
        QList<int> sl;
        sl.clear();
        sl.push_back(w1);
        sl.push_back(w2);
        m_panel->objInfoSplitter->setSizes( sl );
    }
}

void ProjectPanel::loadOpenedRuleSet()
{
    if (rcs==NULL) return;
    QString filename = rcs->getFileName();

    int id = st->getVisibleRuleSetId(
        filename, m_panel->om->getCurrentLib()->getName().c_str());

    if (id > 0)
    {
        FWObject *obj = db()->getById(id, true);
        if (obj)
        {
            m_panel->om->openObject(obj);

            time_t last_modified = db()->getTimeLastModified();
            if (fwbdebug)
                qDebug("ProjectPanel::loadOpenedRuleSet(): checkpoint 1: "
                       "dirty=%d last_modified=%s",
                       db()->isDirty(), ctime(&last_modified));

            openRuleSet(obj);

            last_modified = db()->getTimeLastModified();
            if (fwbdebug)
                qDebug("ProjectPanel::loadOpenedRuleSet(): checkpoint 2: "
                       "dirty=%d last_modified=%s",
                       db()->isDirty(), ctime(&last_modified));
        }
    }
}

void ProjectPanel::saveOpenedRuleSet()
{
    if (rcs==NULL) return;
    QString filename = rcs->getFileName();

    if (visibleRuleSet!=NULL)
    {
        st->setVisibleRuleSet(filename,
                              visibleRuleSet->getLibrary()->getName().c_str(),
                              visibleRuleSet);
        getCurrentRuleSetView()->saveCollapsedGroups();
    }
}
     
void ProjectPanel::saveLastOpenedLib()
{
    QString filename = "";
    if (rcs!=NULL) filename = rcs->getFileName();

    FWObject*  obj = m_panel->om->getCurrentLib();
    if (obj!=NULL)
    {
        std::string sid = FWObjectDatabase::getStringId(obj->getId());
        st->setStr("Window/" + filename + "/LastLib", sid.c_str() );
        
    }
}
    
void ProjectPanel::loadLastOpenedLib()
{
    if (fwbdebug) qDebug("ProjectPanel::loadLastOpenedLib()");

    QString filename = "";
    if (rcs!=NULL) filename = rcs->getFileName();
    QString sid = st->getStr("Window/" + filename + "/LastLib");
    if (filename!="" && sid!="")
    {
        if (fwbdebug)
            qDebug("ProjectPanel::loadLastOpenedLib(): filename=%s "
                   "opening lib id=%s",
                   filename.toAscii().constData(),
                   sid.toAscii().constData());

        m_panel->om->libChangedById(
            FWObjectDatabase::getIntId(sid.toStdString()));
    }
    else
    {
        list<FWObject*> all_libs = db()->getByType(Library::TYPENAME);
        FWObject *first_non_system_lib = NULL;
        for (list<FWObject*>::iterator i=all_libs.begin(); i!=all_libs.end(); ++i)
        {
            string str_id = FWObjectDatabase::getStringId((*i)->getId());
            if (str_id.find("sysid")==0) continue;
            if (first_non_system_lib==NULL) first_non_system_lib = (*i);
            if ((*i)->getName()=="User")
            {
                first_non_system_lib = *i;
                break;
            }
        }
        if (first_non_system_lib)
            //m_panel->om->openObject(first_non_system_lib);
            m_panel->om->libChangedById(first_non_system_lib->getId());
    }

    time_t last_modified = db()->getTimeLastModified();
    if (fwbdebug)
        qDebug("ProjectPanel::loadLastOpenedLib(): done: "
               "dirty=%d last_modified=%s",
               db()->isDirty(), ctime(&last_modified));
}






