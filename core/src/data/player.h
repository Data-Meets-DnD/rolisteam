/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#ifndef PLAYER_H
#define PLAYER_H
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "person.h"
#include <QMetaType>

class NetworkLink;
/**
 * @brief Represents a player.
 *
 * Players are stored in PlayersList. A player may have some characters.
 */
class Player : public Person
{
    Q_OBJECT
public:
    Player();
    /**
     * @brief Player
     * @param name
     * @param color
     * @param master
     * @param link
     */
    Player(const QString& getName, const QColor& getColor, bool master= false, NetworkLink* link= nullptr);
    /**
     * @brief Player
     * @param uuid
     * @param name
     * @param color
     * @param master
     * @param link
     */
    Player(const QString& uuid, const QString& getName, const QColor& getColor, bool master= false,
           NetworkLink* link= nullptr);
    /**
     * @brief Player
     * @param data
     * @param link
     */
    Player(NetworkMessageReader& data, NetworkLink* link= nullptr);
    /**
     * @brief ~Player
     */
    virtual ~Player() override;
    /**
     * @brief fill
     * @param message
     */
    void fill(NetworkMessageWriter& message, bool addAvatar= true) override;
    /**
     * @brief readFromMsg
     * @param data
     */
    void readFromMsg(NetworkMessageReader& data);
    /**
     * @brief link
     * @return
     */
    NetworkLink* link() const;

    /**
     * @brief getCharactersCount
     * @return
     */
    virtual int getChildrenCount() const override;
    /**
     * @brief getCharacterByIndex
     * @param index
     * @return
     */
    Character* getCharacterByIndex(int index) const;
    /**
     * @brief getChildrenCharacter
     * @return
     */
    QList<Character*> getChildrenCharacter();
    /**
     * @brief getIndexOfCharacter
     * @param character
     * @return
     */
    int indexOf(ResourcesNode* character) const override;
    /**
     * @brief getIndexOf
     * @param id
     * @return
     */
    int getIndexOf(QString id) const;

    /**
     * @brief isGM
     * @return
     */
    bool isGM() const;
    /**
     * @brief getUserVersion
     * @return
     */
    QString getUserVersion() const;
    /**
     * @brief setUserVersion
     * @param softV
     */
    void setUserVersion(QString softV);

    /**
     * @brief hasFeature
     * @param name
     * @param version
     * @return
     */
    bool hasFeature(const QString& getName, quint8 version= 0) const;
    /**
     * @brief setFeature
     * @param name
     * @param version
     */
    void setFeature(const QString& getName, quint8 version= 0);
    /**
     * @brief addCharacter
     * @param character
     */
    void addCharacter(Character* character);
    /**
     * @brief setGM
     * @param value
     */
    void setGM(bool value);
    /**
     * @brief delCharacter
     * @param index
     */
    virtual bool removeChild(ResourcesNode*) override;

    void clearCharacterList();
    /**
     * @brief searchCharacter
     * @param character
     * @param index
     * @return
     */
    bool searchCharacter(Character* character, int& index) const;

    virtual QHash<QString, QString> getVariableDictionnary() override;

    virtual bool isLeaf() const override;

    bool isFullyDefined();

    void copyPlayer(Player* player);

private:
    friend class SendFeaturesIterator;

private:
    bool m_gameMaster= false;
    NetworkLink* m_link= nullptr;
    QList<Character*> m_characters;
    QMap<QString, quint8> m_features;
    QString m_softVersion;
};

// Q_DECLARE_METATYPE(Player)
#endif // PLAYER_H
