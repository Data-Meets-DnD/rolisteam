/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef SHAREDNOTEMEDIACONTROLLER_H
#define SHAREDNOTEMEDIACONTROLLER_H

#include <memory>
#include <vector>

#include "mediacontrollerinterface.h"

class SharedNoteController;
class SharedNoteMediaController : public MediaControllerInterface
{
    Q_OBJECT
public:
    SharedNoteMediaController();
    ~SharedNoteMediaController() override;

    CleverURI::ContentType type() const override;
    bool openMedia(CleverURI* uri, const std::map<QString, QVariant>& args) override;
    void closeMedia(const QString& id) override;
    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
    void setUndoStack(QUndoStack* stack) override;

signals:
    void sharedNoteControllerCreated(SharedNoteController* crtl);

private:
    std::vector<std::unique_ptr<SharedNoteController>> m_sharedNotes;
};

#endif // SHAREDNOTEMEDIACONTROLLER_H
