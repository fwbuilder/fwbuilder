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


#include "global.h"
#include "utils.h"

#include "ColorLabelMenuItem.h"
#include "FWBSettings.h"

#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qpainter.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

#include <iostream>

using namespace std;


ColorLabelMenuItem::ColorLabelMenuItem(QWidget *parent) :
    QWidget(parent)
{
    m_widget = new Ui::colorLabelMenuItem_q;
    m_widget->setupUi(this);

    setup( m_widget->noneBtn,   "#FFFFFF", tr("no color") );
    setup( m_widget->redBtn,
           st->getLabelColor(FWBSettings::RED   ),
           st->getLabelText(FWBSettings::RED   ));
    setup( m_widget->orangeBtn,
           st->getLabelColor(FWBSettings::ORANGE),
           st->getLabelText(FWBSettings::ORANGE));
    setup( m_widget->yellowBtn,
           st->getLabelColor(FWBSettings::YELLOW),
           st->getLabelText(FWBSettings::YELLOW));
    setup( m_widget->greenBtn,
           st->getLabelColor(FWBSettings::GREEN ),
           st->getLabelText(FWBSettings::GREEN ));
    setup( m_widget->blueBtn,
           st->getLabelColor(FWBSettings::BLUE  ),
           st->getLabelText(FWBSettings::BLUE  ));
    setup( m_widget->purpleBtn,
           st->getLabelColor(FWBSettings::PURPLE),
           st->getLabelText(FWBSettings::PURPLE));
    setup( m_widget->grayBtn,
           st->getLabelColor(FWBSettings::GRAY  ),
           st->getLabelText(FWBSettings::GRAY  ));
}

void ColorLabelMenuItem::setup(QToolButton *btn,
                               const QString &c, const QString &t)
{
    QPixmap pm(8,8);
    pm.fill( QColor(c) );
    QPainter p( &pm );
    p.drawRect( pm.rect() );
    btn->setIcon(QIcon(pm));
    btn->setToolTip(t);
}

void ColorLabelMenuItem::colorClicked()
{
    if (isVisible() &&
        parentWidget() &&
        parentWidget()->inherits("QPopupMenu") ) parentWidget()->close();

    emit returnColor(color);   // signal
}


void ColorLabelMenuItem::noneColorClicked()
{
    color="";
    colorClicked();
}

void ColorLabelMenuItem::redColorClicked()
{
    color=st->getLabelColor(FWBSettings::RED);
    colorClicked();
}


void ColorLabelMenuItem::orangeColorClicked()
{
    color=st->getLabelColor(FWBSettings::ORANGE);
    colorClicked();
}


void ColorLabelMenuItem::yellowColorClicked()
{
    color=st->getLabelColor(FWBSettings::YELLOW);
    colorClicked();
}


void ColorLabelMenuItem::greenColorClicked()
{
    color=st->getLabelColor(FWBSettings::GREEN);
    colorClicked();
}


void ColorLabelMenuItem::blueColorClicked()
{
    color=st->getLabelColor(FWBSettings::BLUE);
    colorClicked();
}


void ColorLabelMenuItem::purpleColorClicked()
{
    color=st->getLabelColor(FWBSettings::PURPLE);
    colorClicked();
}


void ColorLabelMenuItem::grayColorClicked()
{
    color=st->getLabelColor(FWBSettings::GRAY);
    colorClicked();
}


