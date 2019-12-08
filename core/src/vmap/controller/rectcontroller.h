/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef RECTITEMCONTROLLER_H
#define RECTITEMCONTROLLER_H

#include <QColor>
#include <QRectF>
#include <QVariant>

#include "visualitemcontroller.h"

class VectorialMapController;
class RectController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect NOTIFY rectChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool filled READ filled NOTIFY filledChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth NOTIFY penWidthChanged)
public:
    enum Corner
    {
        TopLeft= 0,
        TopRight,
        BottomRight,
        BottomLeft,
    };
    RectController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    bool filled() const;
    QColor color() const;
    QRectF rect() const;
    quint16 penWidth() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;

signals:
    void colorChanged();
    void filledChanged();
    void rectChanged();
    void penWidthChanged();

public slots:
    void setColor(QColor color);
    void setCorner(const QPointF& move, int corner) override;

private:
    void setRect(QRectF rect);

private:
    QRectF m_rect= QRectF(0, 0, 1, 1);
    bool m_filled;
    QColor m_color;
    quint16 m_penWidth;
};

#endif // RECTITEMCONTROLLER_H