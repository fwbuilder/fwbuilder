/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include "FWBSettings.h"
#include "FWWindow.h"
#include "ObjectManipulator.h"

#include "fwbuilder/FWObjectDatabase.h"

#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include <QDir>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QUuid>
#include <QRegExp>
#include <QtDebug>
#include <QCryptographicHash>
#include <QTime>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#else
#  include <unistd.h>     // for access(2)
#endif

#include <iostream>

using namespace std;
using namespace libfwbuilder;


const char* DTDSetpath = SETTINGS_PATH_PREFIX "/System/DTDPath";
const char* ResSetpath = SETTINGS_PATH_PREFIX "/System/ResPath";
const char* compression = SETTINGS_PATH_PREFIX "/DataFile/compression";
const char* wdirSetpath = SETTINGS_PATH_PREFIX "/Environment/WDir";
const char* datadirSetpath = SETTINGS_PATH_PREFIX "/Environment/DataDir";
const char* ofdirSetpath = SETTINGS_PATH_PREFIX "/Environment/OpenFileDir";
const char* startupActionSetpath =
    SETTINGS_PATH_PREFIX "/Environment/StartupAction";
const char* labelColorPath = SETTINGS_PATH_PREFIX "/ColorLabels/color_";
const char* labelTextPath = SETTINGS_PATH_PREFIX "/ColorLabels/text_";
const char* diffColorPath = SETTINGS_PATH_PREFIX "/Diff/color_";
const char* lastEditedSetpath = SETTINGS_PATH_PREFIX "/Environment/LastEdited";
const char* autoSave = SETTINGS_PATH_PREFIX "/Environment/autoSave";
const char* expandTreeSetpath = SETTINGS_PATH_PREFIX "/UI/ExpandTree";
const char* MergeLibsSetpath = SETTINGS_PATH_PREFIX "/UI/MergeLibraries";
const char* infoStyleSetpath = SETTINGS_PATH_PREFIX "/UI/InfoWindowStyle";
const char* infoWindowHSetpath = SETTINGS_PATH_PREFIX "/UI/InfoWindowHeight";
const char* groupModeSetpath = SETTINGS_PATH_PREFIX "/UI/GroupViewMode";
const char* groupColsSetpath = SETTINGS_PATH_PREFIX "/UI/GroupViewColumns";
const char* customTemplatesEn =
    SETTINGS_PATH_PREFIX "/UI/customTemplatesEnabled";
const char* objTooltips = SETTINGS_PATH_PREFIX "/UI/objTooltips";
const char* tooltipDelay = SETTINGS_PATH_PREFIX "/UI/tooltipDelay";
const char* showUndoPanel = SETTINGS_PATH_PREFIX "/UI/showUndoPanel";
const char* userWorkflowFlags = SETTINGS_PATH_PREFIX "/UI/userWorkFlowFlags";
const char* iconsWithText = SETTINGS_PATH_PREFIX "/UI/IconWithText";
const char* emptyRCSLog = SETTINGS_PATH_PREFIX "/RCS/emptyLog";
const char* rcsFilePreviewStyle = SETTINGS_PATH_PREFIX "/RCS/FilePreviewStyle";
const char* rcsFilePreviewSortColumn =
    SETTINGS_PATH_PREFIX "/RCS/FilePreviewSortColumn";
const char* dontSaveStdLib = SETTINGS_PATH_PREFIX "/DataFormat/dontSaveStdLib";
const char* WindowGeometrySetpath= SETTINGS_PATH_PREFIX "/Layout/";
const char* screenPositionSetpath= SETTINGS_PATH_PREFIX "/ScreenPos/";
const char* showIconsInRules = SETTINGS_PATH_PREFIX "/UI/Icons/ShowIconsInRules";
const char* showDirectionText =
    SETTINGS_PATH_PREFIX "/UI/Icons/ShowDirectionTextInRules";
const char* iconsInRulesSize = SETTINGS_PATH_PREFIX "/UI/Icons/IconsInRulesSize";
const char* rulesFont = SETTINGS_PATH_PREFIX "/UI/Fonts/RulesFont";
const char* treeFont = SETTINGS_PATH_PREFIX "/UI/Fonts/TreeFont";
const char* uiFont = SETTINGS_PATH_PREFIX "/UI/Fonts/UiFont";
const char* compilerOutputFont = 
    SETTINGS_PATH_PREFIX "/UI/Fonts/CompilerOutputFont";
const char* clipComment = SETTINGS_PATH_PREFIX "/UI/ClipComment";
const char* checkUpdates = SETTINGS_PATH_PREFIX "/UI/CheckUpdates";
const char* updateAvailableWarningLastTime =
    SETTINGS_PATH_PREFIX "/UI/updateAvailableWarningLastTime";
const char* announcementLastTime =
    SETTINGS_PATH_PREFIX "/UI/announcementLastTime/%1";
const char* checkUpdatesProxy = SETTINGS_PATH_PREFIX "/UI/CheckUpdatesProxy";
const char* reminderAboutStandardLibSuppressed =
    SETTINGS_PATH_PREFIX "/UI/reminderAboutStandardLibSuppressed";
const char* reminderDataDir = SETTINGS_PATH_PREFIX "/UI/reminderDataDir";
const char* introDialogEnabled = SETTINGS_PATH_PREFIX "/UI/introDialogEnabled";
const char* newFirewallPlatform =
    SETTINGS_PATH_PREFIX "/Objects/NewFireallPlatform";
const char* newClusterFailoverProtocol =
    SETTINGS_PATH_PREFIX "/Objects/newClusterFailoverProtocol";
const char* abTestingGroup = SETTINGS_PATH_PREFIX "/abTestingGroup";
const char* startsCounter = SETTINGS_PATH_PREFIX "/startsCounter";
const char* targetStatus = SETTINGS_PATH_PREFIX "/TargetStatus/";
const char* SSHPath = SETTINGS_PATH_PREFIX "/SSH/SSHPath";
const char* SCPPath = SETTINGS_PATH_PREFIX "/SSH/SCPPath";
const char* DiffPath = SETTINGS_PATH_PREFIX "/Diff/DiffPath";

const char* appGUID = "/fwbuilder_gui/ApplicationGUID";
const char* appGUID_4_0 = "/4.0/ApplicationGUID";
const char* appGUID_4_1 = "/4.1/ApplicationGUID";

#ifdef _WIN32
const char* SSHTimeout = "Sessions/fwb_session_with_keepalive/PingIntervalSecs";
#else
const char* SSHTimeout = SETTINGS_PATH_PREFIX "/SSH/SSHTimeout";
#endif

const char * displayUnmodifiedRules = SETTINGS_PATH_PREFIX "/Diff/displayUnmodifiedRules";


/**
 * Settings path defined here should match Windows registry paths used
 * in the Windows installer/uninstaller scripts.
 *
 * Path used for uuid_settings should not include version to ensure
 * uuid persistence across upgrades. This means do not use getApplicationNameForSettings()
 */
FWBSettings::FWBSettings(bool testData) :
    QSettings(QSettings::UserScope,
              "netcitadel.com",
              testData?"fwb_test_data":getApplicationNameForSettings())
{
    if (testData)
    {
        this->clear();
    }
    uuid_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                  "netcitadel.com", "FirewallBuilder");
#ifdef _WIN32
    ssh_timeout_setings_object = new QSettings(QSettings::UserScope,
                                                 "SimonTatham", "PuTTY");
#else
    ssh_timeout_setings_object = this;
#endif
}

FWBSettings::~FWBSettings()
{
    delete uuid_settings;
#ifdef _WIN32
    delete ssh_timeout_setings_object;
#endif
}

/**
 *  to preserve behavior of the old versions of fwbuilder on Unix, the
 *  default working dir is set to "." - current dir.
 *
 *  On Windows default working dir is set to
 *             "Documents and settings/USERNAME/Firewalls"
 */
void FWBSettings::init(bool force_first_time_run)
{
    bool ok = false;
    first_run = false;

    ok = contains(reminderAboutStandardLibSuppressed);
    if (!ok)
        suppressReminderAboutStandardLib(true);

    ok = uuid_settings->contains(appGUID);
    if (!ok)
    {
        ok = uuid_settings->contains(appGUID_4_1);
        if (ok)
        {
            uuid_settings->setValue(
                appGUID, uuid_settings->value(appGUID_4_1).toString());
            uuid_settings->remove(appGUID_4_1);
        } else
        {
            ok = uuid_settings->contains(appGUID_4_0);
            if (ok) 
            {
                uuid_settings->setValue(
                    appGUID, uuid_settings->value(appGUID_4_0).toString());
                uuid_settings->remove(appGUID_4_0);
            } else
            {
                qsrand(time(nullptr));
                uuid_settings->setValue(appGUID, QUuid::createUuid().toString());
                first_run = true;
            }
        }
    }

    if (force_first_time_run) first_run = true;

    if (first_run)
    {
        suppressReminderAboutStandardLib(false);
    } else
    {
        // enable custom templates for existing users for backwards
        // compatibility. New users will have this disabled for simplicity.
        ok = contains(customTemplatesEn);
        if (!ok)
            setCustomTemplatesEnabled(true);
    }

    ok = contains(abTestingGroup);
    if (!ok)
    {
        // a/b group codes are "1" and "2"
        setABTestingGroup(QTime::currentTime().second() % 2 + 1);
    }

    ok = contains(introDialogEnabled);
    if (!ok)
    {
        setIntroDialogEnabled(true);
    }

    ok = contains(startsCounter);
    if (!ok)
    {
        setValue(startsCounter, 0);
    }

    setValue(startsCounter, getStartsCounter() + 1);
    // disable invitation to watch quick start guide after 5 starts
    if (getStartsCounter() > 5) setIntroDialogEnabled(false);

    /*
     * I am seeing two particular uuids a lot in the logs, both coming
     * from thousands of different instances all over the world. I
     * have no idea why so many different systems assigned themselves
     * the same uuid which is supposed to be random and
     * unique. Apparently QUuid::createUuid() returns predictable uuid
     * in some cases.  Versions of the program before 4.0.2 did not
     * call qsrand() to re-seed random generator because Qt
     * documentation says that createUuid() does that. Interestingly,
     * if I run google search for the first of the two uuids, I get
     * some results which means exactly the same uuid was generated by
     * an unrelated program in some completely unrelated case.
     * Anyway, I am going to "flush" these two repeatable uuids to
     * reduce systematic error in counting how many instances of
     * fwbuilder are running out there.
     */
    QString my_uuid = getAppGUID();
    if (my_uuid == "b7203c47-06bf-4878-9ff5-6afffb2db546" ||
        my_uuid == "46759a87-7956-431f-a171-ccb754ef239e")
    {
        qsrand(time(nullptr));
        uuid_settings->setValue(appGUID, QUuid::createUuid().toString());
    }


    // By default sort RCS File preview by date, which is column 1
    ok = contains(rcsFilePreviewSortColumn);
    if (!ok) setRCSFilePreviewSortColumn(1);

    ok = contains(infoStyleSetpath);
    if (!ok) setValue(infoStyleSetpath,2);

    ok = contains(infoWindowHSetpath);
    if (!ok) setValue(infoWindowHSetpath,200);

    ok = contains(dontSaveStdLib);
    if (!ok) setDontSaveStdLib(true);

    ok = contains(startupActionSetpath);
    if (!ok) setStartupAction(2);

    ok = contains(showDirectionText);
    if (!ok) setShowDirectionText(true);

#ifdef _WIN32
    QString wd = getWDir().replace('/','\\');
#else
    QString wd = getWDir();
#endif
    if ( ! wd.isEmpty())
    {
        QDir wdir(wd);
        if (!wdir.exists() && !wdir.mkdir(wd))
        {
            QString err = QString(QObject::tr("Working directory %1 does not exist and could not be created.\nIgnoring this setting.")).arg(wd);;

            if (app != nullptr)
            {
                QMessageBox::critical( 0,"Firewall Builder", err,
                                       "&Continue", 0, 0, 0 );
            } else
            {
                qDebug() << err;
            }
            setWDir("");
        }
    }

    ok = contains(objTooltips);
    if (!ok) setObjTooltips(true);

    QString c;
    if (getLabelColor(RED   ).isEmpty())
    { setLabelColor(RED   ,"#C86E6E"); setLabelText(RED,"Red"); }
    if (getLabelColor(ORANGE).isEmpty())
    { setLabelColor(ORANGE,"#C08B5A"); setLabelText(ORANGE,"Orange"); }
    if (getLabelColor(YELLOW).isEmpty())
    { setLabelColor(YELLOW,"#C0BA44"); setLabelText(YELLOW,"Yellow"); }
    if (getLabelColor(GREEN ).isEmpty())
    { setLabelColor(GREEN ,"#8BC065"); setLabelText(GREEN ,"Green"); }
    if (getLabelColor(BLUE  ).isEmpty())
    { setLabelColor(BLUE  ,"#7694C0"); setLabelText(BLUE  ,"Blue"); }
    if (getLabelColor(PURPLE).isEmpty())
    { setLabelColor(PURPLE,"#A37EC0"); setLabelText(PURPLE,"Purple"); }
    if (getLabelColor(GRAY  ).isEmpty())
    { setLabelColor(GRAY  ,"#C0C0C0"); setLabelText(GRAY  ,"Gray"); }

    if (getDiffColor(ADD_COLOR).isEmpty())
    { setDiffColor(ADD_COLOR,"#8BC065"); }
    if (getDiffColor(EDIT_COLOR).isEmpty())
    { setDiffColor(EDIT_COLOR,"#7694C0"); }
    if (getDiffColor(MOVE_COLOR).isEmpty())
    { setDiffColor(MOVE_COLOR,"#C0C0C0"); }
    if (getDiffColor(REMOVE_COLOR).isEmpty())
    { setDiffColor(REMOVE_COLOR,"#C86E6E"); }

    ok = contains(showIconsInRules);
    if (!ok) setShowIconsInRules(true);

    ok = contains(iconsInRulesSize);
    if (!ok) setIconsInRulesSize(SIZE25X25);

    ok = contains(rulesFont);
    if (!ok) setRulesFont(QApplication::font());

    ok = contains(treeFont);
    if (!ok) setTreeFont(QApplication::font());

    ok = contains(uiFont);
    if (!ok) setUiFont(QApplication::font());

    ok = contains(compilerOutputFont);
    if (!ok) setCompilerOutputFont(QApplication::font());

    if (fwbdebug)
        qDebug() << "Default application font:"
                 << QApplication::font();

    ok = contains(iconsWithText);
    if (!ok) setIconsWithText(true);

    ok = contains(clipComment);
    if (!ok) setClipComment(true);

    ok = contains(checkUpdates);
    if (!ok)
    {
        setCheckUpdates(true);
        setTimeOfLastUpdateAvailableWarning(0);
    }

    ok = contains(compression);
    if (!ok) setCompression(false);

#ifndef _WIN32
    if (getSSHPath().isEmpty()) setSSHPath("ssh");
    if (getSCPPath().isEmpty()) setSCPPath("scp");
#endif
    // default timeout is 30 sec (default value of ServerAliveCountMax is 3)
    // do this for both Linux and windows !
    if (!haveSSHTimeout()) setSSHTimeout(10);

    // Note: hasKey calls QSettings::contains using path given as
    // argument, prepended with SETTINGS_PATH_PREFIX
    if (!hasKey("Window/maximized")) setInt("Window/maximized", 1);

    if (!hasKey("Objects/DNSName/useCompileTimeForNewObjects"))
        setBool("Objects/DNSName/useCompileTimeForNewObjects", true);

    if (!hasKey("Objects/DNSName/useNameForDNSRecord"))
        setBool("Objects/DNSName/useNameForDNSRecord", false);

    if (!hasKey("Objects/AddressTable/useCompileTimeForNewObjects"))
        setBool("Objects/AddressTable/useCompileTimeForNewObjects", true);

    if (!hasKey("Objects/PolicyRule/defaultLoggingState"))
        setBool("Objects/PolicyRule/defaultLoggingState", true);

    if (!hasKey("Objects/PolicyRule/defaultStateful"))
        setBool("Objects/PolicyRule/defaultStateful", true);

    if (!hasKey("Objects/PolicyRule/defaultAction"))
        setInt("Objects/PolicyRule/defaultAction", 0);

    if (!hasKey("Objects/PolicyRule/defaultDirection"))
        setInt("Objects/PolicyRule/defaultDirection", 0);

    if (!hasKey("Objects/PolicyRule/defaultInterface"))
        setInt("Objects/PolicyRule/defaultInterface", 0);

    if (!hasKey("Objects/Interface/autoconfigureInterfaces"))
        setBool("Objects/Interface/autoconfigureInterfaces", true);

}

bool FWBSettings::isReminderAboutStandardLibSuppressed()
{
    return value(reminderAboutStandardLibSuppressed).toBool();
}

void FWBSettings::suppressReminderAboutStandardLib(bool f)
{
    setValue(reminderAboutStandardLibSuppressed, f);
}

bool FWBSettings::isReminderAboutDataDirSuppressed()
{
    return value(reminderDataDir).toBool();
}

void FWBSettings::suppressReminderAboutDataDir(bool f)
{
    setValue(reminderDataDir, f);
}

bool FWBSettings::hasKey(const QString &attribute)
{
    return QSettings::contains(SETTINGS_PATH_PREFIX "/" + attribute);
}

QString FWBSettings::getAppGUID()
{
    return uuid_settings->value(appGUID).toString();
}

QString FWBSettings::getStr(const QString &attribute)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    return value(path).toString();
}

void FWBSettings::setStr(const QString &attribute,
                            const QString &val)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    setValue(path,val);
}

bool FWBSettings::getBool(const QString &attribute)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    return value(path).toBool();
}

void FWBSettings::setBool(const QString &attribute, bool val )
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    setValue(path,val);
}

int FWBSettings::getInt(const QString &attribute)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    return value(path).toInt();
}

void FWBSettings::setInt(const QString &attribute, int val )
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    setValue(path,val);
}

QStringList FWBSettings::getList(const QString &attribute)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    return value(path).toStringList();
}

void FWBSettings::setList(const QString &attribute, QStringList &list)
{
    QString path = SETTINGS_PATH_PREFIX "/" + attribute;
    setValue(path, list);
}

QString FWBSettings::getWDir() { return value(wdirSetpath).toString();}
void FWBSettings::setWDir(const QString &wd) { setValue(wdirSetpath, wd);}
QString FWBSettings::getDataDir() { return value(datadirSetpath).toString();}
void FWBSettings::setDataDir(const QString &d) {
    setValue(datadirSetpath, d);
    FWObject::setDataDir(d.toUtf8().constData());
}
int FWBSettings::getInfoStyle() { return value(infoStyleSetpath).toInt();}
void FWBSettings::setInfoStyle(int s) { setValue(infoStyleSetpath,s);}
int FWBSettings::getInfoWindowHeight() { return value(infoWindowHSetpath).toInt();}
void FWBSettings::setInfoWindowHeight(int h) { setValue(infoWindowHSetpath,h);}

QString FWBSettings::getGroupViewMode() { return value(groupModeSetpath).toString();}
void FWBSettings::setGroupViewMode(const QString &m) { setValue(groupModeSetpath,m);}

QString FWBSettings::getGroupViewColumns() { return value(groupColsSetpath).toString();}
void FWBSettings::setGroupViewColumns(const QString &m) { setValue(groupColsSetpath,m);}


int FWBSettings::getStartupAction() { return value(startupActionSetpath).toInt();}
void FWBSettings::setStartupAction(int sa) { setValue( startupActionSetpath , sa );}

int FWBSettings::getExpandTree() {  return value(expandTreeSetpath).toInt(); }
void FWBSettings::setExpandTree(int f) { setValue( expandTreeSetpath , f ); }

int FWBSettings::getMergeLibs() {  return value(MergeLibsSetpath).toInt(); }
void FWBSettings::setMergeLibs(int f) { setValue( MergeLibsSetpath , f ); }

bool FWBSettings::getObjTooltips() {  return value( objTooltips ).toBool();}
void FWBSettings::setObjTooltips(bool f) {  setValue( objTooltips, f); }

int FWBSettings::getTooltipDelay() { return value( tooltipDelay ).toInt(); }
void FWBSettings::setTooltipDelay(int v) { setValue( tooltipDelay, v); }


QString FWBSettings::getLastEdited() { return value(lastEditedSetpath).toString();}
void FWBSettings::setLastEdited(const QString &file) { setValue(lastEditedSetpath,file);}

QString FWBSettings::getOpenFileDir(const QString &existingPath)
{
    QString ret = getWDir();
    if (!ret.isEmpty() && QFileInfo(ret).isDir()) return ret;

    ret = value(ofdirSetpath).toString();
    if (!ret.isEmpty() && QFileInfo(ret).isDir()) return ret;

    if (!existingPath.isEmpty()) {
        ret = getFileDir(existingPath);
        if (QFileInfo(ret).isDir()) return ret;
    }

    return userDataDir.c_str();
}

void FWBSettings::setOpenFileDir(const QString &d)
{
    QString dirPath = d;

    QFileInfo info(d);
    if (!info.isDir()) {
        dirPath = info.dir().path();
    }

    setValue(ofdirSetpath, dirPath);
}

void FWBSettings::save()
{
}

bool FWBSettings::getRCSLogState() { return value( emptyRCSLog ).toBool(); }
void FWBSettings::setRCSLogState(bool f) { setValue( emptyRCSLog , f ); }

int  FWBSettings::getRCSFilePreviewStyle()
{
    return value(rcsFilePreviewStyle).toInt();
}

void FWBSettings::setRCSFilePreviewStyle(int style)
{
    setValue(rcsFilePreviewStyle, style);
}

int  FWBSettings::getRCSFilePreviewSortColumn()
{
    return value(rcsFilePreviewSortColumn).toInt();
}

void FWBSettings::setRCSFilePreviewSortColumn(int col)
{
    setValue(rcsFilePreviewSortColumn, col);
}

bool FWBSettings::getAutoSave() { return value( autoSave ).toBool(); }
void FWBSettings::setAutoSave(bool f) { setValue( autoSave, f); }

bool FWBSettings::getCompression() { return value(compression).toBool(); }
void FWBSettings::setCompression(bool f) { setValue(compression, f); }

bool FWBSettings::getDontSaveStdLib() {return value(dontSaveStdLib).toBool();}
void FWBSettings::setDontSaveStdLib( bool f) { setValue(dontSaveStdLib,f);}

bool FWBSettings::getShowUndoPanel() {return value(showUndoPanel).toBool();}
void FWBSettings::setShowUndoPanel(bool f) {setValue(showUndoPanel, f);}

bool FWBSettings::getIconsWithText() { return value(iconsWithText).toBool(); }
void FWBSettings::setIconsWithText(bool f) {setValue(iconsWithText, f);}

bool FWBSettings::haveScreenPosition(const QString &wname)
{
    QString val = value(QString(screenPositionSetpath)+wname ).toString();
    bool res=(!val.isEmpty());

    if (fwbdebug)
    {
    qDebug("FWBSettings::haveScreenPosition wname '%s' ret=%d",
        wname.toLatin1().constData(), res);
    }

    return res;
}

QPoint FWBSettings::getScreenPosition(const QString &wname)
{
    QString val = value(QString(screenPositionSetpath)+wname ).toString();
    int     x  = val.section(',',0,0).toInt();
    int     y  = val.section(',',1,1).toInt();
    int  width = 150;  // won't get closer to the screen edge than this
    int height = 150;

    QDesktopWidget *d = QApplication::desktop();
// get geometry of the screen that contains mw
    QRect sg = d->screenGeometry(mw);

    if (x+width > sg.width())   x=sg.width()-width;
    if (y+height > sg.height()) y=sg.height()-height;
    if (x<0)                    x=(sg.width()-width)/2;
    if (y<0)                    y=(sg.height()-height)/2;

    if (fwbdebug)
    {
    qDebug("FWBSettings::getScreenPosition wname '%s' x=%d y=%d",
        wname.toLatin1().constData(), x,y );
    }

    return QPoint(x,y);
}

void FWBSettings::saveScreenPosition(const QString &wname, const QPoint &p)
{
    int x = p.x();
    int y = p.y();
    if (x<0) x=0;
    if (y<0) y=0;

    QString val =QString("%1,%2").arg(x).arg(y);

    if (fwbdebug)
    {
    qDebug("FWBSettings::saveScreenPosition wname '%s' x=%d y=%d",
        wname.toLatin1().constData(), x,y );
    }

    setValue(QString(screenPositionSetpath)+wname, val );
}

bool FWBSettings::haveGeometry(QWidget *w)
{
    QString name=w->objectName();
    QString val = value(QString(WindowGeometrySetpath)+name,"").toString();
    return (!val.isEmpty());
}

void FWBSettings::restoreGeometry(QWidget *w)
{
    QString name=w->objectName();
    QString val = value(QString(WindowGeometrySetpath)+name ).toString();
    int     x      = val.section(',',0,0).toInt();
    int     y      = val.section(',',1,1).toInt();
    int     width  = val.section(',',2,2).toInt();
    int     height = val.section(',',3,3).toInt();

    QDesktopWidget *d = QApplication::desktop();
// get geometry of the screen that contains mw
    QRect sg = d->screenGeometry(mw);

    if (width > sg.width() || height > sg.height())
    {
        w->showMaximized();
        return;
    }

    if (x+width > sg.width())   x=sg.width()-width;
    if (y+height > sg.height()) y=sg.height()-height;
    if (x<0)                    x=(sg.width()-width)/2;
    if (y<0)                    y=(sg.height()-height)/2;

    if (fwbdebug)
    {
        qDebug("FWBSettings::restoreGeometry  widget '%s' vis=%d x=%d y=%d",
                   name.toLatin1().constData(), w->isVisible(), x,y);
    }

    w->resize( QSize(width,height) );
}

void FWBSettings::restoreGeometry(QWidget *w, const QRect &dg)
{
    QString name=w->objectName();
    QString defval =QString("%1,%2,%3,%4")
        .arg(dg.x()).arg(dg.y()).arg(dg.width()).arg(dg.height());

    QString val = value(QString(WindowGeometrySetpath)+name , defval ).toString();
    int     x      = val.section(',',0,0).toInt();
    int     y      = val.section(',',1,1).toInt();
    int     width  = val.section(',',2,2).toInt();
    int     height = val.section(',',3,3).toInt();

    QDesktopWidget *d = QApplication::desktop();
// get geometry of the screen that contains mw
    QRect sg = d->screenGeometry(mw);

    if (width > sg.width() || height > sg.height())
    {
        w->showMaximized();
        return;
    }

    if (x+width > sg.width())   x=sg.width()-width;
    if (y+height > sg.height()) y=sg.height()-height;
    if (x<0)                    x=(sg.width()-width)/2;
    if (y<0)                    y=(sg.height()-height)/2;

    if (fwbdebug)
    {
        qDebug("FWBSettings::restoreGeometry  widget '%s' vis=%d x=%d y=%d",
                   name.toLatin1().constData(), w->isVisible(), x,y);
    }

    w->resize( QSize(width,height) );
}

void FWBSettings::saveGeometry(QWidget *w)
{
    QString name = w->objectName();
//    QRect   g = w->geometry();
//    g.moveTopLeft(w->frameGeometry().topLeft());
    QPoint p = w->pos();
    QSize  s = w->size();

    int x = p.x();
    int y = p.y();
    if (x<0) x=0;
    if (y<0) y=0;

    QString val =QString("%1,%2,%3,%4")
        .arg(p.x())
        .arg(p.y())
        .arg(s.width())
        .arg(s.height());

    if (fwbdebug)
    {
        qDebug("FWBSettings::saveGeometry  widget '%s' vis=%d val=%s",
                   name.toLatin1().constData(), w->isVisible(), val.toLatin1().constData());
    }

    setValue(QString(WindowGeometrySetpath)+name, val);
}


QString FWBSettings::getLabelColorStr(enum LabelColors c)
{
    switch (c)
    {
    case RED:    return "red";
    case ORANGE: return "orange";
    case YELLOW: return "yellow";
    case GREEN:  return "green";
    case BLUE:   return "blue";
    case PURPLE: return "purple";
    case GRAY:   return "gray";
    default:     return "default";
    }
}

QString FWBSettings::getDiffColorStr(enum LabelColors c)
{
    switch (c)
    {
    case ADD_COLOR:     return "add";
    case EDIT_COLOR:    return "edit";
    case MOVE_COLOR:    return "move";
    case REMOVE_COLOR:  return "remove";
    default:            return "default";
    }
}

QString FWBSettings::getLabelColor(enum LabelColors c)
{
    return value(QString(labelColorPath) + getLabelColorStr(c)).toString();
}

void FWBSettings::setLabelColor(enum LabelColors c,const QString &s)
{
    setValue(QString(labelColorPath) + getLabelColorStr(c), s);
}

QString FWBSettings::getLabelText(enum LabelColors c)
{
    return value(QString(labelTextPath) + getLabelColorStr(c)).toString();
}

void FWBSettings::setLabelText(enum LabelColors c, const QString &s)
{
    setValue(QString(labelTextPath) + getLabelColorStr(c),s);
}

QString FWBSettings::getDiffColor(FWBSettings::LabelColors c)
{
    return value(QString(diffColorPath) + getDiffColorStr(c)).toString();
}

void FWBSettings::setDiffColor(FWBSettings::LabelColors c, const QString &s)
{
    setValue(QString(diffColorPath) + getDiffColorStr(c), s);
}

QString FWBSettings::getSSHPath()
{
    return value(SSHPath).toString();
}

void FWBSettings::setSSHPath(const QString &path)
{
    setValue(SSHPath,path);
}

QString FWBSettings::getSCPPath()
{
    return value(SCPPath).toString();
}

void FWBSettings::setSCPPath(const QString &path)
{
    setValue(SCPPath,path);
}

QString FWBSettings::getDiffPath()
{
    return value(DiffPath).toString();
}

void FWBSettings::setDiffPath(const QString &path)
{
    setValue(DiffPath,path);
}

// Putty uses different parameter name for the server alive interval
// and keeps it as part of the session, stored in registry. Using
// separate QSettings object on windows that controls putty session
// "fwb_session_with_keepalive". On all other platforms
// ssh_timeout_setings_object == this

bool FWBSettings::haveSSHTimeout()
{
    return ssh_timeout_setings_object->contains(SSHTimeout);
}

int FWBSettings::getSSHTimeout()
{
    return ssh_timeout_setings_object->value(SSHTimeout).toInt();
}

void FWBSettings::setSSHTimeout(int value_sec)
{
    ssh_timeout_setings_object->setValue(SSHTimeout, value_sec);
}





void FWBSettings::getPrinterOptions(QPrinter *printer,
                                    int &pageWidth,
                                    int &pageHeight)
{
    QString name = getStr("PrintSetup/printerName");
    if (!name.isEmpty())
        printer->setPrinterName(
            getStr("PrintSetup/printerName"));
#ifndef _WIN32
    printer->setPrinterSelectionOption(
        getStr("PrintSetup/printerSelectionOption"));
#endif
#ifndef Q_OS_MAC
    printer->setOutputFileName(getStr("PrintSetup/outputFileName"));
#endif
    printer->setOrientation(
        QPrinter::Orientation(getInt("PrintSetup/orientation")));
    printer->setPageSize(
        QPrinter::PageSize(getInt("PrintSetup/pageSize")));
    printer->setPageOrder(
        QPrinter::PageOrder(getInt("PrintSetup/pageOrder")));
//    int res = getInt("PrintSetup/resolution");
//    if (res>0) printer->setResolution(res);
    printer->setColorMode(
        QPrinter::ColorMode(getInt("PrintSetup/colorMode")));
    printer->setFullPage(
        getBool("PrintSetup/fullPage"));
//    printer->setFromTo(getInt("PrintSetup/fromPage"),getInt("PrintSetup/toPage"));
//    printer->setNumCopies(getInt("PrintSetup/numCopies"));

    pageWidth = getInt("PrintSetup/pageWidth");
    pageHeight = getInt("PrintSetup/pageHeight");
}

void FWBSettings::setPrinterOptions(QPrinter *printer,
                                    int pageWidth,
                                    int pageHeight)
{
    setStr("PrintSetup/printerName",printer->printerName());
#ifndef _WIN32
    setStr("PrintSetup/printerSelectionOption",
           printer->printerSelectionOption());
#endif
    setStr("PrintSetup/outputFileName",printer->outputFileName());
    setInt("PrintSetup/orientation",printer->orientation());
    setInt("PrintSetup/pageSize",printer->pageSize());
    setInt("PrintSetup/pageOrder",printer->pageOrder());
//    setInt("PrintSetup/resolution",printer->resolution());
    setInt("PrintSetup/colorMode",printer->colorMode());
    setBool("PrintSetup/fullPage",printer->fullPage());
//    setInt("PrintSetup/fromPage",printer->fromPage());
//    setInt("PrintSetup/toPage",printer->toPage());
//    setInt("PrintSetup/numCopies",printer->numCopies());

    setInt("PrintSetup/pageWidth",pageWidth);
    setInt("PrintSetup/pageHeight",pageHeight);
}


FWBSettings::IconSize FWBSettings::getIconsInRulesSize()
{
    QString val = value(QString(iconsInRulesSize)).toString();
    if ("SIZE25X25" == val)
        return SIZE25X25;
    if ("SIZE16X16" == val)
        return SIZE16X16;
    return SIZE25X25;
}

void FWBSettings::setIconsInRulesSize(FWBSettings::IconSize size)
{
    setValue(QString(iconsInRulesSize), QString(SIZE25X25 == size ? "SIZE25X25":"SIZE16X16"));
}

bool FWBSettings::getShowIconsInRules()
{
    return value(showIconsInRules).toBool();
}

void FWBSettings::setShowIconsInRules(bool showIcons)
{
    setValue(showIconsInRules, showIcons);
}

bool FWBSettings::getShowDirectionText()
{
    return value(showDirectionText).toBool();
}

void FWBSettings::setShowDirectionText(bool showText)
{
    setValue(showDirectionText, showText);
}

QFont FWBSettings::getRulesFont()
{
    return getFontByType(rulesFont);
}

void FWBSettings::setRulesFont(const QFont &font)
{
    setValue(rulesFont, font.toString());
}

QFont FWBSettings::getTreeFont()
{
    return getFontByType(treeFont);
}

void FWBSettings::setTreeFont(const QFont &font)
{
    setValue(treeFont, font.toString());
}

QFont FWBSettings::getUiFont()
{
    return getFontByType(uiFont);
}

void FWBSettings::setUiFont(const QFont &font)
{
    setValue(uiFont, font.toString());
}

QFont FWBSettings::getCompilerOutputFont()
{
    return getFontByType(compilerOutputFont);
}

void FWBSettings::setCompilerOutputFont(const QFont &font)
{
    setValue(compilerOutputFont, font.toString());
}

QFont FWBSettings::getFontByType(const char *type)
{
    QFont font = QFont();
    bool ok = font.fromString(value(type).toString());
    if (ok)
      return font;
    return QApplication::font();
}

bool FWBSettings::getClipComment()
{
    return value(clipComment).toBool();
}

void FWBSettings::setClipComment(bool clip)
{
    setValue(clipComment, clip);
}

bool FWBSettings::getCheckUpdates()
{
    return value(checkUpdates).toBool();
}

void FWBSettings::setCheckUpdates(bool f)
{
    setValue(checkUpdates, f);
}

uint FWBSettings::getTimeOfLastUpdateAvailableWarning()
{
    return value(updateAvailableWarningLastTime).toUInt();
}

void FWBSettings::setTimeOfLastUpdateAvailableWarning(uint v)
{
    setValue(updateAvailableWarningLastTime, v);
}

uint FWBSettings::getTimeOfLastAnnouncement(const QString &announcement)
{
    QByteArray h = QCryptographicHash::hash(announcement.toLatin1().constData(),
                                            QCryptographicHash::Md5).toHex();
    return value(QString(announcementLastTime).arg(h.constData())).toUInt();
}

void FWBSettings::setTimeOfLastAnnouncement(const QString &announcement, uint v)
{
    QByteArray h = QCryptographicHash::hash(announcement.toLatin1().constData(),
                                            QCryptographicHash::Md5).toHex();
    setValue(QString(announcementLastTime).arg(h.constData()), v);
}

QString FWBSettings::getCheckUpdatesProxy()
{
    return value(checkUpdatesProxy).toString();
}

void FWBSettings::setCheckUpdatesProxy(const QString &proxy_line)
{
    setValue(checkUpdatesProxy, proxy_line);
}

void FWBSettings::getExpandedObjectIds(const QString &filename,
                                       const QString &lib,
                                       std::set<int> &ids)
{
    ids.clear();
    QString ids_str = value(
        QString(SETTINGS_PATH_PREFIX "/") +
        "Window/" + filename + "/" + lib + "/ExpandedTreeItems").toString();
    QStringList strl = ids_str.split(",");
    for (QStringList::iterator i=strl.begin(); i!=strl.end(); ++i)
        ids.insert(FWObjectDatabase::getIntId(i->toStdString()));
}

void FWBSettings::setExpandedObjectIds(const QString &filename,
                                       const QString &lib,
                                       const std::set<int> &ids)
{
    QStringList strl;
    for (set<int>::const_iterator i=ids.begin(); i!=ids.end(); ++i)
        strl.push_back(FWObjectDatabase::getStringId(*i).c_str());
    setValue(
        QString(SETTINGS_PATH_PREFIX "/") +
        "Window/" + filename + "/" + lib + "/ExpandedTreeItems",
        strl.join(","));
}

int FWBSettings::getTreeSectionSize(const QString &filename,
                                    const QString &lib,
                                    int section_index)
{
    int v = value(
        QString(SETTINGS_PATH_PREFIX "/Window/%1/%2/TreeSection/%3")
        .arg(filename).arg(lib).arg(section_index)).toInt();
    if (v <= 0) v = 300;
    return v;
}

void FWBSettings::setTreeSectionSize(const QString &filename,
                                     const QString &lib,
                                     int section_index,
                                     int size)
{
    setValue(
        QString(SETTINGS_PATH_PREFIX "/Window/%1/%2/TreeSection/%3")
        .arg(filename).arg(lib).arg(section_index),
        size);
}

int FWBSettings::getVisibleRuleSetId(const QString &filename,
                                     const QString &lib)
{
    string str_id = value(
        QString(SETTINGS_PATH_PREFIX "/") +
        "Window/" + filename + "/" + lib + "/OpenedRuleSet").toString().toStdString();
    return FWObjectDatabase::getIntId(str_id);
}

void FWBSettings::setVisibleRuleSet(const QString &filename,
                                    const QString &lib,
                                    FWObject *ruleset)
{
    setValue(QString(SETTINGS_PATH_PREFIX "/") +
             "Window/" + filename + "/" + lib + "/OpenedRuleSet",
             FWObjectDatabase::getStringId(ruleset->getId()).c_str() );
}

void FWBSettings::getCollapsedRuleGroups(const QString &filename,
                                         const QString &firewall,
                                         const QString &ruleset,
                                         QStringList &collapsed_groups)
{
    QString key = "Window/" + filename + "/" + firewall + "/" +
        ruleset + "/CollapsedRuleGroups";
    QString strl = value(QString(SETTINGS_PATH_PREFIX "/") + key).toString();
    // QT regexp does not support negative lookbehind that we need to
    // find all "," if they are not preceded by a "\". Will split the
    // string on all commas, then find elements of the array that end
    // with "\" and combine them with the following ones.
    QStringList lst = strl.split(",");

    QStringListIterator i(lst);
    while (i.hasNext())
    {
        QString group_name = i.next();
        while (group_name.endsWith("\\") && i.hasNext())
            group_name += QString(",") + i.next();
        group_name.replace("\\,", ",");
        collapsed_groups.push_back(group_name);
    }

    if (fwbdebug)
        qDebug() << "FWBSettings::getCollapsedRuleGroups"
                 << key
                 << collapsed_groups.join(" ||| ");
}

void FWBSettings::setCollapsedRuleGroups(const QString &filename,
                                         const QString &firewall,
                                         const QString &ruleset,
                                         const QStringList &collapsed_groups)
{
    QStringList lst;
    foreach(QString group_name, collapsed_groups)
    {
        group_name.replace(",", "\\,");
        lst.push_back(group_name);
    }
    QString key = "Window/" + filename + "/" + firewall + "/" + ruleset +
        "/CollapsedRuleGroups";
    QString val = lst.join(",");
    if (fwbdebug)
        qDebug() << "FWBSettings::setCollapsedRuleGroups"
                 << key
                 << val;
    setValue(QString(SETTINGS_PATH_PREFIX "/") + key, val);
}

QStringList FWBSettings::getRecentFiles()
{
    return getList("recentFiles");
}

void FWBSettings::setRecentFiles(QStringList &list)
{
    return setList("recentFiles", list);
}

QString FWBSettings::getNewFirewallPlatform()
{
    return value(newFirewallPlatform).toString();
}

void FWBSettings::setNewFirewallPlatform(const QString &platform)
{
    setValue(newFirewallPlatform, platform);
}

QString FWBSettings::getNewClusterFailoverProtocol()
{
    return value(newClusterFailoverProtocol).toString();
}

void FWBSettings::setNewClusterFailoverProtocol(const QString &platform)
{
    setValue(newClusterFailoverProtocol, platform);
}

QString FWBSettings::getTargetStatus(const QString &platform,
                                     const QString &default_stat)
{
    QString var_path = targetStatus + platform;
    bool ok = contains(var_path);
    if (!ok) return default_stat;
    return value(var_path).toString();
}

void FWBSettings::setTargetStatus(const QString &platform, const QString &status)
{
    QString var_path = targetStatus + platform;
    setValue(var_path, status);
}

int FWBSettings::getABTestingGroup()
{
    return value(abTestingGroup).toInt();
}

void FWBSettings::setABTestingGroup(int n)
{
    setValue(abTestingGroup, n);
}

int FWBSettings::getStartsCounter()
{
    return value(startsCounter).toInt();
}

bool FWBSettings::isIntroDialogEnabled()
{
    return value(introDialogEnabled).toBool();
}

void FWBSettings::setIntroDialogEnabled(bool f)
{
    setValue(introDialogEnabled, f);
}

bool FWBSettings::customTemplatesEnabled()
{
    return value(customTemplatesEn).toBool();
}

void FWBSettings::setCustomTemplatesEnabled(bool f)
{
    setValue(customTemplatesEn, f);
}

bool FWBSettings::getDisplayUnmodifiedRules()
{
    return value(displayUnmodifiedRules).toBool();
}

void FWBSettings::setDisplayUnmodifiedRules(bool f)
{
    setValue(displayUnmodifiedRules, f);
}
