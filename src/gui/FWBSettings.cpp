/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: FWBSettings.cpp,v 1.47 2006/06/26 03:21:00 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "../../config.h"
#include "global.h"

#include "FWBSettings.h"
#include "FWWindow.h"
#include "ObjectManipulator.h"

#include "fwbuilder/FWObjectDatabase.h"

#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <QDir>
#include <QDesktopWidget>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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

/*
 * Note:
 *
 * We need to keep installation data and program settings in registry
 * folders with different names. QSettings always looks into Current
 * User registry first, so if the folders have the same names, then we
 * store evaluation key in Current User, while it is better to put it
 * in the Local Machine branch.
 *
 * So, installation data goes to HKLM Software\NetCitadel\FirewallBuilder
 * and settings to HKCU Software\NetCitadel\FirewallBuilder2
 *
 * fwbuilder-lm determines folder path for the license file by
 * reading key Install_Dir under HKLM Software\NetCitadel\FirewallBuilder
 */

const char* DTDSetpath           = SETTINGS_PATH_PREFIX "/System/DTDPath";
const char* ResSetpath           = SETTINGS_PATH_PREFIX "/System/ResPath";
const char* wdirSetpath          = SETTINGS_PATH_PREFIX "/Environment/WDir";
const char* ofdirSetpath         = SETTINGS_PATH_PREFIX "/Environment/OpenFileDir";
const char* sfdirSetpath         = SETTINGS_PATH_PREFIX "/Environment/SaveFileDir";
const char* startupActionSetpath = SETTINGS_PATH_PREFIX "/Environment/StartupAction";
const char* labelColorPath       = SETTINGS_PATH_PREFIX "/ColorLabels/color_";
const char* labelTextPath        = SETTINGS_PATH_PREFIX "/ColorLabels/text_";
const char* lastEditedSetpath    = SETTINGS_PATH_PREFIX "/Environment/LastEdited";
const char* autoSave             = SETTINGS_PATH_PREFIX "/Environment/autoSave";
const char* expandTreeSetpath    = SETTINGS_PATH_PREFIX "/UI/ExpandTree";
const char* MergeLibsSetpath     = SETTINGS_PATH_PREFIX "/UI/MergeLibraries";
const char* infoStyleSetpath     = SETTINGS_PATH_PREFIX "/UI/InfoWindowStyle";
const char* infoWindowHSetpath   = SETTINGS_PATH_PREFIX "/UI/InfoWindowHeight";
const char* groupModeSetpath     = SETTINGS_PATH_PREFIX "/UI/GroupViewMode";
const char* groupColsSetpath     = SETTINGS_PATH_PREFIX "/UI/GroupViewColumns";
const char* objTooltips          = SETTINGS_PATH_PREFIX "/UI/objTooltips";
const char* tooltipDelay         = SETTINGS_PATH_PREFIX "/UI/tooltipDelay";
const char* emptyRCSLog          = SETTINGS_PATH_PREFIX "/RCS/emptyLog";
const char* dontSaveStdLib       = SETTINGS_PATH_PREFIX "/DataFormat/dontSaveStdLib";
const char* WindowGeometrySetpath= SETTINGS_PATH_PREFIX "/Layout/";
const char* screenPositionSetpath= SETTINGS_PATH_PREFIX "/ScreenPos/";

const char* SSHPath              = SETTINGS_PATH_PREFIX "/SSH/SSHPath";
const char* showIconsInRules     = SETTINGS_PATH_PREFIX "/UI/Icons/ShowIconsInRules";
const char* showDirectionText    = SETTINGS_PATH_PREFIX "/UI/Icons/ShowDirectionTextInRules";
const char* iconsInRulesSize     = SETTINGS_PATH_PREFIX "/UI/Icons/IconsInRulesSize";
const char* rulesFont            = SETTINGS_PATH_PREFIX "/UI/Fonts/RulesFont";
const char* treeFont             = SETTINGS_PATH_PREFIX "/UI/Fonts/TreeFont";
const char* uiFont               = SETTINGS_PATH_PREFIX "/UI/Fonts/UiFont";

const char* clipComment       = SETTINGS_PATH_PREFIX "/UI/ClipComment";

FWBSettings::FWBSettings() : QSettings(QSettings::UserScope, "netcitadel.com", "Firewall Builder")
{
//    writeEntry( DTDSetpath, librespath.c_str() );
//    writeEntry( ResSetpath, respath.c_str()   );
}

/**
 *  to preserve behavior of the old versions of fwbuilder on Unix, the
 *  default working dir is set to "." - current dir.
 *
 *  On Windows default working dir is set to
 *             "Documents and settings/USERNAME/Firewalls"
 */
void FWBSettings::init()
{
    bool ok=false;
/*
    QString defwd =
#ifdef _WIN32
        QString(getenv("HOMEPATH"))+"/Firewalls";
#else
        "";
#endif
    QString wd = readEntry(wdirSetpath,defwd, &ok);
    if (!ok)  writeEntry(wdirSetpath, defwd );
*/

    ok = contains(infoStyleSetpath);
    if (!ok) setValue(infoStyleSetpath,2);

    ok = contains(infoWindowHSetpath);
    if (!ok) setValue(infoWindowHSetpath,200);

    ok = contains(dontSaveStdLib);
    if (!ok) setDontSaveStdLib(true);

    ok = contains(startupActionSetpath);
    if (!ok) setStartupAction(2);

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
            QString err= QString(QObject::tr("Working directory %1 does not exist and could not be created.\nIgnoring this setting.")).arg(wd.toLatin1().constData());

            if (app != NULL)
            {
                QMessageBox::critical( 0,"Firewall Builder", err,
                                       "&Continue", 0, 0, 0 );
            } else
            {
                qDebug( err.toAscii().constData() );
            }
            setWDir("");
        }
    }

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

    ok = contains(showIconsInRules);
    if (!ok) setShowIconsInRules(true);

    ok = contains(iconsInRulesSize);
    if (!ok) setIconsInRulesSize(SIZE25X25);

    ok = contains(rulesFont);
    if (!ok) setRulesFont(QFont("times", 11, QFont::Normal));

    ok = contains(treeFont);
    if (!ok) setTreeFont(QFont("times", 11, QFont::Normal));

    ok = contains(uiFont);
    if (!ok) setUiFont(QFont("times", 11, QFont::Normal));

    ok = contains(clipComment);
    if (!ok) setClipComment(true);

#ifndef _WIN32
    if (getSSHPath().isEmpty())  setSSHPath("ssh");
#endif
}

QString FWBSettings::getStr(const QString &attribute)
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    return value(path).toString();
}

void    FWBSettings::setStr(const QString &attribute,
                            const QString &val)
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    setValue(path,val);
}

bool    FWBSettings::getBool(const QString &attribute)
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    return value(path).toBool();
}

void    FWBSettings::setBool(const QString &attribute, bool val )
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    setValue(path,val);
}


int     FWBSettings::getInt(const QString &attribute)
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    return value(path).toInt();
}

void    FWBSettings::setInt(const QString &attribute, int val )
{
    QString path=SETTINGS_PATH_PREFIX "/"+attribute;
    setValue(path,val);
}



QString FWBSettings::getWDir() { return value(wdirSetpath).toString();}
void    FWBSettings::setWDir( const QString &wd ) { setValue(wdirSetpath,wd);}
int     FWBSettings::getInfoStyle() { return value(infoStyleSetpath).toInt();}
void    FWBSettings::setInfoStyle(int s) { setValue(infoStyleSetpath,s);}
int     FWBSettings::getInfoWindowHeight() { return value(infoWindowHSetpath).toInt();}
void    FWBSettings::setInfoWindowHeight(int h) { setValue(infoWindowHSetpath,h);}

QString FWBSettings::getGroupViewMode() { return value(groupModeSetpath).toString();}
void    FWBSettings::setGroupViewMode(const QString &m) { setValue(groupModeSetpath,m);}

QString FWBSettings::getGroupViewColumns() { return value(groupColsSetpath).toString();}
void    FWBSettings::setGroupViewColumns(const QString &m) { setValue(groupColsSetpath,m);}


int     FWBSettings::getStartupAction() { return value(startupActionSetpath).toInt();}
void    FWBSettings::setStartupAction(int sa) { setValue( startupActionSetpath , sa );}

int     FWBSettings::getExpandTree() {  return value(expandTreeSetpath).toInt(); }
void    FWBSettings::setExpandTree(int f) { setValue( expandTreeSetpath , f ); }

int     FWBSettings::getMergeLibs() {  return value(MergeLibsSetpath).toInt(); }
void    FWBSettings::setMergeLibs(int f) { setValue( MergeLibsSetpath , f ); }

bool    FWBSettings::getObjTooltips() {  return value( objTooltips ).toBool();}
void    FWBSettings::setObjTooltips(bool f) {  setValue( objTooltips, f); }

int     FWBSettings::getTooltipDelay() { return value( tooltipDelay ).toInt(); }
void    FWBSettings::setTooltipDelay(int v) { setValue( tooltipDelay, v); }


QString FWBSettings::getLastEdited() { return value(lastEditedSetpath).toString();}
void    FWBSettings::setLastEdited(const QString &file) { setValue(lastEditedSetpath,file);}

QString FWBSettings::getOpenFileDir()
{
    return value(ofdirSetpath).toString();
}

void    FWBSettings::setOpenFileDir( const QString &d )
{
     setValue(ofdirSetpath,d);
}

QString FWBSettings::getSaveFileDir()
{
    return value(sfdirSetpath).toString();
}

void    FWBSettings::setSaveFileDir( const QString &d )
{
     setValue(sfdirSetpath,d);
}


void    FWBSettings::save()
{
    if (mw->db()!=NULL)
        setLastEdited( mw->db()->getFileName().c_str() );
}

bool    FWBSettings::getRCSLogState() { return value( emptyRCSLog ).toBool(); }
void    FWBSettings::setRCSLogState(bool f) { setValue( emptyRCSLog , f ); }

bool    FWBSettings::getAutoSave() { return value( autoSave ).toBool(); }
void    FWBSettings::setAutoSave(bool f) { setValue( autoSave, f); }

bool    FWBSettings::getDontSaveStdLib() {return value(dontSaveStdLib).toBool();}
void    FWBSettings::setDontSaveStdLib( bool f) { setValue(dontSaveStdLib,f);}

bool    FWBSettings::haveScreenPosition(const QString &wname)
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

QPoint  FWBSettings::getScreenPosition(const QString &wname)
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

void    FWBSettings::saveScreenPosition(const QString &wname, const QPoint &p)
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

bool    FWBSettings::haveGeometry(QWidget *w)
{
    QString name=w->objectName();
    QString val = value(QString(WindowGeometrySetpath)+name,"").toString();
    return (!val.isEmpty());
}

void    FWBSettings::restoreGeometry(QWidget *w)
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

    if (x+width > sg.width())   x=sg.width()-width;
    if (y+height > sg.height()) y=sg.height()-height;
    if (x<0)                    x=(sg.width()-width)/2;
    if (y<0)                    y=(sg.height()-height)/2;

    if (fwbdebug)
    {
	qDebug("FWBSettings::restoreGeometry  widget '%s' vis=%d x=%d y=%d",
               name.toAscii().constData(), w->isVisible(), x,y);
    }

    w->resize( QSize(width,height) );
}

void    FWBSettings::restoreGeometry(QWidget *w, const QRect &dg)
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

    if (x+width > sg.width())   x=sg.width()-width;
    if (y+height > sg.height()) y=sg.height()-height;
    if (x<0)                    x=(sg.width()-width)/2;
    if (y<0)                    y=(sg.height()-height)/2;

    if (fwbdebug)
    {
	qDebug("FWBSettings::restoreGeometry  widget '%s' vis=%d x=%d y=%d",
               name.toAscii().constData(), w->isVisible(), x,y);
    }

    w->resize( QSize(width,height) );
}

void    FWBSettings::saveGeometry(QWidget *w)
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
               name.toAscii().constData(), w->isVisible(), val.toAscii().constData());
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

QString FWBSettings::getLabelColor(enum LabelColors c)
{
    return value(QString(labelColorPath) + getLabelColorStr(c)).toString();
}

void    FWBSettings::setLabelColor(enum LabelColors c,const QString &s)
{
    setValue(QString(labelColorPath) + getLabelColorStr(c), s);
}

QString FWBSettings::getLabelText(enum LabelColors c)
{
    return value(QString(labelTextPath) + getLabelColorStr(c)).toString();
}

void    FWBSettings::setLabelText(enum LabelColors c, const QString &s)
{
    setValue(QString(labelTextPath) + getLabelColorStr(c),s);
}

QString FWBSettings::getSSHPath()
{
    return value(SSHPath).toString();
}

void FWBSettings::setSSHPath(const QString &path)
{
    setValue(SSHPath,path);
}

void    FWBSettings::getPrinterOptions(QPrinter *printer,int &pageWidth,int &pageHeight)
{
    printer->setPrinterName(getStr("PrintSetup/printerName"));
    printer->setPrinterSelectionOption(getStr("PrintSetup/printerSelectionOption"));
    printer->setOutputFileName(getStr("PrintSetup/outputFileName"));
    printer->setOrientation(QPrinter::Orientation(getInt("PrintSetup/orientation")));
    printer->setPageSize(QPrinter::PageSize(getInt("PrintSetup/pageSize")));
    printer->setPageOrder(QPrinter::PageOrder(getInt("PrintSetup/pageOrder")));
//    int res = getInt("PrintSetup/resolution");
//    if (res>0) printer->setResolution(res);
    printer->setColorMode(QPrinter::ColorMode(getInt("PrintSetup/colorMode")));
    printer->setFullPage(getBool("PrintSetup/fullPage"));
//    printer->setFromTo(getInt("PrintSetup/fromPage"),getInt("PrintSetup/toPage"));
//    printer->setNumCopies(getInt("PrintSetup/numCopies"));

    pageWidth = getInt("PrintSetup/pageWidth");
    pageHeight = getInt("PrintSetup/pageHeight");
}

void    FWBSettings::setPrinterOptions(QPrinter *printer,int pageWidth,int pageHeight)
{
    setStr("PrintSetup/printerName",printer->printerName());
    setStr("PrintSetup/printerSelectionOption",printer->printerSelectionOption());
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

QFont FWBSettings::getFontByType(const char *type)
{
    QFont font = QFont();
    bool ok = font.fromString(value(type).toString());
    if (ok)
      return font;
    return QFont("times", 11, QFont::Normal);
}

bool FWBSettings::getClipComment()
{
    return value(clipComment).toBool();
}

void FWBSettings::setClipComment(bool clip)
{
    setValue(clipComment, clip);
}
