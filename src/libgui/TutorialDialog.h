/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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

#ifndef TUTORIALDIALOG_H
#define TUTORIALDIALOG_H

#include <QDialog>
#include <QTextDocument>

namespace Ui {
    class TutorialDialog_q;
}

class TutorialDialog : public QDialog {
    Q_OBJECT;
public:
    TutorialDialog(QString tutorial, QWidget *parent = 0);

    ~TutorialDialog();
    QString tutorial;
    QString css_stylesheet;
    QTextDocument *doc;

    QString getScenarioForPage(int page);
    QString getUndoForPage(int page);
    QString getResetForPage(int page);

    void runScenario(QString scenario);

    static void showTutorial(QString tutorial);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TutorialDialog_q *ui;
    int currentPage;
    void initializeTutorial(QString tutorial);
    static TutorialDialog *dialog;

public slots:
    void previous();
    void next();
    void reset();
    void showPage(int page);
};

#endif // TUTORIALDIALOG_H
