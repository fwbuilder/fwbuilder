/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Resources.h"

#include "ObjConflictResolutionDialog.h"
#include "FWObjectPropertiesFactory.h"
#include "FWBSettings.h"


#include <QCloseEvent>
#include <qdialog.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qtextbrowser.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qmap.h>
#include <QtDebug>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace libfwbuilder;


ObjConflictResolutionDialog::ObjConflictResolutionDialog(QWidget *parent): QDialog(parent)
{
    m_dialog = new Ui::ObjConflictResolutionDialog_q;
    m_dialog->setupUi(this);
    setObjectName("ObjConflictResolutionDialog");

    alwaysCurrent=false;
    alwaysNew    =false;
    m_dialog->dlgIcon->setPixmap( QMessageBox::standardIcon( QMessageBox::Warning ) );

    defaultLeftButtonText = tr("Keep current object");
    defaultRightButtonText = tr("Replace with this object");

    if (st->haveGeometry(this)) st->restoreGeometry(this);

    richText = true;
}

ObjConflictResolutionDialog::~ObjConflictResolutionDialog()
{
    saveGeometry();
}

QString ObjConflictResolutionDialog::makeBold(const QString &str)
{
    QString bold   = (richText)?QString("<b>"):"";
    QString unbold = (richText)?QString("</b>"):"";
    return QString("%1%2%3").arg(bold).arg(str).arg(unbold);
}

int ObjConflictResolutionDialog::run(FWObject *o1, FWObject *o2)
{
    // some simple cases where we don't have to ask the user and can make
    // decision automatically.

    // CustomService object, if one of the objects adds code string
    // for a platform which was absent in another
    if (CustomService::isA(o1) && CustomService::isA(o2))
    {
        bool o1_adds_code_string = false;
        bool o2_adds_code_string = false;
        bool code_changes = false;
        map<string,string> platforms = Resources::getPlatforms();
        for (map<string,string>::iterator i=platforms.begin(); i!=platforms.end(); i++)
        {
            string c1 = CustomService::cast(o1)->getCodeForPlatform( (*i).first );
            string c2 = CustomService::cast(o2)->getCodeForPlatform( (*i).first );
            if (c1 != c2 && !c1.empty() && !c2.empty()) code_changes = true;
            if (c1 != c2 && c1.empty()) o2_adds_code_string = true;
            if (c1 != c2 && c2.empty()) o1_adds_code_string = true;
        }

        if (fwbdebug)
        {
            qDebug() << "Comparing to CustomService objects:";
            qDebug() << "o1=" << o1->getName().c_str()
                     << "o2=" << o2->getName().c_str();
            qDebug() << "code_changes=" << code_changes
                     << "o1_adds_code_string=" << o1_adds_code_string
                     << "o2_adds_code_string=" << o2_adds_code_string;
        }

        if (!code_changes && o1_adds_code_string) return QDialog::Rejected;
    }

    // fill in dialogs even though the user might have
    // checked checkbox that makes  decision without
    // them having to click a button. This is so that
    // classes that inherit from ObjConflictResolutionDialog
    // can use data collected in this method. Particularly
    // CompareObjectsDialog::run needs it


    QString leftBtnTxt, rightBtnTxt;
    bool    leftCB,  rightCB, leftBtn, rightBtn;

    leftBtnTxt  = defaultLeftButtonText;
    rightBtnTxt = defaultRightButtonText;
    leftCB=rightCB=leftBtn=rightBtn=true;

    QString p1, p2;

    FWObject *delObjLib1 = o1->getRoot()->getById(
        FWObjectDatabase::DELETED_OBJECTS_ID );
    FWObject *delObjLib2 = o2->getRoot()->getById(
        FWObjectDatabase::DELETED_OBJECTS_ID );

    if (delObjLib1!=nullptr && o1->isChildOf(delObjLib1))
    {
        /* This is the case when an object present in the file we are
         * trying to load has been deleted in the tree. We can not
         * just ignore deleted object in the tree and load a copy from
         * the file because it will create a conflict (two objects
         * with the same ID). I am not sur eI can delete object from
         * here either. It is unclear how to solve this problem
         * correctly. Defer to the user. */

        p1=tr("Object '%1' has been deleted").arg(makeBold(o1->getName().c_str()));
        rightBtnTxt = tr("Delete");
        leftCB  = rightCB = leftBtn = false;
    } else
        p1= FWObjectPropertiesFactory::getObjectPropertiesDetailed(o1,
                                                                   true,
                                                                   false,
                                                                   false);

    if (delObjLib2!=nullptr && o2->isChildOf(delObjLib2))
    {
        /* This is the case where object o2 has been deleted in the
         * file we are trying to load but is present in the tree.  One
         * situation when this occurs is when we preloaded bunch of
         * libraries and this object is in one of them but has been
         * deleted in the file. We should ignore deleted objects in
         * the file and use copy present in the tree.
         */

        cerr << "Deleted object found: o2 "
             << o2->getId() << " " << o2->getName() << endl;

        return QDialog::Rejected;

        p2=tr("Object '%1' has been deleted").arg(makeBold(o2->getName().c_str()));
        leftBtnTxt = tr("Delete");
        leftCB  = rightCB = rightBtn = false;
    } else
        p2= FWObjectPropertiesFactory::getObjectPropertiesDetailed(o2,
                                                                   true,
                                                                   false,
                                                                   false);


    m_dialog->useCurrentObj->setText(leftBtnTxt);
    m_dialog->useNewObj->setText(rightBtnTxt);

    m_dialog->useNewObj->setEnabled(rightBtn);
    m_dialog->newAll->setEnabled(rightCB);
    m_dialog->useCurrentObj->setEnabled(leftBtn);
    m_dialog->currentAll->setEnabled(leftCB);

    if (leftBtn) m_dialog->useCurrentObj->setFocus();
    else         m_dialog->useNewObj->setFocus();

    QString f1= FWObjectDatabase::cast(o1->getRoot())->getFileName().c_str();
    QString f2= FWObjectDatabase::cast(o2->getRoot())->getFileName().c_str();

    current_filename = f1;
    new_filename = f2;

    current_objname = o1->getName().c_str();
    new_objname = o2->getName().c_str();

    current_properties = p1;
    new_properties = p2;


    if (f1.isEmpty())
        f1=tr("Object '%1' in the objects tree").arg(makeBold(o1->getName().c_str()));
    else
        f1=tr("Object '%1' in file %2").arg(makeBold(o1->getName().c_str())).arg(f1);

    f2=tr("Object '%1' in file %2").arg(makeBold(o2->getName().c_str())).arg(f2);


    m_dialog->currentObjLbl->setText(f1);
    m_dialog->newObjLbl->setText(f2);

    m_dialog->currentObj->clear();
    m_dialog->newObj->clear();

    QString s;
    s="<a name=\"top\">\n";
    s+=p1;
    s+="<hr height=\"0\">";
    s+=o1->getComment().c_str();

    m_dialog->currentObj->moveCursor(QTextCursor::Start);
    m_dialog->currentObj->append( s );
    m_dialog->currentObj->scrollToAnchor("top");

    s="<a name=\"top\">\n";
    s+=p2;
    s+="<hr height=\"0\">";
    s+=o2->getComment().c_str();

    m_dialog->newObj->moveCursor( QTextCursor::Start );
    m_dialog->newObj->append( s );
    m_dialog->newObj->scrollToAnchor("top");


    if (alwaysCurrent) return QDialog::Rejected;
    if (alwaysNew)     return QDialog::Accepted;

    return QDialog::exec();
}

void ObjConflictResolutionDialog::saveGeometry()
{
    st->saveGeometry(this);
}

/*
 * Important
 *
 * normally close event is sent when user clicks "close window" button
 * on the window titlebar. When this event is processed in this method,
 * the window is still visible so it is safe to retrieve and use its
 * geometry (it is bad to get geometry of the window when it is hidden
 * because at that time window manager decorations do not exist
 * anymore, so window's position on the screen is shiften up and to
 * the left).
 *
 * It seems under certain window manager (at this time it is unknown
 * which one) in Gnome "close event" is generated after the window is
 * closed by clicking one of the buttons at the bottom (choosing which
 * objects to keep). We call saveGeometry from accept and reject to
 * get size and position when user clicks those buttons. Window is
 * then closed and (it seems) window manager sends "close" event to
 * it. By the time when we get control in this method, the window is
 * already closed and geometry returned for it would be incorrect.
 *
 * Finally, I decided to make it so the user can not close conflict
 * resolution dialog using titlebar button. The user is suppposed to
 * make a decision, and although closing dialog was equivalent to
 * clicking one of the choice buttons, it wasn't obvious. So it is
 * better to disable this completely and make it obvious for the user
 * that they must make a choice.
 */
void ObjConflictResolutionDialog::closeEvent(QCloseEvent *e)
{
    if (fwbdebug) qDebug("ObjConflictResolutionDialog::closeEvent");

    e->ignore();
}

void ObjConflictResolutionDialog::setFlags()
{
    alwaysCurrent = m_dialog->currentAll->isChecked();
    alwaysNew     = m_dialog->newAll->isChecked();
}

void ObjConflictResolutionDialog::accept()
{
  if (fwbdebug)
    qDebug("ObjConflictResolutionDialog::accept(): isVisible=%d",
	   isVisible());

    QDialog::accept();
}

void ObjConflictResolutionDialog::reject()
{
  if (fwbdebug)
    qDebug("ObjConflictResolutionDialog::reject(): isVisible=%d",
	   isVisible());

    QDialog::reject();
}

// ################################################################

CompareObjectsDialog::CompareObjectsDialog(QWidget *p) :
    ObjConflictResolutionDialog(p)
{
    richText = false;
    num_conflicts = 0;
    column_width[0] = 30;
    column_width[1] = 30;
    column_width[2] = 30;
    column_width[3] = 30;

    m_dialog->currentAll->hide();
    m_dialog->useCurrentObj->hide();

    defaultLeftButtonText = "";
    defaultRightButtonText = tr("Next");

    m_dialog->dialogHeading->setText( tr("The following two objects have the same internal ID but different attributes:") );
    m_dialog->newAll->setText( tr("Skip the rest but build report") );

    clearReport();

    // Note : these keys match strings generated by
    // FWObjectPropertyFactory::getObjectPropertiesDetailed
    // That is, getObjectPropertiesDetailed generates text like this:
    //
    //  Library: TestLibrary
    //  Object Id: id3F3D04676
    //  Object Type: Firewall
    //  Object Name: guardian
    //
    // Keys in report_attributes must match strings before ':' exactly

    report_attributes.push_back("Name");
    report_attributes.push_back("Library");
    report_attributes.push_back("Object Id");
    report_attributes.push_back("Object Type");
    report_attributes.push_back("Object Name");
    report_attributes.push_back("Path");

}

void CompareObjectsDialog::writeColumn(ostringstream &sstr,
                                       int column_num,
                                       const QString &txt)
{
    int col_width = column_width[column_num];

    sstr << txt.toLatin1().constData() << setw(col_width-txt.length()) << setfill(' ') << ' ';
}

int CompareObjectsDialog::run(FWObject *o1,FWObject *o2)
{
    ostringstream str;

    int res = ObjConflictResolutionDialog::run(o1,o2);

/*
    currentObj->setTextFormat(Qt::PlainText);
    QString l_text = currentObj->text(0);
    // QTextEdit returns whole paragraph as one line
    // Since we enforce PlainText, all html formatting
    // is lost and individual lines are glued together
    // with some character that prints as '?'
    // Could be chr(0) ?

    if (fwbdebug) qDebug("%s",l_text.ascii());

    newObj->setTextFormat(Qt::PlainText);
    QString r_text = newObj->text(0);

    if (fwbdebug) qDebug("%s",r_text.ascii());

    str << l_text << endl;
    str << r_text << endl;
    str << endl;
*/

    num_conflicts++;

    QString prop1 = FWObjectPropertiesFactory::stripHTML(current_properties);
    QString prop2 = FWObjectPropertiesFactory::stripHTML(new_properties);


    QStringList proplist1 = prop1.split("\n");
    QStringList proplist2 = prop2.split("\n");

    QMap<QString,QString> propdict1;
    QMap<QString,QString> propdict2;

    int n = 0;
    QStringList::Iterator i1 = proplist1.begin();
    for ( ; i1!=proplist1.end(); ++i1,++n)
    {
        ostringstream tstr;
        tstr << "key_" << n;

        QString k = (*i1).section(':',0,0).trimmed();
        QString v = (*i1).section(':',1).trimmed();
        if (v=="")
        {
            v = k;
            k = tstr.str().c_str();
        }

        if (fwbdebug) qDebug() << QString("proplist1:    k='%1'  v='%2'")
                          .arg(k).arg(v);

        propdict1[k] = v;
    }

    n = 0;
    QStringList::Iterator i2 = proplist2.begin();
    for ( ; i2!=proplist2.end(); ++i2,++n)
    {
        ostringstream tstr;
        tstr << "key_" << n;

        QString k = (*i2).section(':',0,0).trimmed();
        QString v = (*i2).section(':',1).trimmed();
        if (v=="")
        {
            v = k;
            k = tstr.str().c_str();
        }

        if (fwbdebug) qDebug() << QString("proplist2:    k='%1'  v='%2'")
                          .arg(k).arg(v);

        propdict2[k] = v;
    }


    QStringList::Iterator i3 = report_attributes.begin();
    for ( ; i3!=report_attributes.end(); ++i3)
    {
        QString attr = *i3;

        if (fwbdebug) qDebug() << "report_attributes:    attr=" << attr;

        if (!propdict1.contains(attr) || !propdict2.contains(attr)) continue;

        writeColumn(str, 1, attr);
        writeColumn(str, 2, propdict1[attr]);
        writeColumn(str, 3, propdict2[attr]);
        str << endl;

        propdict1[attr] = "";
        propdict2[attr] = "";
    }

    QMap<QString,QString>::Iterator mi1 = propdict1.begin();
    for ( ; mi1!=propdict1.end(); ++mi1)
    {
        QString key = mi1.key();
        QString val = mi1.value();

        if (fwbdebug) qDebug() << QString("propdict1:    key=%1  val=%2")
                          .arg(key).arg(val);

        if (val=="") continue;

        if (key.startsWith("key_"))  writeColumn(str, 1, " ");
        else                         writeColumn(str, 1, key);
        if (propdict1.contains(key)) writeColumn(str, 2, propdict1[key]);
        else                         writeColumn(str, 2, " ");
        if (propdict2.contains(key)) writeColumn(str, 3, propdict2[key]);
        else                         writeColumn(str, 3, " ");
        str << endl;

        propdict1[key] = "";
        propdict2[key] = "";
    }

    QMap<QString,QString>::Iterator mi2 = propdict2.begin();
    for ( ; mi2!=propdict2.end(); ++mi2)
    {
        QString key = mi2.key();
        QString val = mi2.value();

        if (fwbdebug) qDebug() << QString("propdict2:    key=%1  val=%2")
                          .arg(key).arg(val);

        if (val=="") continue;

        if (key.startsWith("key_"))  writeColumn(str, 1, " ");
        else                         writeColumn(str, 1, key);
        if (propdict1.contains(key)) writeColumn(str, 2, propdict1[key]);
        else                         writeColumn(str, 2, " ");
        if (propdict2.contains(key)) writeColumn(str, 3, propdict2[key]);
        else                         writeColumn(str, 3, " ");
        str << endl;
    }

    str << setw(78) << setfill('-') << '-' << endl;

    report.push_back( QString(str.str().c_str()) );

    return res;
}

void CompareObjectsDialog::clearReport()
{
    report.clear();
}

list<QString> CompareObjectsDialog::getReport()
{
    ostringstream str;

    str << "File 1:  " << current_filename.toLatin1().constData() << endl;
    str << "File 2:  " << new_filename.toLatin1().constData() << endl;
    str << setw(78) << setfill('-') << '-' << endl;

    report.push_front( QString(str.str().c_str()) );

    return report;
}

