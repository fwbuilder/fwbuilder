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

#include "fwbuilder_ph.h"

#include "inplaceComboBox.h"

#include <qvariant.h>
#include <qpixmapcache.h>
#include "qcombobox.h"
#include "qlayout.h"

inplaceComboBox::inplaceComboBox( QWidget* parent, const char* name, Qt::WindowFlags fl )
    : QFrame( parent, fl )
{

    if ( !name )
        setObjectName( "inplaceComboBox" );
    else
        setObjectName(name);

//    setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    setFocusPolicy( Qt::StrongFocus );
    qLayout = new QGridLayout( this );
    qLayout->setObjectName("qLayout");
    qLayout->setMargin( 0 );
    qLayout->setSpacing( 0 );

    layout1 = new QVBoxLayout( 0 );
    layout1->setMargin( 0 );
    layout1->setSpacing( 0 );
    layout1->setObjectName( "layout1" );

    comboBox = new QComboBox( this );
    comboBox->setEditable( FALSE );
    comboBox->setObjectName( "comboBox" );

    QSizePolicy p = QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ); //5, 5
    p.setHorizontalStretch(0);
    p.setVerticalStretch(0);
    p.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());

    comboBox->setSizePolicy( p );

    comboBox->setFocusPolicy( Qt::WheelFocus );

    layout1->addWidget( comboBox );
    QSpacerItem* spacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout1->addItem( spacer );

    qLayout->addLayout( layout1, 0, 0 );
    //setWindowState( WState_Polished );
    ensurePolished();
}

void inplaceComboBox::insertItem( const QPixmap &pm, const QString &txt, int index)
{
    if (index > -1)
        comboBox->insertItem(index, QIcon(pm), txt);
    else
        comboBox->addItem(QIcon(pm), txt);

    comboBox->setFixedHeight(pm.height()+4);
}

int  inplaceComboBox::currentIndex() { return comboBox->currentIndex(); }

void inplaceComboBox::setCurrentIndex( int index ) { comboBox->setCurrentIndex(index); }
