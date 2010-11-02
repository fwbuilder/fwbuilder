/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __OBJCONFLICTRESOLUTIONDIALOG_H_
#define __OBJCONFLICTRESOLUTIONDIALOG_H_

#include <ui_objconflictresolutiondialog_q.h>

#include "fwbuilder/FWObjectDatabase.h"

#include <sstream>

namespace libfwbuilder {
    class FWObject;
};

class ObjConflictResolutionDialog : public QDialog
{
    Q_OBJECT

    bool alwaysCurrent;
    bool alwaysNew;

protected:

    QString current_filename;
    QString new_filename;
    QString current_objname;
    QString new_objname;
    QString current_properties;
    QString new_properties;

    QString defaultLeftButtonText;
    QString defaultRightButtonText;

    bool    richText;

    void     saveGeometry();
    QString  makeBold(const QString &str);

public:
    Ui::ObjConflictResolutionDialog_q *m_dialog;
    
    ObjConflictResolutionDialog(QWidget *parent);
    virtual ~ObjConflictResolutionDialog();

    virtual int run( libfwbuilder::FWObject *o1,
                     libfwbuilder::FWObject *o2);

public slots:
    virtual void closeEvent(QCloseEvent *e);
    void setFlags();

protected slots:
    virtual void accept();
    virtual void reject();
        
};


class MergeConflictRes :
    public libfwbuilder::FWObjectDatabase::ConflictResolutionPredicate,
    ObjConflictResolutionDialog
{
    public:
    MergeConflictRes(QWidget *p) : ObjConflictResolutionDialog(p) {}
    virtual bool askUser(libfwbuilder::FWObject *o1,libfwbuilder::FWObject *o2)
    { 
        int res=run(o1,o2);
        return (res==QDialog::Accepted);
    }
};


class CompareObjectsDialog :
    public libfwbuilder::FWObjectDatabase::ConflictResolutionPredicate,
    ObjConflictResolutionDialog
{
    std::list<QString> report;
    QStringList        report_attributes;
    int                num_conflicts;
    int                column_width[4];
    
    void writeColumn(std::ostringstream &sstr,
                     int column_num,
                     const QString &txt);


public:
    CompareObjectsDialog(QWidget *p);
    
    virtual int run( libfwbuilder::FWObject *o1,
                     libfwbuilder::FWObject *o2);

    void clearReport();
    std::list<QString> getReport();
    int  getNumberOfConflicts() { return num_conflicts; }

    
    virtual bool askUser(libfwbuilder::FWObject *o1,libfwbuilder::FWObject *o2)
    {
        run(o1,o2);
        return QDialog::Accepted;
    }
};

#endif
