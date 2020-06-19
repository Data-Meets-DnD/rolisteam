/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
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
#include "deletemediacontainercommand.h"
#include "controller/contentcontroller.h"
#include "network/networkmessagewriter.h"
#include "widgets/workspace.h"

DeleteMediaContainerCommand::DeleteMediaContainerCommand(MediaContainer* media,
                                                         /*SessionManager* manager, */ QMenu* menu,
                                                         Workspace* workspace, bool isGM,
                                                         QHash<QString, MediaContainer*>& hash, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_media(media)
    // , m_manager(manager)
    , m_menu(menu)
    , m_mdiArea(workspace)
    , m_hash(hash)
    , m_gm(isGM)
{
    // TODO fix that - give controller
    // setText(QObject::tr("Close %1").arg(m_media->getUriName()));
}

DeleteMediaContainerCommand::~DeleteMediaContainerCommand()
{
    if(!m_media->isVisible())
    {
        delete m_media;
    }
}
void DeleteMediaContainerCommand::redo()
{
    qInfo() << QStringLiteral("redo command DeleteMediaContainerCommand: %1 ").arg(text());
    if(nullptr != m_media)
    {
        auto act= m_media->getAction();
        if(act)
        {
            act->setVisible(false);
        }
        m_mdiArea->removeMediaContainer(m_media);
        // m_manager->resourceClosed(m_media->getCleverUri());
        m_hash.remove(m_media->mediaId());
        if(m_gm)
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::CloseMedia);
            msg.string8(m_media->mediaId());
            msg.sendToServer();
        }
    }
}

void DeleteMediaContainerCommand::undo()
{
    qInfo() << QStringLiteral("undo command DeleteMediaContainerCommand: %1 ").arg(text());
    if(nullptr != m_media)
    {
        /* CleverURI* uri= m_media->getCleverUri();
         if(nullptr != uri)
         {
             // m_manager->addRessource(m_media->getCleverUri());
             uri->setDisplayed(true);
         }*/
        QAction* action= m_media->getAction();
        if(action == nullptr)
        {
            // action= m_menu->addAction(m_media->getUriName());
            action->setCheckable(true);
            action->setChecked(true);
            m_media->setAction(action);
        }
        action->setVisible(true);
        m_mdiArea->addContainerMedia(m_media);
        m_media->setVisible(true);
        m_media->setFocus();
        m_hash.insert(m_media->mediaId(), m_media);
        /*if(sendAtOpening())
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
            msg.uint8(static_cast<quint8>(uri->getType()));
            m_media->fill(msg);
            msg.sendToServer();
        }*/
    }
}
bool DeleteMediaContainerCommand::sendAtOpening()
{
    if(m_media->isRemote())
    {
        return false;
    }
    bool result= false;
    /*auto uri= m_media->getCleverUri();
    if(nullptr == uri)
    {
        return false;
    }
    switch(uri->getType())
    {
    case CleverURI::PICTURE:
    case CleverURI::VMAP:
    case CleverURI::MAP:
    case CleverURI::ONLINEPICTURE:
        result= true;
        break;
    default:
        result= false;
        break;
    }*/
    return result;
}
