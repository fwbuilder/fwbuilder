/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: PrefsDialog.cpp,v 1.29 2007/02/04 04:20:26 vkurland Exp $

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied wdarranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


#include "config.h"
#include "global.h"
#include "utils.h"

#include "PrefsDialog.h"
#include "FWBSettings.h"
#include "listOfLibraries.h"
#include "FWWindow.h"
#include "ObjectManipulator.h"

#include <qlineedit.h>
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlistwidget.h>
#include <qdir.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qtooltip.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qpixmapcache.h>
#include <qfontdialog.h>
/*

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#ifdef _WIN32
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#else
#  include <unistd.h>
#endif
*/

#include <iostream>

using namespace std;

void PrefsDialog::setButtonColor(QPushButton *btn,const QString &colorCode)
{
    QPixmap pm(40,14);
    pm.fill( QColor(colorCode) );
    QPainter p( &pm );
    p.drawRect( pm.rect() );
    btn->setIcon(QIcon(pm));
}

PrefsDialog::~PrefsDialog()
{
    delete m_dialog;
}

PrefsDialog::PrefsDialog(QWidget *parent) : QDialog(parent)
{
    m_dialog = new Ui::prefsDialog_q;
    m_dialog->setupUi(this);
    
    m_dialog->wDir->setText( st->getWDir() );
    m_dialog->startupAction->setCurrentIndex( st->getStartupAction() );
    m_dialog->expandTree->setChecked( st->getExpandTree() );
//    mergeLibs->setChecked( st->getMergeLibs() );

    m_dialog->objTooltips->setChecked( st->getObjTooltips() );
    m_dialog->tooltipDelay->setValue( st->getTooltipDelay() );

    m_dialog->deletedObj->setChecked( st->getBool("UI/ShowDeletedObjects") );

    m_dialog->emptyRCSLog->setChecked( st->getRCSLogState() );
    m_dialog->autosave->setChecked( st->getAutoSave() );

    m_dialog->autosaveFile->setChecked( st->getBool("Environment/autoSaveFile") );
    m_dialog->autosaveInterval->setValue( st->getInt("Environment/autoSaveFilePeriod"));

//    dontSaveStdLib->setChecked( st->getDontSaveStdLib() );

    m_dialog->sshPath->setText( st->getSSHPath() );

    for (list<libData>::iterator i=addOnLibs->begin(); i!=addOnLibs->end(); ++i)
    {
        QStringList qsl;
        qsl << i->name << "" << i->path; //fromUtf8
        QTreeWidgetItem *itm = new QTreeWidgetItem( m_dialog->avLibs, qsl );
        if (i->load)
        {
            //itm->setPixmap(1, QPixmap::fromMimeSource( "apply.png" ) );

            QPixmap pm;
            QString icn = ":/Icons/apply.png";
            if ( ! QPixmapCache::find( icn, pm) )
            {
                pm.load( icn );
                QPixmapCache::insert( icn, pm);
            }
            itm->setIcon(1, QIcon(pm));
        }
    }

// set label icons colors and text strings using user's settings

    QString t;
    colors[FWBSettings::RED]=st->getLabelColor(FWBSettings::RED);
    t=st->getLabelText (FWBSettings::RED);

    setButtonColor(m_dialog->redBtn,colors[FWBSettings::RED]);
    m_dialog->redText->setText(t);

    colors[FWBSettings::ORANGE]=st->getLabelColor(FWBSettings::ORANGE);
    t=st->getLabelText (FWBSettings::ORANGE);

    setButtonColor(m_dialog->orangeBtn,colors[FWBSettings::ORANGE]);
    m_dialog->orangeText->setText(t);

    colors[FWBSettings::YELLOW]=st->getLabelColor(FWBSettings::YELLOW);
    t=st->getLabelText (FWBSettings::YELLOW);

    setButtonColor(m_dialog->yellowBtn,colors[FWBSettings::YELLOW]);
    m_dialog->yellowText->setText(t);

    colors[FWBSettings::GREEN]=st->getLabelColor(FWBSettings::GREEN);
    t=st->getLabelText (FWBSettings::GREEN);

    setButtonColor(m_dialog->greenBtn,colors[FWBSettings::GREEN]);
    m_dialog->greenText->setText(t);

    colors[FWBSettings::BLUE]=st->getLabelColor(FWBSettings::BLUE);
    t=st->getLabelText (FWBSettings::BLUE);

    setButtonColor(m_dialog->blueBtn,colors[FWBSettings::BLUE]);
    m_dialog->blueText->setText(t);

    colors[FWBSettings::PURPLE]=st->getLabelColor(FWBSettings::PURPLE);
    t=st->getLabelText (FWBSettings::PURPLE);

    setButtonColor(m_dialog->purpleBtn,colors[FWBSettings::PURPLE]);
    m_dialog->purpleText->setText(t);

    colors[FWBSettings::GRAY]=st->getLabelColor(FWBSettings::GRAY);
    t=st->getLabelText (FWBSettings::GRAY);

    setButtonColor(m_dialog->grayBtn,colors[FWBSettings::GRAY]);
    m_dialog->grayText->setText(t);

    m_dialog->chShowIcons->setChecked(st->getShowIconsInRules() );

    if (FWBSettings::SIZE25X25 == st->getIconsInRulesSize())
        m_dialog->rb25->setChecked(true);
    else
        m_dialog->rb16->setChecked(true);
    changeShowIcons();
    
    rulesFont = st->getRulesFont();
    treeFont = st->getTreeFont();
    uiFont = st->getUiFont();
    m_dialog->chClipComment->setChecked(st->getClipComment() );
}

void PrefsDialog::changeColor(QPushButton *btn,
                              FWBSettings::LabelColors colorCode)
{
    QColor clr = QColorDialog::getColor(
        QColor(colors[colorCode]), this);

    if (!clr.isValid()) return;
    colors[colorCode]= clr.name();
    setButtonColor(btn,colors[colorCode]);
}

void PrefsDialog::changeRedColor()
{
    changeColor(m_dialog->redBtn, FWBSettings::RED);
}

void PrefsDialog::changeOrangeColor()
{
    changeColor(m_dialog->orangeBtn, FWBSettings::ORANGE);
}

void PrefsDialog::changeYellowColor()
{
    changeColor(m_dialog->yellowBtn, FWBSettings::YELLOW);
}

void PrefsDialog::changeGreenColor()
{
    changeColor(m_dialog->greenBtn, FWBSettings::GREEN);
}

void PrefsDialog::changeBlueColor()
{
    changeColor(m_dialog->blueBtn, FWBSettings::BLUE);
}

void PrefsDialog::changePurpleColor()
{
    changeColor(m_dialog->purpleBtn, FWBSettings::PURPLE);
}

void PrefsDialog::changeGrayColor()
{
    changeColor(m_dialog->grayBtn, FWBSettings::GRAY);
}

void PrefsDialog::changeIconSize25()
{
    //st->setIconsInRulesSize(FWBSettings::SIZE25X25);
}

void PrefsDialog::changeIconSize16()
{
    //st->setIconsInRulesSize(FWBSettings::SIZE16X16);
}

void PrefsDialog::changeShowIcons()
{
    bool areShown = m_dialog->chShowIcons->isChecked();
    m_dialog->rb16->setEnabled(areShown);
    m_dialog->rb25->setEnabled(areShown);
}

void PrefsDialog::changeRulesFont()
{
    changeFont(&rulesFont);
}

void PrefsDialog::changeTreeFont()
{
    changeFont(&treeFont);
}

void PrefsDialog::changeUiFont()
{
    changeFont(&uiFont);
}

void PrefsDialog::changeFont(QFont *font)
{
    bool ok;
    QFont f = QFontDialog::getFont(&ok, *font, this);
    if (ok)
        *font = f;
}

void PrefsDialog::findWDir()
{
    QString wd = st->getWDir();
    QString dir = QFileDialog::getExistingDirectory(
            this, tr("Find working directory"), wd, QFileDialog::ShowDirsOnly );

    if (!dir.isEmpty()) m_dialog->wDir->setText(dir);
}

void PrefsDialog::findSSH()
{
    QString fp = QFileDialog::getOpenFileName(
        this,
        tr("Find Secure Shell utility") );

    if (!fp.isEmpty()) m_dialog->sshPath->setText(fp);
}

void PrefsDialog::libClick(QTreeWidgetItem* itm, int col)
{
    if (itm && col==1)
    {
        if (itm->text(0)=="Standard") return;

        for (list<libData>::iterator i=addOnLibs->begin();
             i!=addOnLibs->end(); ++i)
        {
            if (i->name == itm->text(0)) //fromUtf8
            {
                if (i->load)  itm->setIcon(1, QIcon());
                else
                {
                    //itm->setPixmap(1, QPixmap::fromMimeSource( "apply.png" ) );
                    QPixmap pm;
                    QString icn = ":/Icons/apply.png";
                    if ( ! QPixmapCache::find( icn, pm) )
                    {
                        pm.load( icn );
                        QPixmapCache::insert( icn, pm);
                    }
                    itm->setIcon(1, QIcon(pm));
                }

                i->load = !i->load;

#if 0
// commented out for bug #1620284
                if (i->load)
                {
                    mw->loadLibrary( i->path.latin1() );
                    om->loadObjects();
                }
#endif
                break;
            }
        }
    }
}

void PrefsDialog::addLibrary()
{
    QString fp = QFileDialog::getOpenFileName(
        this, 
        tr("Find add-on library"),
        st->getWDir(),
        "Firewall Builder 2 files (*.fwl)");

    if (!fp.isEmpty())
    {
        list<libData>::iterator i = addOnLibs->add( fp.toLatin1().constData(), true );
        if (i==addOnLibs->end())
        {
            if (fwbdebug)
                qDebug("PrefsDialog::addLibrary(): library addition failed");
            return;
        }
        QString libname = i->name; //fromUtf8
        QStringList qsl;
        qsl << libname << "" << fp;
        QTreeWidgetItem *itm = new QTreeWidgetItem( m_dialog->avLibs, qsl );

        QPixmap pm;
        QString icn = ":/Icons/apply.png";
        if ( ! QPixmapCache::find( icn, pm) )
        {
            pm.load( icn );
            QPixmapCache::insert( icn, pm);
        }
        itm->setIcon(1, QIcon(pm));

        // commented out for bug #1620284
        //
        //mw->loadLibrary( i->path.latin1() );
        //om->loadObjects();
    }
}

/* when user removes library from the list, the change gets in effect
 * next time they start the program. There is a warning in the dialog
 * that says so
 */
void PrefsDialog::remLibrary()
{
    QTreeWidgetItem *itm = m_dialog->avLibs->currentItem();
    if (itm==NULL) return;

    if (itm->text(0)=="Standard") return;

    for (list<libData>::iterator i=addOnLibs->begin(); i!=addOnLibs->end(); ++i)
    {
        if (i->name == itm->text(0)) //fromUtf8
        {
            addOnLibs->erase(i);
            delete itm;
            break;
        }
    }
}

void PrefsDialog::accept()
{
    QString wd=m_dialog->wDir->text();

/* check if the default working directory does not exist yet */

    st->setWDir( wd );
    st->setStartupAction( m_dialog->startupAction->currentIndex() );
    st->setExpandTree( m_dialog->expandTree->isChecked() );
//    st->setMergeLibs( mergeLibs->isChecked() );

    st->setObjTooltips( m_dialog->objTooltips->isChecked() );
    st->setTooltipDelay( m_dialog->tooltipDelay->value() );

    st->setBool("UI/ShowDeletedObjects", m_dialog->deletedObj->isChecked() );

//  QToolTip::setWakeUpDelay( st->getTooltipDelay()*1000 );

    st->setRCSLogState( m_dialog->emptyRCSLog->isChecked() );
    st->setAutoSave( m_dialog->autosave->isChecked() );

    st->setBool("Environment/autoSaveFile", m_dialog->autosaveFile->isChecked() );
    st->setInt("Environment/autoSaveFilePeriod", m_dialog->autosaveInterval->value() );

//    st->setDontSaveStdLib( dontSaveStdLib->isChecked() );

    st->setLabelColor(FWBSettings::RED,    colors[FWBSettings::RED]);
    st->setLabelColor(FWBSettings::ORANGE, colors[FWBSettings::ORANGE]);
    st->setLabelColor(FWBSettings::YELLOW, colors[FWBSettings::YELLOW]);
    st->setLabelColor(FWBSettings::GREEN,  colors[FWBSettings::GREEN]);
    st->setLabelColor(FWBSettings::BLUE,   colors[FWBSettings::BLUE]);
    st->setLabelColor(FWBSettings::PURPLE, colors[FWBSettings::PURPLE]);
    st->setLabelColor(FWBSettings::GRAY,   colors[FWBSettings::GRAY]);

    st->setLabelText (FWBSettings::RED,    m_dialog->redText->text() );
    st->setLabelText (FWBSettings::ORANGE, m_dialog->orangeText->text() );
    st->setLabelText (FWBSettings::YELLOW, m_dialog->yellowText->text() );
    st->setLabelText (FWBSettings::GREEN,  m_dialog->greenText->text() );
    st->setLabelText (FWBSettings::BLUE,   m_dialog->blueText->text() );
    st->setLabelText (FWBSettings::PURPLE, m_dialog->purpleText->text() );
    st->setLabelText (FWBSettings::GRAY,   m_dialog->grayText->text() );
    
    st->setShowIconsInRules(m_dialog->chShowIcons->isChecked());
    FWBSettings::IconSize sz = m_dialog->rb25->isChecked() ? 
        FWBSettings::SIZE25X25 : FWBSettings::SIZE16X16;
    st->setIconsInRulesSize(sz);
    
    st->setRulesFont(rulesFont);
    st->setTreeFont(treeFont);
    st->setUiFont(uiFont);

    st->setClipComment(m_dialog->chClipComment->isChecked());
    
    st->setSSHPath( m_dialog->sshPath->text() );
    
    QDir d;
    d.mkdir( wd );

    mw->setupAutoSave();
    om->showDeletedObjects(st->getBool("UI/ShowDeletedObjects"));

    QDialog::accept();
}
