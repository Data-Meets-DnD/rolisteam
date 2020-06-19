/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QComboBox>
#include <QFile>
#include <QMdiSubWindow>
#include <QPointer>
#include <QSplitter>
#include <QTextBrowser>
#include <QToolBar>

#include "chatbrowser.h"
#include "common/widgets/colorbutton.h"
#include "network/networkmessage.h"
#include "preferences/preferencesmanager.h"

#include "data/diceshortcut.h"
#include "data/person.h"
#include "diceparser.h"
#include <memory>

class AbstractChat;
class MainWindow;
class Player;
class ImprovedTextEdit;

/**
 * @brief chat window display on screen the chat.
 */
class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    enum CHAT_OPERATOR
    {
        NONE,
        DICEROLL,
        SECRET_DICEROLL,
        COMMAND,
        TO_GM_DICEROLL
    };
    /**
     * @brief ChatWindow
     * @param chat
     * @param parent
     */
    ChatWindow(AbstractChat* chat, QWidget* parent= nullptr);
    /**
     * @brief ~ChatWindow
     */
    virtual ~ChatWindow();
    /**
     * @brief chat
     * @return
     */
    AbstractChat* chat() const;
    /**
     * @brief diceToText
     * @param dice
     * @return
     */
    QStringList diceToText(QList<ExportedDiceResult>& dice);
    /**
     * @brief toggleViewAction
     * @return
     */
    QAction* toggleViewAction() const;
    /**
     * @brief showMessage
     * @param utilisateur
     * @param couleur
     * @param message
     * @param msgtype
     */
    void showMessage(const QString& utilisateur, const QColor& couleur, const QString& message,
                     const QString& comment= QString(), NetMsg::Action msgtype= NetMsg::InstantMessageAction);
    /**
     * @brief hasUnseenMessage
     * @return
     */
    bool hasUnseenMessage() const;
    /**
     * @brief setSubWindow
     * @param subWindow
     */
    void setSubWindow(QMdiSubWindow* subWindow);
    /**
     * @brief getSubWindow
     * @return
     */
    QMdiSubWindow* getSubWindow();
    /**
     * @brief setLocalPlayer
     */
    void setLocalPlayer(Person*);

    /**
     * @brief updateDiceAliases
     * @param map
     */
    static void updateDiceAliases(QList<DiceAlias*>* map);

    ImprovedTextEdit* getEditionZone() const;

    void setEditionZone(ImprovedTextEdit* editionZone);

    bool isTimeShown() const;

    Qt::DropActions supportedDropActions() const;
    void appendDiceShortCut(const DiceShortCut& pair);

    std::vector<DiceShortCut>& getDiceShortCuts();
    void contextMenuEvent(QContextMenuEvent* event);
signals:
    /**
     * @brief ChatWindowHasChanged
     * @param what
     */
    void ChatWindowHasChanged(ChatWindow* what);

public slots:
    /**
     * @brief isVisible
     * @return
     */
    virtual bool isVisible();
    /**
     * @brief save
     */
    void save();
    /**
     * @brief getTitleFromChat
     * @return
     */
    QString getTitleFromChat();
    /**
     * @brief editionGetFocus
     */
    void editionGetFocus();
    /**
     * @brief detachView
     * @param b
     */
    void detachView(bool b);

    void rollDiceCmd(QString cmd, QString owner, bool alias);
    void rollDiceCmdForCharacter(QString cmd, QString uuid, bool alias);

    void showTime(bool showTime);

protected:
    /**
     * @brief init
     * @param parent
     */
    void init(MainWindow* parent);
    /**
     * @brief showEvent
     * @param event
     */
    void showEvent(QShowEvent* event);
    /**
     * @brief hideEvent
     * @param event
     */
    void hideEvent(QHideEvent* event);
    /**
     * @brief updateListAlias
     */
    void updateListAlias();
    /**
     * @brief setProperDictionnary
     * @param idOwner
     */
    void setProperDictionnary(QString idOwner);
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void updateAction();
    void removeAllShortcut();
    void createAction(const DiceShortCut& pair);
private slots:
    /**
     * @brief sendOffTextMessage
     * @param hasHtml true if the given message has html
     * @param message to send.
     */
    void sendOffTextMessage(bool hasHtml, QString message);
    /**
     * @brief upSelectPerson
     */
    void upSelectPerson();
    /**
     * @brief downSelectPerson
     */
    void downSelectPerson();
    /**
     * @brief scheduleUpdateChatMembers
     */
    void scheduleUpdateChatMembers();
    /**
     * @brief updateChatMembers
     */
    void updateChatMembers();
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief getMessageResult
     */
    bool getMessageResult(QString& mainResult, QString& detailedResult);
    /**
     * @brief manageDiceRoll
     * @param str
     * @param secret
     */
    void manageDiceRoll(QString str, QString& messageTitle, QString& message, bool alias, bool showResult= true);

private:
    static QStringList m_keyWordList;
    QMdiSubWindow* m_window;
    QPointer<AbstractChat> m_chat;
    QString m_filename;
    bool m_hasUnseenMessage;
    PreferencesManager* m_preferences;
    ChatBrowser* m_displayZone;
    QComboBox* m_selectPersonComboBox;
    ImprovedTextEdit* m_editionZone;
    QAction* m_toggleViewAction;
    std::unique_ptr<DiceParser> m_diceParser;
    QMap<QString, CHAT_OPERATOR>* m_operatorMap;
    Person* m_localPerson;
    static QList<DiceAlias*>* m_receivedAlias;
    QHash<QString, QHash<QString, QString>> m_dicoByCharacter;
    std::vector<DiceShortCut> m_diceBookMarks;
    std::vector<QAction*> m_actionList;
    bool m_showTime;
    QToolBar* m_toolBar;
};

#endif
