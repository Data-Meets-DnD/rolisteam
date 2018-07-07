/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   Copyright (C) 2015 by Renaud Guezennec                              *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <QtGlobal>
#include <QStringList>
//
class NetworkLink;
class ClientManager;
struct NetworkMessageHeader
{
    quint8  category;
    quint8  action;
    quint32 dataSize;
};
namespace NetMsg
{
enum Category {
    AdministrationCategory,
    PlayerCategory,
    CharacterPlayerCategory,
    NPCCategory,
    CharacterCategory,
    DrawCategory,
    MapCategory,
    ChatCategory,
    MusicCategory,
    SetupCategory,
    SharePreferencesCategory,
    VMapCategory,
    MediaCategory,
    SharedNoteCategory
};

enum Action {
    // AdministrationCategory
    EndConnectionAction = 0,
    Heartbeat,
    ConnectionInfo,
    Goodbye,
    Kicked,
    MoveChannel,
    SetChannelList,
    NeedPassword,
    AuthentificationSucessed,
    AuthentificationFail,
    LockChannel,
    JoinChannel,
    DeleteChannel,
    AddChannel,
    ChannelPassword,
    BanUser,
    ClearTable,
    AdminPassword,
    AdminAuthSucessed,
    AdminAuthFail,
    MovedIntoChannel,
    GMStatus,


    // PlayerCategory
    PlayerConnectionAction = 0,
    DelPlayerAction,
    ChangePlayerNameAction,
    ChangePlayerColorAction,
    ChangePlayerAvatarAction,

    // CharacterPlayerCategory
    AddPlayerCharacterAction = 0,
    DelPlayerCharacterAction,
    ToggleViewPlayerCharacterAction,
    ChangePlayerCharacterSizeAction,
    ChangePlayerCharacterNameAction,
    ChangePlayerCharacterColorAction,
    ChangePlayerCharacterAvatarAction,

    //NPCCategory
    addNpc =0,
    delNpc,

    //CharacterCategory
    addCharacterList=0,
    moveCharacter,
    changeCharacterState,
    changeCharacterOrientation,
    showCharecterOrientation,
    addCharacterSheet,
    updateFieldCharacterSheet,
    closeCharacterSheet,


    // MapCategory
    AddEmptyMap = 0,
    LoadMap,
    ImportMap,
    CloseMap,

    // Painting
    penPainting =0,
    linePainting,
    emptyRectanglePainting,
    filledRectanglePainting,
    emptyEllipsePainting,
    filledEllipsePainting,
    textPainting,
    handPainting,




    // ChatCategory
    ChatMessageAction = 0,
    DiceMessageAction,
    EmoteMessageAction,
    UpdateChatAction,
    DelChatAction,

    // MusicCategory
    StopSong =0,
    PlaySong,
    PauseSong,
    NewSong,
    ChangePositionSong,

    // SetupCategory
    AddFeatureAction = 2,

    //SharePreferencesCategory
    addDiceAlias = 0,
    moveDiceAlias,
    removeDiceAlias,
    addState,
    moveState,
    removeState,






    //Vmap
    addVmap = 0,
    vmapChanges,
    loadVmap,
    closeVmap,
    addItem,
    DelItem,
    MoveItem,
    ZValueItem,
    RotationItem,
    RectGeometryItem,
    DelPoint,
    OpacityItemChanged,
    LayerItemChanged,
    GeometryItemChanged,
    AddPoint,
    GeometryViewChanged,
    characterStateChanged,
    SetParentItem,
    MovePoint,
    VisionChanged,
    ColorChanged,

    //mediacategory
    addMedia=0,
    closeMedia,

    //SharedNoteCategory
    updateTextAndPermission,
    updateText,
    updatePermissionOneUser
};
}
/**
 * @brief The NetworkMessage class is pure virtual class to manage network message.
 */
class NetworkMessage
{
public:
    enum RecipientMode {All,OneOrMany};
    /**
     * @brief NetworkMessage
     * @param server
     */
    NetworkMessage(NetworkLink* server = nullptr);
    /**
     * @brief ~NetworkMessage
     */
    virtual ~NetworkMessage();
    /**
     * @brief sendTo
     * @param link
     */
    //void sendTo(NetworkLink * link);
    /**
     * @brief sendAll
     * @param butLink
     */
    virtual void sendToServer();
    /**
     * @brief category
     * @return
     */

    virtual NetMsg::Category category() const =0;
    /**
     * @brief action
     * @return
     */
    virtual NetMsg::Action action() const =0;
    /**
     * @brief setLinkToServer
     * @param server
     */
    void setLinkToServer(NetworkLink* server);
    virtual NetworkMessage::RecipientMode getRecipientMode() const = 0;
    virtual QStringList getRecipientList() const = 0;
    

    /**
     * @brief getSize
     * @return
     */
    quint64 getSize();
    /**
     * @brief buffer
     * @return
     */
    virtual NetworkMessageHeader *  buffer() =0;


protected:
    NetworkLink* m_linkToServer;

};
#endif
