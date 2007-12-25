/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: inplaceComboBox.h,v 1.2 2004/01/13 07:21:18 vkurland Exp $

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


#ifndef  __INPLACECOMBOBOX_H_
#define  __INPLACECOMBOBOX_H_

#include "qframe.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QPixmap;

class inplaceComboBox : public QFrame
{

    QComboBox *comboBox;

 protected:
    QGridLayout* qLayout;
    QVBoxLayout* layout1;

 public:

    inplaceComboBox(QWidget *p=0, const char* name = 0, Qt::WindowFlags f = 0);

    void insertItem( const QPixmap &pm, const QString &txt, int index=-1);
    
    
    int  currentIndex();
    void setCurrentIndex( int index );
    
};

#endif
