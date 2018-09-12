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

#ifndef __FWBSETTINGS_H_
#define __FWBSETTINGS_H_

#include <qsettings.h>
#include <qrect.h>
#include <qprinter.h>
#include <qfont.h>

#include <set>

#define SETTINGS_PATH_PREFIX  "/" GENERATION

#include <fwbuilder/FWObject.h>


class QWidget;

/*
 * startup actions. I know, enum would be better, but QComboBox
 * operates with integers and it is much simpler to just store item
 * numbers in preferences.
 */
#define LoadStandardObjects 0
#define LoadLastEditedFile  1

class FWBSettings : public QSettings
{
 public:

    enum LabelColors { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, GRAY, ADD_COLOR, EDIT_COLOR, MOVE_COLOR, REMOVE_COLOR };
    enum IconSize{ SIZE25X25, SIZE16X16};

 private:
    QSettings *uuid_settings;
    QSettings *ssh_timeout_setings_object;
    bool first_run;
    
    QString getLabelColorStr(enum LabelColors c);
    QString getDiffColorStr(enum LabelColors c);

    
 public:

    FWBSettings(bool testData = false);
    ~FWBSettings();

    static QString getApplicationNameForSettings()
    {
        return "FirewallBuilder" GENERATION;
    }

    void init(bool force_first_time_run=false);
    void save();

    bool isFirstRun() { return first_run; }
    
    QString getWDir();
    void    setWDir(const QString &wd);

    QString getDataDir();
    void    setDataDir(const QString &dataDir);

    QString getOpenFileDir( const QString &existingPath = "");
    void    setOpenFileDir( const QString &d );

    int     getInfoStyle();
    void    setInfoStyle(int s);

    QString getGroupViewMode();
    void    setGroupViewMode(const QString &mode);

    QString getGroupViewColumns();
    void    setGroupViewColumns(const QString &mode);

    int     getStartupAction();
    void    setStartupAction(int sa);

    int     getExpandTree();
    void    setExpandTree(int sa);

    int     getMergeLibs();
    void    setMergeLibs(int sa);

    bool    getObjTooltips();
    void    setObjTooltips(bool f);

    int     getTooltipDelay();
    void    setTooltipDelay(int v);

    QString getLastEdited();
    void    setLastEdited(const QString &file);

    int     getInfoWindowHeight();
    void    setInfoWindowHeight(int h);

    bool    getRCSLogState();
    void    setRCSLogState(bool f);

    int     getRCSFilePreviewStyle();
    void    setRCSFilePreviewStyle(int style);

    int     getRCSFilePreviewSortColumn();
    void    setRCSFilePreviewSortColumn(int col);

    bool    getAutoSave();
    void    setAutoSave(bool f);

    bool    getCompression();
    void    setCompression(bool f);

    bool    getDontSaveStdLib();
    void    setDontSaveStdLib( bool f);

    bool hasKey(const QString &attribute);
    
    QString getStr(const QString &attribute);
    void    setStr(const QString &attribute, const QString &val);

    bool    getBool(const QString &attribute);
    void    setBool(const QString &attribute, bool f );

    int     getInt(const QString &attribute);
    void    setInt(const QString &attribute, int v );

    QStringList getList(const QString &attribute);
    void    setList(const QString &attribute, QStringList &list);

    bool    haveGeometry(QWidget *w);
    void    restoreGeometry(QWidget *w);
    void    restoreGeometry(QWidget *w, const QRect &defaultGeometry);
    void    saveGeometry(QWidget *w);

    bool    haveScreenPosition(const QString &wname);
    QPoint  getScreenPosition(const QString &wname);
    void    saveScreenPosition(const QString &wname, const QPoint &p);

    QString getLabelColor(enum LabelColors c);
    void    setLabelColor(enum LabelColors c,const QString &s);
    QString getLabelText(enum LabelColors c);
    void    setLabelText(enum LabelColors c, const QString &s);
    QString getDiffColor(enum LabelColors c);
    void    setDiffColor(enum LabelColors c,const QString &s);

    QString getSSHPath();
    void    setSSHPath(const QString &path);

    QString getSCPPath();
    void    setSCPPath(const QString &path);

    QString getDiffPath();
    void    setDiffPath(const QString &path);

    bool    haveSSHTimeout();
    int     getSSHTimeout();
    void    setSSHTimeout(int value_sec);

    void    getPrinterOptions(QPrinter *printer,int &pageWidth,int &pageHeight);
    void    setPrinterOptions(QPrinter *printer,int pageWidth,int pageHeight);

    QString getAppGUID();

    bool isReminderAboutStandardLibSuppressed();
    void suppressReminderAboutStandardLib(bool f);
    
    bool isReminderAboutDataDirSuppressed();
    void suppressReminderAboutDataDir(bool f);
    
    enum IconSize getIconsInRulesSize();
    void setIconsInRulesSize(enum IconSize size);

    bool getShowIconsInRules();
    void setShowIconsInRules(bool showIcons);

    bool getShowDirectionText();
    void setShowDirectionText(bool showText);

    QFont getRulesFont();
    void setRulesFont(const QFont &font);

    QFont getTreeFont();
    void setTreeFont(const QFont &font);

    QFont getUiFont();
    void setUiFont(const QFont &font);

    QFont getCompilerOutputFont();
    void setCompilerOutputFont(const QFont &font);

    bool getClipComment();
    void setClipComment(bool);

    bool getCheckUpdates();
    void setCheckUpdates(bool);

    uint getTimeOfLastUpdateAvailableWarning();
    void setTimeOfLastUpdateAvailableWarning(uint v);
    
    uint getTimeOfLastAnnouncement(const QString &announcement);
    void setTimeOfLastAnnouncement(const QString &announcement, uint v);
    
    QString getTargetStatus(const QString &platform, const QString &default_stat);
    void setTargetStatus(const QString &plaform, const QString &status);

    QString getCheckUpdatesProxy();
    void setCheckUpdatesProxy(const QString &proxy_line);

    void getExpandedObjectIds(const QString &filename,
                              const QString &lib,
                              std::set<int> &ids);
    void setExpandedObjectIds(const QString &filename,
                              const QString &lib,
                              const std::set<int> &ids);

    int getTreeSectionSize(const QString &filename,
                           const QString &lib,
                           int section_index);
    void setTreeSectionSize(const QString &filename,
                            const QString &lib,
                            int section_index,
                            int size);

    int getVisibleRuleSetId(const QString &filename,
                            const QString &lib);
    void setVisibleRuleSet(const QString &filename,
                           const QString &lib,
                           libfwbuilder::FWObject *ruleset);

    void getCollapsedRuleGroups(const QString &filename,
                                const QString &firewall,
                                const QString &ruleset,
                                QStringList &collapsed_groups);
    void setCollapsedRuleGroups(const QString &filename,
                                const QString &firewall,
                                const QString &ruleset,
                                const QStringList &collapsed_groups);

    QStringList getRecentFiles();
    void setRecentFiles(QStringList &list);

    QString getNewFirewallPlatform();
    void setNewFirewallPlatform(const QString &platform);

    QString getNewClusterFailoverProtocol();
    void setNewClusterFailoverProtocol(const QString &protocol);

    bool getShowUndoPanel();
    void setShowUndoPanel(bool);

    bool getIconsWithText();
    void setIconsWithText(bool f);
    
    int getABTestingGroup();
    void setABTestingGroup(int n);

    int getStartsCounter();

    bool isIntroDialogEnabled();
    void setIntroDialogEnabled(bool f);

    bool customTemplatesEnabled();
    void setCustomTemplatesEnabled(bool f);

    bool getDisplayUnmodifiedRules();
    void setDisplayUnmodifiedRules(bool);
    
private:
    QFont getFontByType(const char*type);
};

#endif

