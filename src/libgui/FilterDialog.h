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


#ifndef __FILTERDIALOG_H_
#define __FILTERDIALOG_H_

#include <ui_filterdialog_q.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Resources.h"
#include <qvector.h>
#include <QDialog>

class QRegExp;
class ObjectDescriptor;

enum {FWF_ANY = 0, FWF_ALL = 1}; 
enum {FWF_NAME = 0,FWF_ADDRESS = 1};
enum {FWF_CONTAINS = 0,
      FWF_IS_EQUAL_TO = 1,
      FWF_STARTS_WITH = 2,
      FWF_ENDS_WITH =3,
      FWF_MATCHES_WILDCARD = 4,
      FWF_MATCHES_REGEXP = 5};

class Filter
{
    private:
    
    bool CaseSensitive;
    bool MatchAny;
    QVector<QRegExp> addr_patterns;
    QVector<QRegExp> name_patterns; 


public:
    
    Filter();
    ~Filter();
    
    void addNamePattern(const QString &s,bool wc);
    void addNameRegExp(const QRegExp &r);
    void addAddrPattern(const QString &s,bool wc);
    void addAddrRegExp(const QRegExp &r);
    
    void setCaseSens(bool b);
    bool isCaseSens ();
    
    void setMatchAny(bool b);
    bool isMatchAny ();
    
    bool testName(const QString &s);
    bool testAddr(const QString &s);
    bool test(const ObjectDescriptor &od);
    bool isValid();
    
    void clear();

    int getNamePatternsNumber();
    int getAddrPatternsNumber();
    
    QString getNamePatternString(int p);
    QString getAddrPatternString(int p);
    bool isNameWildcard(int p);
    bool isAddrWildcard(int p);

    Filter& operator=(const Filter& f);
};

class FilterDialog : public QDialog
{
    Q_OBJECT
 private:
    Filter * flt;
    
    Ui::FilterDialog_q *m_dialog;
    
    bool validate();
    void update();
    QString LastFile;
    QRegExp constructRegExp(int p);
 public:
    FilterDialog(QWidget *parent);
    ~FilterDialog();
    void setFilter(Filter *);
    


public slots:
    virtual void apply();
    virtual void save();
    virtual void load();
    virtual void addPattern();
    virtual void removePattern();
    virtual void clearPatterns();
    virtual void updateData();

    
    
};

#endif 
