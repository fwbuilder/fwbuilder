/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectEditor.h,v 1.21 2006/09/07 15:42:12 vkurland Exp $

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


#ifndef  __OBJECTEDITOR_H_
#define  __OBJECTEDITOR_H_

#include "config.h"
#include "global.h"

#include "qdialog.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <qmap.h>
#include <qstring.h>

class ObjectTreeViewItem;
class QComboBox;
class QMenu;
class QStackedWidget;

class ObjectEditor : public QObject {

    Q_OBJECT

    QMap<QString, int>               stackIds;
    QMap<int, QWidget*>              dialogs;
    
    libfwbuilder::FWObject          *opened;
    int                              visible;
    QStackedWidget *parentWidget;
    QPushButton *closeButton;
    QPushButton *applyButton;

    void disconnectSignals();
    
public: 
    enum OptType{optAction,optComment,optMetric,optNone};
private: 
   OptType  openedOpt;
    

public:
   
    ObjectEditor( QWidget *parent );
    virtual ~ObjectEditor() {}
    

    QString getOptDialogName(OptType t);
    void open(libfwbuilder::FWObject *o);
    void openOpt(libfwbuilder::FWObject *, OptType t);
    void show();
    void hide();
    bool isVisible();
    bool isModified();

    libfwbuilder::FWObject* getOpened() { return opened; };
    OptType getOpenedOpt() {return openedOpt;};

    void purge();

    bool validateAndSave();
    void setCloseButton(QPushButton * b);
    void setApplyButton(QPushButton * b);
    void selectObject(libfwbuilder::FWObject *o);
    void selectionChanged(libfwbuilder::FWObject *o);
    void actionChanged(libfwbuilder::FWObject *o);

    
public slots:
    void validateAndClose(QCloseEvent *e);
    void apply();
    void discard();
    void close();
    void changed();
    void blank();
signals:

/**
 * the dialog class should have a slot that can load object's data
 * into dialog elements when ObjectEditor emits this signal
 */
    void loadObject_sign(libfwbuilder::FWObject *);

/**
 * the dialog class should have a slot that can verify data entered by
 * user in the dialog elements when ObjectEditor emits this
 * signal. The validation result is returned in variable "bool *res"
 */
    void validate_sign(bool *res);

/**
 * the dialog class should have a slot that can verify if the data in
 * dialog has been edited.
 */
    void isChanged_sign(bool *res);

/**
 * the dialog class should have a slot that applies changes made by
 * the user and saves data in the object.
 */
    void applyChanges_sign();

/**
 * the dialog class should have a slot that discards changes made by
 * the user 
 */
    void discardChanges_sign();

};

#endif
