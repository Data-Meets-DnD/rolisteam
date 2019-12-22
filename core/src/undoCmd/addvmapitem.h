/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#ifndef ADDVMAPITEMCOMMAND_H
#define ADDVMAPITEMCOMMAND_H

#include "vmap/vmap.h"
#include "vmap/vtoolbar.h"
#include <QUndoCommand>

class VectorialMapController;
class VisualItemControllerManager;
class AddVmapItemCommand : public QUndoCommand
{
public:
    AddVmapItemCommand(VectorialMapController* ctrl, VisualItemControllerManager* visualCtrl,
                       const std::map<QString, QVariant>& args, QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

    /*    template <class T>
        T* getItem() const;
        VisualItem* getItem() const;
        VisualItem* getPath() const;

        QString getLocalUserId() const;
        void setLocalUserId(const QString& localUserId);

        bool hasError() const;
        void setError(bool error);

        bool isNpc() const;

        bool isUndoable() const;
        void setUndoable(bool undoable);

        bool getInitPoint() const;
        void setInitPoint(bool initPoint);
        bool hasToBeDeleted() const;

    protected:
        bool isVisible();
        void initItem(bool addMapLayer);*/

private:
    QPointer<VectorialMapController> m_ctrl;
    QPointer<VisualItemControllerManager> m_visualCtrl;
    std::map<QString, QVariant> m_params;
    QString m_uuid;
};

#endif // ADDFIELDCOMMAND_H
