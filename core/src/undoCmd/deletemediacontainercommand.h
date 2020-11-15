/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
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
#ifndef DELETEMEDIACONTAINERCOMMAND_H
#define DELETEMEDIACONTAINERCOMMAND_H

#include "data/mediacontainer.h"
#include <QUndoCommand>

class SessionManager;
class Workspace;
class DeleteMediaContainerCommand : public QUndoCommand
{
public:
    DeleteMediaContainerCommand(MediaContainer* media, /*SessionManager* manager,*/ QMenu* menu, Workspace* workspace,
                                bool isGM, QHash<QString, MediaContainer*>& hash, QUndoCommand* parent= nullptr);

    ~DeleteMediaContainerCommand() override;

    void redo() override;
    void undo() override;

    bool sendAtOpening();

private:
    MediaContainer* m_media= nullptr;
    // SessionManager* m_manager= nullptr;
    QMenu* m_menu= nullptr;
    Workspace* m_mdiArea= nullptr;
    QHash<QString, MediaContainer*>& m_hash;
    bool m_gm;
};

#endif // DELETEMEDIACONTAINERCOMMAND_H
