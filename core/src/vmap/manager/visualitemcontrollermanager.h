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
#ifndef VISUALITEMCONTROLLERMANAGER_H
#define VISUALITEMCONTROLLERMANAGER_H

#include <QObject>

namespace vmap
{
class VisualItemController;
}
class NetworkMessageReader;
class VisualItemControllerManager : public QObject
{
    Q_OBJECT
public:
    explicit VisualItemControllerManager(QObject* parent= nullptr);

    virtual QString addItem(const std::map<QString, QVariant>& params)= 0;
    virtual void addController(vmap::VisualItemController* controller)= 0;
    virtual void removeItem(const QString& id)= 0;
    virtual void processMessage(NetworkMessageReader* msg)= 0;
    virtual bool loadItem(const QString& id)= 0;

signals:
    void itemAdded(const QString& id);
};

#endif // VISUALITEMCONTROLLERMANAGER_H
