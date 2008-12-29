/****************************************************************************
*****************************************************************************/

#include "global.h"
#include <ui_aboutdialog_q.h>

#include "fwbuilder/Constants.h"

#include "../../VERSION.h"
#include "../../build_num"

class FWBAboutDialog: public QDialog {
    Q_OBJECT
            
public:
    Ui::AboutDialog_q *m_aboutDialog;
    
    FWBAboutDialog() 
    {
        m_aboutDialog = new Ui::AboutDialog_q;
        
        m_aboutDialog->setupUi(this);
        
        //it was the "init" method of AboutDialog_q:
        m_aboutDialog->titleLbl->setText(
            QString("Firewall Builder v%1").arg(VERSION) );
        m_aboutDialog->revLbl->setText(
            tr("Revision: %1 ( Build: %2 )").arg(RELEASE_NUM).arg(BUILD_NUM) );
    
        m_aboutDialog->apiLbl->setText(
            tr("Using Firewall Builder API %1").arg(
                libfwbuilder::Constants::getLibraryVersion().c_str() ) );

#ifdef ELC
        if (registered) m_aboutDialog->reg->setText(tr("Registered"));
        else            m_aboutDialog->reg->setText(tr("Unregistered"));
#endif
        
        setWindowTitle(QString("Firewall Builder: About..."));
        adjustSize();
    };
    ~FWBAboutDialog() { delete m_aboutDialog; };
};
