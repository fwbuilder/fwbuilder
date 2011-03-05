/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef IC_PROGRESSPAGE_H
#define IC_PROGRESSPAGE_H

#include "ui_ic_progresspage_q.h"

#include <QTextCharFormat>

class ImporterThread;


class IC_ProgressPage : public QWizardPage
{
    Q_OBJECT;

    Ui::IC_ProgressPage_q *m_dialog;
    QTextCharFormat normal_format;
    QTextCharFormat error_format;
    QTextCharFormat warning_format;
    ImporterThread *importer;
    int errors_count;
    int warnings_count;
    
public:
    IC_ProgressPage(QWidget *parent);
    virtual ~IC_ProgressPage();

    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool validatePage();
    virtual bool isComplete() const;
    virtual int nextId () const;
        
    
public slots:
    void saveLog();
    void logLine(const QString &line);
    void importerDestroyed(QObject*);
    void importerFinished();

};

#endif // IC_PROGRESSPAGE_H
