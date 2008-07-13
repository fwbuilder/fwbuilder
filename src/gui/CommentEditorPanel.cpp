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



#include "../../config.h"
#include "global.h"

#include "CommentEditorPanel.h"
#include "FWBSettings.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <fstream>

using namespace std;
using namespace libfwbuilder;

CommentEditorPanel::~CommentEditorPanel()
{
    delete m_widget;
}

CommentEditorPanel::CommentEditorPanel(QWidget *p,
                                   bool enableLoadFromFile):QWidget(p)
{
    m_widget = new Ui::CommentEditorPanel_q;
    m_widget->setupUi(this);

    if (enableLoadFromFile) m_widget->inputFromFileButton->show();
    else                    m_widget->inputFromFileButton->hide();

    //m_widget->editor->setTextFormat(QTextEdit::PlainText);
    rule=NULL;
    //editor->setText(txt);
}

QString CommentEditorPanel::text()
{
    return m_widget->editor->toPlainText();
}
void CommentEditorPanel::setText(QString s)
{
    m_widget->editor->setText(s);
}
void CommentEditorPanel::setTitle(QString s)
{
    m_widget->editorTitle->setText(s);
}

void CommentEditorPanel::loadFromFile()
{
    if ( QMessageBox::warning(
       this,"Firewall Builder",
       tr("Warning: loading from file discards current contents of the script."),
       "&Load", "&Cancel", QString::null, 0, 1 )==0)
    {
        QString filename = QFileDialog::getOpenFileName(
                this, tr("Choose file that contains PIX commands"), st->getWDir());
        if (filename!="")
        {
           ifstream ifile(filename.toLatin1().constData());
           if (!ifile)
           {
               QMessageBox::warning(
                   this,"Firewall Builder",
                   tr("Could not open file %1").arg(filename),
                   "&Continue", QString::null, QString::null, 0, 1 );
               return;
           }

           char buf[1024];
           while (ifile.getline(buf,1024))
           {
               m_widget->editor->append( buf );
           }
        }
    }
}
void CommentEditorPanel::changed()
{
    emit changed_sign();
}
void CommentEditorPanel::applyChanges()
{
    mw->updateLastModifiedTimestampForAllFirewalls(rule);
    rule->setComment( string(m_widget->editor->toPlainText().toUtf8().constData()) );

}
void CommentEditorPanel::loadFWObject(FWObject *obj)
{
    Rule *r=Rule::cast(obj);

    rule=r;

    FWObject *o = r;
    while (o!=NULL && Firewall::cast(o)==NULL) o=o->getParent();
    assert(o!=NULL);
    Firewall *f=Firewall::cast(o);

    setText(QString::fromUtf8(r->getComment().c_str()));
    setTitle(QString("%1 / %2 / %3 ( Comment )")
            .arg(QString::fromUtf8(f->getName().c_str()))
            .arg(r->getTypeName().c_str())
            .arg(r->getPosition()));
}
void CommentEditorPanel::discardChanges()
{
}
void CommentEditorPanel::validate(bool* b)
{
    *b=true;
}
void CommentEditorPanel::isChanged(bool*)
{

}
void CommentEditorPanel::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}
