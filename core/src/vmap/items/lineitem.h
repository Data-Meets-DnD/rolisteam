/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LINEITEM_H
#define LINEITEM_H

#include "visualitem.h"
#include <QPen>
class VisualItemController;
/**
 * @brief displays a line on maps.
 */
class LineItem : public VisualItem
{
public:
    LineItem(VisualItemController* ctrl);
    /**
     * @brief constructor with parameters
     */
    // LineItem(const QPointF& p, const QColor& penColor, int penSize, QGraphicsItem* parent= nullptr);
    /**
     * @brief paint the line
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    /**
     * @brief gives bounding rect of the line
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief shape
     * @return
     */
    virtual QPainterPath shape() const override;

    /**
     * @brief defines new position of the end line.
     */
    virtual void setNewEnd(const QPointF& nend) override;
    /**
     * @brief serialisation writing
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief serialisation reading
     */
    virtual void readData(QDataStream& in) override;

    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const override;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg) override;
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg) override;
    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF& pos) override;
    virtual void initChildPointItem() override;
    virtual VisualItem* getItemCopy() override;
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h) override;

    void setHoldSize(bool holdSize) override;

private:
    /**
     * @brief starting point, does not move except when the whole line is moved.
     */
    QPointF m_startPoint;
    /**
     * @brief ending point, should moved
     */
    QPointF m_endPoint;
};

#endif // LINEITEM_H
