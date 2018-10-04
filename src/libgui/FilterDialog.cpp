/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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
#include "platforms.h"

#include "FilterDialog.h"
#include "ObjectManipulator.h"
#include "ObjectDescriptor.h"
#include "FWBSettings.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/XMLTools.h"


#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qstackedwidget.h>
#include <qcursor.h>
#include <qregexp.h>
#include <qtablewidget.h>
#include <qmessagebox.h>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

FilterDialog::FilterDialog(QWidget *parent) : QDialog(parent)
{
    m_dialog = new Ui::FilterDialog_q;
    m_dialog->setupUi(this);
}

FilterDialog::~FilterDialog()
{
    delete m_dialog;
}

void FilterDialog::setFilter(Filter * f)
{
    flt=f;

    /*
    QString p_n;
    QString p_a;
    int f_a,f_n,f_w,f_c;

    f_w=flt->isWildcard();
    f_c=flt->isCaseSens();
    f_a=flt->flt_addr;
    f_n=flt->flt_name;
    p_a=flt->getAddrPattern();
    p_n=flt->getNamePattern();

    init(f_w,f_c,f_a,f_n,p_a,p_n);
      */
    //table->setColumnStretchable(2,true);
}
void FilterDialog::apply()
{
    updateData();
    if (validate())
    {
        update();
        accept();
    }
    else
    {
        QMessageBox::critical(this,tr("Filter error"),tr("Invalid RegExp.") );
    }
}

void  FilterDialog::save()
{
    QString dir = LastFile;
    if (dir.isEmpty()) dir = st->getOpenFileDir();

    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Save file dialog",
                    dir,
                    "FWBuilder filter files (*.fwf)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    if (!s.endsWith(".fwf")) s += ".fwf";

    xmlDocPtr doc;

    xmlNodePtr node;
    //xmlNodePtr tree;

    doc = xmlNewDoc(XMLTools::ToXmlCast("1.0"));
    doc->children = xmlNewDocNode(doc, nullptr, XMLTools::ToXmlCast("FWB_FILTER"), nullptr);

    xmlSetProp(doc->children, XMLTools::ToXmlCast("version"),
               XMLTools::ToXmlCast( VERSION ));
    xmlSetProp(doc->children, XMLTools::ToXmlCast("CaseSensitive"),
               XMLTools::ToXmlCast( ((m_dialog->case_sensitive->isChecked())?"1":"0") ));
    xmlSetProp(doc->children, XMLTools::ToXmlCast("Match"),
               XMLTools::ToXmlCast( QString("%1").arg(m_dialog->combo->currentIndex()).toLatin1().constData() ));

    QString buf;
    int n=m_dialog->table->rowCount();
    for (int i=0;i<n;i++)
    {
        node = xmlNewChild(doc->children, nullptr,
                           XMLTools::ToXmlCast("FWB_FILTER_ITEM"), nullptr);

        buf=QString("%1").arg(((QComboBox*)m_dialog->table->cellWidget(i,0))->currentIndex());
        xmlSetProp(node,(const xmlChar*)  "Target",
                   XMLTools::ToXmlCast(buf.toLatin1().constData()) );

        buf=QString("%1").arg(((QComboBox*)m_dialog->table->cellWidget(i,1))->currentIndex());
        xmlSetProp(node, (const xmlChar*) "Type",
                   XMLTools::ToXmlCast(buf.toLatin1().constData()) );

        xmlSetProp(node, (const xmlChar*) "Pattern",
                   XMLTools::ToXmlCast(m_dialog->table->item(i,2)->text().toLatin1().constData()));
    }

    xmlSaveFile(s.toLatin1().constData(),doc);
    xmlFreeDoc(doc);
}

void  FilterDialog::load()
{
    QString s = QFileDialog::getOpenFileName(
                    this,
                    "Open file dialog",
                    st->getOpenFileDir(),
                    "FWBuilder filter files (*.fwf)");

    if (s.isEmpty()) return;
    st->setOpenFileDir(s);

    xmlDocPtr doc=xmlParseFile(s.toLatin1().constData());
    //TODO: use local codepage
    if (doc == nullptr)
    {
        qDebug("Document not parsed successfully.");
        return;
    }

    xmlNodePtr node= xmlDocGetRootElement(doc);

    if (node == nullptr)
    {
        qDebug("empty document");
        xmlFreeDoc(doc);
        return;
    }

    if (xmlStrcmp(node->name,(const xmlChar*) "FWB_FILTER"))
    {
        qDebug("document of the wrong type. (FWB_FILTER)");
        xmlFreeDoc(doc);
        return;
    }

    xmlChar *xmlbuf;
    QString qbuf;

    xmlbuf=xmlGetProp(node,(const xmlChar*) "CaseSensitive");
    qbuf=XMLTools::FromXmlCast(xmlbuf);
    XMLTools::FreeXmlBuff(xmlbuf);
    m_dialog->case_sensitive->setChecked(qbuf.toInt());

    xmlbuf=xmlGetProp(node,(const xmlChar*) "Match");
    qbuf=XMLTools::FromXmlCast(xmlbuf);
    XMLTools::FreeXmlBuff(xmlbuf);
    m_dialog->combo->setCurrentIndex(qbuf.toInt());


    node=node->xmlChildrenNode;
    while (node != nullptr)
    {
        if (xmlStrcmp(node->name,(const xmlChar*) "FWB_FILTER_ITEM"))
        {
            qDebug("document of the wrong type. (FWB_FILTER_ITEM)");
            xmlFreeDoc(doc);
            return;
        }

        addPattern();
        int n=m_dialog->table->rowCount()-1;


        xmlbuf=xmlGetProp(node,(const xmlChar*) "Target");
        qbuf=XMLTools::FromXmlCast(xmlbuf);
        XMLTools::FreeXmlBuff(xmlbuf);
        ((QComboBox*)m_dialog->table->cellWidget(n,0))->setCurrentIndex(
            qbuf.toInt());

        xmlbuf=xmlGetProp(node,(const xmlChar*) "Type");
        qbuf=XMLTools::FromXmlCast(xmlbuf);
        XMLTools::FreeXmlBuff(xmlbuf);
        ((QComboBox*)m_dialog->table->cellWidget(n,1))->setCurrentIndex(
            qbuf.toInt());


        xmlbuf=xmlGetProp(node,(const xmlChar*) "Pattern");
        qbuf=XMLTools::FromXmlCast(xmlbuf);
        XMLTools::FreeXmlBuff(xmlbuf);
        m_dialog->table->item(n,2)->setText(qbuf);

        node=node->next;
    }
    LastFile=s;
}

void FilterDialog::update()
{
    QRegExp r;
    Filter newflt;

    newflt.setMatchAny(m_dialog->combo->currentIndex());
    newflt.setCaseSens(m_dialog->case_sensitive->isChecked());
    newflt.clear();

    int n=m_dialog->table->rowCount();
    for(int i=0; i<n;i++)
    {
        r=constructRegExp(i);
        switch (((QComboBox*)m_dialog->table->cellWidget(i,0))->currentIndex())
        {
            case FWF_ADDRESS:
                {
                    newflt.addAddrRegExp(r);
                    break;
                }
            case FWF_NAME:
                {
                    newflt.addNameRegExp(r);
                    break;
                }
            default :
                {
                }
        }
    }

    if (newflt.isValid())
    {
        *flt=newflt;
    }
    /*
    bool res=false;

    Filter newflt;

    newflt.setAddrPattern( addresspattern->text());
    newflt.setNamePattern( namepattern->text());

    newflt.setWildcard( radioButton1->isChecked());
    newflt.setCaseSens( casesens->isChecked());

    newflt.flt_name = name_checkbox->isChecked();
    newflt.flt_addr = addr_checkbox->isChecked();

    if (newflt.isValid())
    {
        *flt=newflt;
        res=true;
    }

    return res;
    */
}
bool FilterDialog::validate()
{
    bool res=true;
    QRegExp r;

    int n=m_dialog->table->rowCount();
    for(int i=0; i<n;i++)
    {
        r=constructRegExp(i);
        if(!r.isValid())
        {
            res=false;
            m_dialog->table->selectRow(i);
            return res;
        }

    }

    return res;
}
QRegExp FilterDialog::constructRegExp(int p)
{
    QRegExp r;
    QString buf;
    r.setCaseSensitivity((m_dialog->case_sensitive->isChecked())?
            Qt::CaseSensitive:Qt::CaseInsensitive);
    switch(((QComboBox*)m_dialog->table->cellWidget(p,1))->currentIndex())
    {
        case FWF_CONTAINS:
            {
                r.setPatternSyntax(QRegExp::Wildcard);
                buf=m_dialog->table->item(p,2)->text().toLatin1().constData();
                break;
            }
        case FWF_IS_EQUAL_TO:
            {
                r.setPatternSyntax(QRegExp::RegExp);
                buf="^";
                buf+=m_dialog->table->item(p,2)->text().toLatin1().constData();
                buf+="$";
                break;
            }
        case FWF_STARTS_WITH:
            {
                r.setPatternSyntax(QRegExp::RegExp);
                buf="^";
                buf+=m_dialog->table->item(p,2)->text().toLatin1().constData();
                break;
            }
        case FWF_ENDS_WITH:
            {
                r.setPatternSyntax(QRegExp::RegExp);
                buf=m_dialog->table->item(p,2)->text().toLatin1().constData();
                buf+="$";
                break;
            }
        case FWF_MATCHES_WILDCARD:
            {
                r.setPatternSyntax(QRegExp::Wildcard);
                buf=m_dialog->table->item(p,2)->text().toLatin1().constData();
                break;
            }
        case FWF_MATCHES_REGEXP:
            {
                r.setPatternSyntax(QRegExp::RegExp);
                buf=m_dialog->table->item(p,2)->text().toLatin1().constData();
                break;
            }
        default :
            {
            }
    }
    r.setPattern(buf);
    return r;
}
void FilterDialog::addPattern()
{
    updateData();

    QStringList trg;
    trg+=tr("Name");
    trg+=tr("Address");

    QStringList tp;
    tp+=tr("Contains");
    tp+=tr("Is equal to");
    tp+=tr("Starts with");
    tp+=tr("Ends with");
    tp+=tr("Matches Wildcard");
    tp+=tr("Matches RegExp");

    int n=m_dialog->table->rowCount();
    m_dialog->table->setRowCount(n+1);

    QComboBox *cb = new QComboBox(m_dialog->table);
    cb->addItems(trg);
    m_dialog->table->setCellWidget(n,0,cb);

    cb = new QComboBox(m_dialog->table);
    cb->addItems(tp);
    m_dialog->table->setCellWidget(n,1,cb);

    QTableWidgetItem *itm = new QTableWidgetItem;
    itm->setFlags(itm->flags() | Qt::ItemIsEditable);
    m_dialog->table->setItem(n,2,itm);

    /*m_dialog->table->setItem(n,0,new QTableWidgetItem(m_dialog->table,trg));
    m_dialog->table->setItem(n,1,new QTableWidgetItem(m_dialog->table,tp));
    //table->setItem(n,2,new QTableWidgetItem(table,QTableWidgetItem::Always));
    m_dialog->table->setItem(n,2,new QTableWidgetItem(m_dialog->table,QTableWidgetItem::WhenCurrent,""));*/
}
void FilterDialog::removePattern()
{
    int r=m_dialog->table->currentRow();
    m_dialog->table->removeRow(r);
}
void FilterDialog::clearPatterns()
{
    m_dialog->table->setRowCount(0);
//    for (int i=table->rowCount()-1;i>0;i--)
//        table->removeRow(i);
}
void FilterDialog::updateData()
{
    int row=m_dialog->table->currentRow();
    QTableWidgetItem * item=m_dialog->table->item(row,2);
    QWidget * w =m_dialog->table->cellWidget (row,2 );
    if (w) item->setText ( ((QComboBox*)w)->currentText() );
}
//------------------------------------------------------------------
bool Filter::isCaseSens()
{
    return CaseSensitive;
}
void Filter::addNameRegExp(const QRegExp &r)
{
    name_patterns.push_back(r);
}
void Filter::addAddrRegExp(const QRegExp &r)
{
    addr_patterns.push_back(r);
}

QString  Filter::getNamePatternString(int p)
{
    return name_patterns[p].pattern();
}
QString  Filter::getAddrPatternString(int p)
{
    return addr_patterns[p].pattern();
}

int  Filter::getNamePatternsNumber()
{
    return name_patterns.size();
}
int Filter::getAddrPatternsNumber()
{
    return addr_patterns.size();
}

bool Filter::isNameWildcard(int p)
{
    return name_patterns[p].patternSyntax() == QRegExp::Wildcard;
}
bool Filter::isAddrWildcard(int p)
{
    return addr_patterns[p].patternSyntax() == QRegExp::Wildcard;
}

Filter & Filter::operator=(const Filter& f)
{
    addr_patterns=f.addr_patterns;
    name_patterns=f.name_patterns;
    CaseSensitive=f.CaseSensitive;
    MatchAny=f.MatchAny;

    return *this;
}
/*
void FilterDialog::closeEvent(QCloseEvent *e)
{
    if (fwbdebug)
        qDebug("FilterDialog::closeEvent  got close event: %p",e);
    hide();
}
*/

Filter::Filter()
{
    CaseSensitive=true;
    MatchAny=true;
}
Filter::~Filter()
{
}
void Filter::addNamePattern(const QString &s,bool wc)
{
    name_patterns.push_back(QRegExp(s,Qt::CaseSensitive,wc?QRegExp::Wildcard:QRegExp::RegExp));
}
void Filter::addAddrPattern(const QString &s,bool wc)
{
    addr_patterns.push_back(QRegExp(s,Qt::CaseSensitive,wc?QRegExp::Wildcard:QRegExp::RegExp));
}
void Filter::clear()
{
    name_patterns.clear();
    addr_patterns.clear();
}
void Filter::setCaseSens(bool b)
{
    CaseSensitive=b;
}
void Filter::setMatchAny(bool b)
{
    MatchAny=b;
}
bool Filter::isMatchAny ()
{
    return MatchAny;
}
bool Filter::testName(const QString &s)
{
    int cmp;
    if (name_patterns.isEmpty())
    {
        return addr_patterns.isEmpty() || !MatchAny;
    }
    for (int i=0;i<name_patterns.size();i++)
    {
        name_patterns[i].setCaseSensitivity(Qt::CaseSensitive);
        cmp=name_patterns[i].indexIn(s);
        if (MatchAny)
        {
            if(cmp>=0) return true;
        }
        else
        {
            if(cmp<0) return false;
        }
    }
    return !MatchAny;
}
bool Filter::testAddr(const QString &s)
{
    int cmp;
    if (addr_patterns.isEmpty())
    {
        return (name_patterns.isEmpty() || !MatchAny);
    }

    for (int i=0;i<addr_patterns.size();i++)
    {
        addr_patterns[i].setCaseSensitivity(Qt::CaseSensitive);
        cmp=addr_patterns[i].indexIn(s);
        if (MatchAny)
        {
            if(cmp>=0) return true;
        }
        else
        {
            if(cmp<0) return false;
        }
    }
    return !MatchAny;
}
bool Filter::test(const ObjectDescriptor &od)
{
    QString name=od.sysname.c_str();
    QString addr=od.addr.toString().c_str();
    return (MatchAny)? testAddr(addr) || testName(name):
                       testAddr(addr) && testName(name);
}

bool Filter::isValid()
{
    bool res=true;
    //TODO: Filter validity test
    return res;
}
