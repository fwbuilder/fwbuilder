/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#ifndef RULESETVIEWDELEGATE_H
#define RULESETVIEWDELEGATE_H

#include <QItemDelegate>
#include "RuleSetModel.h"

class RuleNode;
class FWObjectSelectionModel;

namespace libfwbuilder
{
    class RuleElement;
    class FWObject;
}

////////////////////////////////////////////////////////////////////////////
// RuleSetViewDelegate
////////////////////////////////////////////////////////////////////////////
class DrawingContext
{
    public:
    QRect objectRect;
    QRect drawRect;
    int itemHeight;
    QSize iconSize;
};


class RuleSetViewDelegate : public QItemDelegate
{
    Q_OBJECT;

    QColor standard_highlight;
    
public:

    RuleSetViewDelegate(QObject *parent, FWObjectSelectionModel *selectionModel);
    void paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint (const QStyleOptionViewItem &, const QModelIndex & ) const;

    void setStandardHighlightColor(const QColor &c) { standard_highlight = c; }
    
    static const int RULE_ITEM_GAP = 4;

    static const int VERTICAL_MARGIN = 2;
    static const int HORIZONTAL_MARGIN = 2;
    static const int ICON_TEXT_GAP = 2;

    static QSize getIconSize();
    static QSize getTextSize(QString, int flag);
    static int getItemHeight(QString s = "AiqyW", int flag = Qt::TextSingleLine, bool text = true);

private:
    enum PixmapAttr { Normal, Neg, Ref, Tree, NegTree };

    FWObjectSelectionModel *sectionModel;

    QString objectText(libfwbuilder::RuleElement *re,libfwbuilder::FWObject *obj) const;
    QPixmap getPixmap(QString name, PixmapAttr pmattr = Normal) const;

    DrawingContext initContext(QRect rect, bool useEnireSpace = false) const;

    QSize calculateCellSizeForRule(const QStyleOptionViewItem & option, const QModelIndex & index, RuleNode * node ) const;
    QSize calculateCellSizeForObject(const QModelIndex & index) const;
    QSize calculateCellSizeForComment(const QModelIndex & index) const;    
    QSize calculateCellSizeForIconAndText(const QModelIndex & index) const;
    QSize calculateCellSizeForOptions(const QModelIndex & index) const;


    QSize drawIconInRule(QPainter *p, int x, int y, QString name, bool neg) const;
    void drawIconAndText(QPainter *painter, QRect rect, QString icon, QString text, bool negation = false) const;
    void drawIcons(QPainter *painter, QRect rect, const QStringList &icons) const;
    void drawSelectedFocus(QPainter *painter, const QStyleOptionViewItem &option,QRect &rect) const;

    void paintGroup (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, RuleNode * node) const;
    void paintRule (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, RuleNode * node) const;
    void paintRowHeader (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, RuleNode * node) const;

    void paintObject(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;
    void paintDirection(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;
    void paintAction(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;
    void paintOptions(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;
    void paintComment(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;
    void paintMetric(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &v) const;

    QString constructActionText(ActionDesc &actionDesc)const;

    virtual QString getRuleColor(RuleNode * node ) const;
};

#endif // RULESETVIEWDELEGATE_H
