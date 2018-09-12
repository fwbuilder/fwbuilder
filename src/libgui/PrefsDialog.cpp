/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

  $Id$

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


#include "global.h"
#include "check_update_url.h"

#include "utils.h"
#include "platforms.h"

#include "PrefsDialog.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "RuleSetView.h"

#include "fwbuilder/Resources.h"

#include <qlineedit.h>
#include <qfiledialog.h>
#include <qcombobox.h>
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
#include <qmessagebox.h>

#include <QTreeWidgetItem>
#include <QUrl>
#include <QCheckBox>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>

#include <sstream>

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

    m_dialog->tabWidget->setCurrentIndex(0);

    m_dialog->wDir->setText(st->getWDir());
    m_dialog->dataDir->setText(st->getDataDir());

    m_dialog->objTooltips->setChecked( st->getObjTooltips() );
    m_dialog->advTooltipMode->setChecked(st->getBool("UI/AdvancedTooltips"));
    m_dialog->advTooltipMode->setEnabled(st->getObjTooltips());
//    m_dialog->tooltipDelay->setValue( st->getTooltipDelay() );

    m_dialog->enableCustomTemplates->setChecked( st->customTemplatesEnabled() );

    m_dialog->deletedObj->setChecked( st->getBool("UI/ShowDeletedObjects") );
    m_dialog->attributesInTree->setChecked(
        st->getBool("UI/ShowObjectsAttributesInTree") );

    m_dialog->new_dns_name_compile_tm->setChecked(
        st->getBool("Objects/DNSName/useCompileTimeForNewObjects"));
    m_dialog->new_dns_name_run_tm->setChecked(
        ! st->getBool("Objects/DNSName/useCompileTimeForNewObjects"));
    m_dialog->use_name_for_dns_record->setChecked(
        st->getBool("Objects/DNSName/useNameForDNSRecord"));

    m_dialog->new_addr_tbl_compile_tm->setChecked(
        st->getBool("Objects/AddressTable/useCompileTimeForNewObjects"));
    m_dialog->new_addr_tbl_run_tm->setChecked(
        ! st->getBool("Objects/AddressTable/useCompileTimeForNewObjects"));

    m_dialog->toolbarIconsText->setChecked(st->getIconsWithText());

    m_dialog->emptyRCSLog->setChecked( st->getRCSLogState() );

    m_dialog->autosaveFile->setChecked(st->getBool("Environment/autoSaveFile"));
    m_dialog->autosaveInterval->setValue(
        st->getInt("Environment/autoSaveFilePeriod"));

    m_dialog->dataFileCompression->setChecked( st->getCompression() );

//    dontSaveStdLib->setChecked( st->getDontSaveStdLib() );

    m_dialog->sshPath->setText( st->getSSHPath() );
    m_dialog->scpPath->setText( st->getSCPPath() );;
    m_dialog->sshTimeout->setValue( st->getSSHTimeout() );

    m_dialog->rememberSshPass->setChecked(
        st->getBool("Environment/RememberSshPassEnabled") );

    m_dialog->autoCompileChk->setChecked( st->getBool("/Diff/AutoCompile"));
    m_dialog->diffPath->setText( st->getDiffPath() );

    m_dialog->showTips->setChecked( st->getBool("UI/NoStartTip") );

    m_dialog->rulesLoggingOn->setChecked(
        st->getBool("Objects/PolicyRule/defaultLoggingState") );

    m_dialog->rulesDefaultStateful->setChecked(
        st->getBool("Objects/PolicyRule/defaultStateful"));

    m_dialog->rulesDefaultAction->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultAction"));

    m_dialog->rulesDefaultDirection->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultDirection"));

    m_dialog->rulesDefaultSource->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultSource"));

    m_dialog->rulesDefaultDestination->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultDestination"));

    m_dialog->rulesDefaultService->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultService"));

    m_dialog->rulesDefaultInterface->setCurrentIndex(
        st->getInt("Objects/PolicyRule/defaultInterface"));

    m_dialog->autoconfigure_interfaces->setChecked(
        st->getBool("Objects/Interface/autoconfigureInterfaces") );

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
    m_dialog->showDirectionText->setChecked(st->getShowDirectionText());
    if (FWBSettings::SIZE25X25 == st->getIconsInRulesSize())
        m_dialog->rb25->setChecked(true);
    else
        m_dialog->rb16->setChecked(true);
    changeShowIcons();

    rulesFont = st->getRulesFont();
    treeFont = st->getTreeFont();
    uiFont = st->getUiFont();
    compilerOutputFont = st->getCompilerOutputFont();

    m_dialog->rulesFontDescr->setText(getFontDescription(rulesFont));
    m_dialog->treeFontDescr->setText(getFontDescription(treeFont));
    m_dialog->compilerOutputFontDescr->setText(getFontDescription(compilerOutputFont));

    m_dialog->chClipComment->setChecked(st->getClipComment() );

    m_dialog->checkUpdates->setChecked(st->getCheckUpdates() );

    m_dialog->checkUpdatesProxy->setText(st->getCheckUpdatesProxy() );

#if !defined(Q_OS_WIN32)
    m_dialog->plink_hint->hide();
#endif

    // Diff Viewer

    colors[FWBSettings::ADD_COLOR]=st->getDiffColor(FWBSettings::ADD_COLOR);
    setButtonColor(m_dialog->addColorBtn, colors[FWBSettings::ADD_COLOR]);

    colors[FWBSettings::EDIT_COLOR]=st->getDiffColor(FWBSettings::EDIT_COLOR);
    setButtonColor(m_dialog->editColorBtn, colors[FWBSettings::EDIT_COLOR]);

    colors[FWBSettings::MOVE_COLOR]=st->getDiffColor(FWBSettings::MOVE_COLOR);
    setButtonColor(m_dialog->moveColorBtn, colors[FWBSettings::MOVE_COLOR]);

    colors[FWBSettings::REMOVE_COLOR]=st->getDiffColor(FWBSettings::REMOVE_COLOR);
    setButtonColor(m_dialog->removeColorBtn, colors[FWBSettings::REMOVE_COLOR]);

    m_dialog->displayUnmodifiedRulesChk->setChecked(st->getDisplayUnmodifiedRules());

    // Fill lists of platforms and host OS

    QMap<QString,QString> platforms = getAllPlatforms(false);
    QMap<QString,QString> os = getAllOS(false);

    m_dialog->enabled_platforms->setRowCount(platforms.size());
    m_dialog->enabled_platforms->setColumnCount(1);
    int row = 0;
    QMap<QString,QString>::iterator it;
    for (it=platforms.begin(); it!=platforms.end(); ++it)
    {
        QString name = it.key();
        QString readable_name = it.value();
        QTableWidgetItem *cb = new QTableWidgetItem(readable_name);
        m_dialog->enabled_platforms->setItem(row, 0, cb);

        QString res_status = Resources::platform_res[name.toStdString()]->getResourceStr(
            "/FWBuilderResources/Target/status/").c_str();
        QString prefs_status = st->getTargetStatus(name, res_status);
        cb->setCheckState((prefs_status=="disabled") ? Qt::Unchecked : Qt::Checked);

        cb->setData(Qt::UserRole, name);
        row++;
    }
    m_dialog->enabled_platforms->horizontalHeader()->setStretchLastSection(true);
    m_dialog->enabled_platforms->sortItems(0);
    m_dialog->enabled_platforms->update();

    m_dialog->enabled_os->setRowCount(os.size());
    m_dialog->enabled_os->setColumnCount(1);
    row = 0;
    for (it=os.begin(); it!=os.end(); ++it)
    {
        QString name = it.key();
        QString readable_name = it.value();
        QTableWidgetItem *cb = new QTableWidgetItem(readable_name);
        m_dialog->enabled_os->setItem(row, 0, cb);

        QString res_status = Resources::os_res[name.toStdString()]->getResourceStr(
            "/FWBuilderResources/Target/status/").c_str();
        QString prefs_status = st->getTargetStatus(name, res_status);
        cb->setCheckState((prefs_status=="disabled") ? Qt::Unchecked : Qt::Checked);

        cb->setData(Qt::UserRole, name);
        row++;
    }
    m_dialog->enabled_os->horizontalHeader()->setStretchLastSection(true);
    m_dialog->enabled_os->sortItems(0);
    m_dialog->enabled_os->update();
}

QString PrefsDialog::getFontDescription(const QFont &font)
{
    ostringstream str;
    str << font.family().toLatin1().constData()
        << " " << font.pointSize();
    return QString(str.str().c_str());
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

void PrefsDialog::changeAddColor()
{
    changeColor(m_dialog->addColorBtn, FWBSettings::ADD_COLOR);
}

void PrefsDialog::changeEditColor()
{
    changeColor(m_dialog->editColorBtn, FWBSettings::EDIT_COLOR);
}

void PrefsDialog::changeMoveColor()
{
    changeColor(m_dialog->moveColorBtn, FWBSettings::MOVE_COLOR);
}

void PrefsDialog::changeRemoveColor()
{
    changeColor(m_dialog->removeColorBtn, FWBSettings::REMOVE_COLOR);
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
    changeFont(rulesFont);
    m_dialog->rulesFontDescr->setText(getFontDescription(rulesFont));
}

void PrefsDialog::changeTreeFont()
{
    changeFont(treeFont);
    m_dialog->treeFontDescr->setText(getFontDescription(treeFont));
}

void PrefsDialog::changeCompilerOutputFont()
{
    changeFont(compilerOutputFont);
    m_dialog->compilerOutputFontDescr->setText(getFontDescription(compilerOutputFont));
}

void PrefsDialog::changeFont(QFont &font)
{
    bool ok;
    QFont f = QFontDialog::getFont(&ok, font, this);
    if (ok)
    {
        font = f;
    }
}

void PrefsDialog::findWDir()
{
    QString wd = m_dialog->wDir->text();
    if (wd.isEmpty()) wd = st->getWDir();
    if (wd.isEmpty()) wd = st->getOpenFileDir();
    QString dir = QFileDialog::getExistingDirectory(
            this, tr("Find working directory"), wd, QFileDialog::ShowDirsOnly);

    if (dir.isEmpty()) return;
    st->setOpenFileDir(dir);

    m_dialog->wDir->setText(dir);
}

void PrefsDialog::findDataDir()
{
    QString dataDir = m_dialog->dataDir->text();
    if (dataDir.isEmpty()) dataDir = st->getDataDir();
    if (dataDir.isEmpty()) dataDir = st->getOpenFileDir();
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Find data directory"), dataDir, QFileDialog::ShowDirsOnly);

    if (dir.isEmpty()) return;
    st->setOpenFileDir(dir);

    m_dialog->dataDir->setText(dir);
}

void PrefsDialog::findSSH()
{
    QString sshPath = m_dialog->sshPath->text();
    if (!QFileInfo(sshPath).isFile()) sshPath = st->getSSHPath();
    if (!QFileInfo(sshPath).isFile()) sshPath = st->getOpenFileDir();

    QString fp = QFileDialog::getOpenFileName(
        this, tr("Find Secure Shell utility"), sshPath);

    if (fp.isEmpty()) return;
    st->setOpenFileDir(fp);

    m_dialog->sshPath->setText(fp);
}

void PrefsDialog::findSCP()
{
    QString scpPath = m_dialog->scpPath->text();
    if (!QFileInfo(scpPath).isFile()) scpPath = st->getSCPPath();
    if (!QFileInfo(scpPath).isFile()) scpPath = st->getOpenFileDir();

    QString fp = QFileDialog::getOpenFileName(
        this, tr("Find SCP utility"), scpPath);

    if (fp.isEmpty()) return;
    st->setOpenFileDir(fp);

    m_dialog->scpPath->setText(fp);
}

void PrefsDialog::findDiff()
{
    QString diffPath = m_dialog->diffPath->text();
    if (!QFileInfo(diffPath).isFile()) diffPath = st->getDiffPath();
    if (!QFileInfo(diffPath).isFile()) diffPath = st->getOpenFileDir();

    QString fp = QFileDialog::getOpenFileName(
        this, tr("Find Diff utility"), diffPath);

    if (fp.isEmpty()) return;
    st->setOpenFileDir(fp);

    m_dialog->diffPath->setText(fp);
}

void PrefsDialog::accept()
{
    QString wd=m_dialog->wDir->text();

/* check if the default working directory does not exist yet */

    st->setWDir( wd );
    st->setDataDir(m_dialog->dataDir->text());

    st->setObjTooltips( m_dialog->objTooltips->isChecked() );
    st->setBool("UI/AdvancedTooltips", m_dialog->advTooltipMode->isChecked());

    st->setCustomTemplatesEnabled(m_dialog->enableCustomTemplates->isChecked());

//  st->setTooltipDelay( m_dialog->tooltipDelay->value() );
//  QToolTip::setWakeUpDelay( st->getTooltipDelay()*1000 );

    st->setBool("UI/ShowDeletedObjects", m_dialog->deletedObj->isChecked());
    st->setBool("UI/ShowObjectsAttributesInTree", m_dialog->attributesInTree->isChecked());

    st->setIconsWithText(m_dialog->toolbarIconsText->isChecked());

    ProjectPanel *pp = mw->activeProject();
    if (pp)
    {
        pp->m_panel->om->setAttributesColumnEnabled(
            m_dialog->attributesInTree->isChecked());
        if (m_dialog->attributesInTree->isChecked())
            pp->m_panel->om->loadSectionSizes();
    }

    st->setBool("Objects/DNSName/useCompileTimeForNewObjects",
                m_dialog->new_dns_name_compile_tm->isChecked());
    st->setBool("Objects/DNSName/useNameForDNSRecord",
                m_dialog->use_name_for_dns_record->isChecked());

    st->setBool("Objects/AddressTable/useCompileTimeForNewObjects",
                m_dialog->new_addr_tbl_compile_tm->isChecked());

    st->setBool("Objects/PolicyRule/defaultLoggingState",
                m_dialog->rulesLoggingOn->isChecked());

    st->setBool("Objects/PolicyRule/defaultStateful",
                m_dialog->rulesDefaultStateful->isChecked());

    st->setInt("Objects/PolicyRule/defaultAction",
                m_dialog->rulesDefaultAction->currentIndex());

    st->setInt("Objects/PolicyRule/defaultDirection",
                m_dialog->rulesDefaultDirection->currentIndex());

    st->setInt("Objects/PolicyRule/defaultSource",
                m_dialog->rulesDefaultSource->currentIndex());

    st->setInt("Objects/PolicyRule/defaultDestination",
                m_dialog->rulesDefaultDestination->currentIndex());

    st->setInt("Objects/PolicyRule/defaultService",
                m_dialog->rulesDefaultService->currentIndex());

    st->setInt("Objects/PolicyRule/defaultInterface",
                m_dialog->rulesDefaultInterface->currentIndex());

    st->setBool("Objects/Interface/autoconfigureInterfaces",
                m_dialog->autoconfigure_interfaces->isChecked());


    st->setRCSLogState( m_dialog->emptyRCSLog->isChecked() );

    st->setBool("Environment/autoSaveFile", m_dialog->autosaveFile->isChecked());
    st->setInt("Environment/autoSaveFilePeriod", m_dialog->autosaveInterval->value() );

    st->setCompression(m_dialog->dataFileCompression->isChecked());

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
    st->setShowDirectionText(m_dialog->showDirectionText->isChecked());
    FWBSettings::IconSize sz = m_dialog->rb25->isChecked() ?
        FWBSettings::SIZE25X25 : FWBSettings::SIZE16X16;
    st->setIconsInRulesSize(sz);

    st->setRulesFont(rulesFont);
    st->setTreeFont(treeFont);
    st->setUiFont(uiFont);
    st->setCompilerOutputFont(compilerOutputFont);

    st->setClipComment(m_dialog->chClipComment->isChecked());

    st->setCheckUpdatesProxy(m_dialog->checkUpdatesProxy->text());

    // annoyingly, widget shotTip has the name opposite to its meaning.
    // When it is checked, we do not show tip of the day.

    st->setBool("UI/NoStartTip", m_dialog->showTips->isChecked());

    st->setSSHPath( m_dialog->sshPath->text() );
    st->setSCPPath( m_dialog->scpPath->text() );
    st->setSSHTimeout(m_dialog->sshTimeout->value());

    st->setBool("/Diff/AutoCompile", m_dialog->autoCompileChk->isChecked());
    st->setDiffPath( m_dialog->diffPath->text() );

    st->setBool("Environment/RememberSshPassEnabled", m_dialog->rememberSshPass->isChecked());
    
    st->setCheckUpdates(m_dialog->checkUpdates->isChecked());

    st->setDiffColor(FWBSettings::ADD_COLOR,    colors[FWBSettings::ADD_COLOR]);
    st->setDiffColor(FWBSettings::EDIT_COLOR,   colors[FWBSettings::EDIT_COLOR]);
    st->setDiffColor(FWBSettings::MOVE_COLOR,   colors[FWBSettings::MOVE_COLOR]);
    st->setDiffColor(FWBSettings::REMOVE_COLOR, colors[FWBSettings::REMOVE_COLOR]);

    st->setDisplayUnmodifiedRules( m_dialog->displayUnmodifiedRulesChk->isChecked() );

    for (int row=0; row < m_dialog->enabled_platforms->rowCount(); ++row)
    {
        QTableWidgetItem *itm = m_dialog->enabled_platforms->item(row, 0);
        st->setTargetStatus(itm->data(Qt::UserRole).toString(),
                            (itm && itm->checkState() == Qt::Unchecked) ?
                            "disabled" : "active");
    }

    QStringList disabled_os;
    for (int row=0; row < m_dialog->enabled_os->rowCount(); ++row)
    {
        QTableWidgetItem *itm = m_dialog->enabled_os->item(row, 0);
        st->setTargetStatus(itm->data(Qt::UserRole).toString(),
                            (itm && itm->checkState() == Qt::Unchecked) ?
                            "disabled" : "active");
    }

    if (!wd.isEmpty())
    {
        QDir d;
        d.mkdir( wd );
    }

    if (pp)
    {
        RuleSetView* rsv = pp->getCurrentRuleSetView();
        if (rsv)
            rsv->updateAll();
    }

    mw->setupAutoSave();
    mw->showDeletedObjects(st->getBool("UI/ShowDeletedObjects"));
    mw->updateTreeFont();
    mw->setupGlobalToolbar();
//    app->setFont(st->getTreeFont());


    QDialog::accept();
}

void PrefsDialog::objTooltipsEnabled(bool enabled)
{
    if (!enabled && m_dialog->advTooltipMode->isChecked())
        m_dialog->advTooltipMode->setChecked(false);
    m_dialog->advTooltipMode->setEnabled(enabled);
}

void PrefsDialog::selectTab(const QString &name)
{
    for (int i = m_dialog->tabWidget->count(); i >= 0; i--) {
        if (m_dialog->tabWidget->tabText(i) == name) {
            m_dialog->tabWidget->setCurrentIndex(i);
            return;
        }
    }
}
