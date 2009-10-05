/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "CompilerOutputPanel.h"
#include "FWBSettings.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"
#include "ProjectPanel.h"

#include "CompilerDriverFactory.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwcompiler/BaseCompiler.h"

#include <QTextFormat>
#include <QStatusBar>

#include <fstream>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerOutputPanel::CompilerOutputPanel(QWidget *parent) :
    BaseObjectDialog(parent)
{
    m_widget = new Ui::CompilerOutputPanel_q;
    m_widget->setupUi(this);
}

CompilerOutputPanel::~CompilerOutputPanel()
{
    delete m_widget;
}

void CompilerOutputPanel::changed()
{
    emit changed_sign();
}

void CompilerOutputPanel::applyChanges()
{
    emit notify_changes_applied_sign();
}

void CompilerOutputPanel::discardChanges()
{
}

void CompilerOutputPanel::getHelpName(QString *str)
{
    *str = "CompilerOutputPanel";
}

void CompilerOutputPanel::loadFWObject(FWObject *obj)
{
    if (fwbdebug)
        qDebug("CompilerOutputPanel::loadFWObject  obj id=%s",
               FWObjectDatabase::getStringId(obj->getId()).c_str());

    list<string> err_re;
    BaseCompiler::errorRegExp(&err_re);
    foreach(string re, err_re)
    {
        error_re.push_back(QRegExp(re.c_str()));
    }

    list<string> warn_re;
    BaseCompiler::warningRegExp(&warn_re);
    foreach(string re, warn_re)
    {
        warning_re.push_back(QRegExp(re.c_str()));
    }

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
    QStatusBar *sb = mw->statusBar();
    sb->showMessage( tr("Compiling rule...") );
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    FWObject *p = obj;
    // use Firewall::cast to match both Firewall and Cluster
    while (!Firewall::cast(p)) p = p->getParent();
    Firewall *fw = Firewall::cast(p);
    Rule *rule = Rule::cast(obj);

    CompilerDriver *dr = CompilerDriverFactory::createCompilerDriver(fw);
    // run in test mode to prevent fatal errors from causing exit
    dr->setTestMode();
    dr->setEmbeddedMode();

    QTextCharFormat format;
    QTextCharFormat normal_format;
    QTextCharFormat bold_format;
    QTextCharFormat error_format;
    QTextCharFormat warning_format;

    QTextCursor cursor(m_widget->compiler_output_panel->textCursor());
    format = cursor.charFormat();
    format.setFont(st->getCompilerOutputFont());

    normal_format = format;
    normal_format.setForeground(QBrush(Qt::black));

    bold_format = format;
    bold_format.setProperty(QTextFormat::FontWeight, 99);
    bold_format.setForeground(QBrush(Qt::black));

    error_format = format;
    error_format.setForeground(QBrush(Qt::red));
    error_format.setProperty(QTextFormat::FontWeight, 99);

    warning_format = format;
    warning_format.setForeground(QBrush(Qt::blue));
    warning_format.setProperty(QTextFormat::FontWeight, 99);

    m_widget->compiler_output_panel->clear();

    try
    {
        QMapIterator<QString,QString> it(
            dr->compileSingleRule(FWObjectDatabase::getStringId(rule->getId())));

        QTextCursor cursor = m_widget->compiler_output_panel->textCursor();
        while (it.hasNext())
        {
            it.next();
            QString dbg;
            if (fwbdebug)
                dbg = QString("(id: %1)").arg(
                    FWObjectDatabase::getStringId(rule->getId()).c_str());
            QString title("%1 / %2 / rule %3 %4\n");
            cursor.insertText(title
                              .arg(it.key())
                              .arg(rule->getParent()->getName().c_str())
                              .arg(rule->getPosition())
                              .arg(dbg), bold_format);

            foreach (QString line, it.value().trimmed().split("\n"))
            {
                format = normal_format;

                list<QRegExp>::const_iterator it;
                for (it=error_re.begin(); it!=error_re.end(); ++it)
                {
                    if ((*it).indexIn(line) != -1)
                    {
                        format = error_format;
                        break;
                    }
                }

                for (it=warning_re.begin(); it!=warning_re.end(); ++it)
                {
                    if ((*it).indexIn(line) != -1)
                    {
                        format = warning_format;
                        break;
                    }
                }

                cursor.insertText(line + "\n", format);
            }
            cursor.insertText("\n");
            cursor.insertBlock();
        }
    } catch (FWException &e)
    {
        m_widget->compiler_output_panel->append(e.toString().c_str());
        m_widget->compiler_output_panel->append("\n");
    }
    QApplication::restoreOverrideCursor();
    sb->clearMessage();

    delete dr;
}

void CompilerOutputPanel::validate(bool* b )
{
    *b=true;
}

void CompilerOutputPanel::isChanged(bool *)
{
}

void CompilerOutputPanel::closeEvent(QCloseEvent *)
{
}

