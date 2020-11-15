/*************************************************************************
 *     Copyright (C) 2007 by Romain Campioni                             *
 *     Copyright (C) 2009 by Renaud Guezennec                            *
 *     Copyright (C) 2010 by Berenger Morel                              *
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMenu>
#include <QProgressBar>
#include <QStackedWidget>
#include <QTextEdit>
#include <QUndoStack>
#include <memory>

#include "data/cleveruri.h"
#include "data/mediacontainer.h"

//#include "map/map.h"
#include "map/charactertoken.h"

#include "preferences/preferencesmanager.h"

#include "services/ipchecker.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/networkreceiver.h"

#include "vmap/vmap.h"
#include "vmap/vmapframe.h"
#include "vmap/vmaptoolbar.h"
#include "vmap/vtoolbar.h"

#include "network/selectconnectionprofiledialog.h"
#include "noteeditor/src/notecontainer.h"
#include "sharededitor/sharednotecontainer.h"

#ifndef NULL_PLAYER
#include "audio/audioPlayer.h"
#endif

#include "network/channellistpanel.h"

// log
#include "common/controller/logcontroller.h"
#include "common/controller/remotelogcontroller.h"

#include "controller/gamecontroller.h"
#include "userlist/playerspanel.h"

namespace Ui
{
class MainWindow;
}

class UpdateChecker;
class ToolsBar;
class MapFrame;
class Map;
class ChatListWidget;
class Image;
class NetworkLink;
class PreferencesDialog;
class Player;
class PlayersListWidget;
class Workspace;
class TextEdit;
class PlayerModel;
class ConnectionProfile;
class SessionDock;
class CharacterSheetWindow;
class GameMasterTool;
class LogPanel;
class ServerManager;

struct CommandLineProfile
{
    QString m_ip;
    int m_port;
    QByteArray m_pass;
};
/**
 * @brief Main widget for rolisteam, it herits from QMainWindow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow
     */
    MainWindow();
    /**
     * @brief ~MainWindow
     */
    virtual ~MainWindow();

    /**
     * @brief addMap
     * @param BipMapWindow
     * @param titre
     * @param mapsize
     * @param pos
     */
    QWidget* addMap(MapFrame* MapFrame, QString titre, QSize mapsize= QSize(), QPoint pos= QPoint());
    /**
     * @brief mettreAJourEspaceTravail
     */
    void updateWorkspace();
    /**
     * @brief trouverCarte
     * @param idMap
     */
    Map* findMapById(QString idMap);
    /**
     * @brief removeMapFromId
     * @param idMap
     */
    void removeMapFromId(QString idMap);
    /**
     * @brief removeVMapFromId
     * @param idMap
     */
    void removeVMapFromId(QString idMap);
    /**
     * @brief removePictureFromId
     * @param idImage
     */
    void removePictureFromId(QString idImage);
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief getPermission
     * @param id
     * @return
     */
    // Map::PermissionMode getPermission(int id);
    /**
     * @brief readSettings
     */
    void readSettings();
    /**
     * @brief writeSettings
     */
    void writeSettings();
    /**
     * @brief setUpNetworkConnection
     */
    void setUpNetworkConnection();
    /**
     * @brief parseCommandLineArguments
     */
    void parseCommandLineArguments(QStringList);

    /**
     * @brief processMessage
     * @param msg
     */
//    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

    /**
     * @brief prepareMap
     * @param mapFrame
     */
    void prepareMap(MapFrame* mapFrame);
    /**
     * @brief prepareVMap
     * @param tmp
     */
    void prepareVMap(VMapFrame* tmp);
    /**
     * @brief addMediaToMdiArea
     * @param mediac
     */
    void addMediaToMdiArea(MediaContainer* mediac, bool undoable= true);

    /**
     * @brief setLatestFile
     * @param fileName
     */
    void setLatestFile(CleverURI* fileName);

    void openRecentScenario();
    void updateRecentScenarioAction();
public slots:
    /**
     * @brief
     * @param
     */
    bool mayBeSaved(bool connectionLost= false);
    /**
     * @brief updateUi
     */
    void updateUi();
    /**
     * @brief updateWindowTitle
     */
    void updateWindowTitle();
    /**
     * @brief closeAllImagesAndMap - remove all maps and Pictures
     */
    void closeAllMediaContainer();

    /**
     * @brief showConnectionDialog
     */
    void showConnectionDialog(bool forced= false);
    /**
     * @brief postConnection
     */
    void postConnection();
    /**
     * @brief openImageAs
     * @param pix
     * @param type
     */
    void openImageAs(const QPixmap pix, CleverURI::ContentType type);

protected:
    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    /**
     * @brief processImageMessage
     * @param msg
     */
    void processImageMessage(NetworkMessageReader* msg);
    /**
     * @brief processMapMessage
     * @param msg
     */
  //  void processMapMessage(NetworkMessageReader* msg);
    /**
     * @brief processNpcMessage
     * @param msg
     */
    void processNpcMessage(NetworkMessageReader* msg);
    /**
     * @brief processPaintingMessage
     * @param msg
     */
    void processPaintingMessage(NetworkMessageReader* msg);
    /**
     * @brief processCharacterMessage
     * @param msg
     */
    void processCharacterMessage(NetworkMessageReader* msg);
    /**
     * @brief processAdminstrationMessage
     * @param msg
     */
    void processAdminstrationMessage(NetworkMessageReader* msg);
    /**
     * @brief processCharacterPlayerMessage
     * @param msg
     */
    void processCharacterPlayerMessage(NetworkMessageReader* msg);
    /**
     * @brief processMediaMessage
     * @param msg
     */
    void processMediaMessage(NetworkMessageReader* msg);
    void processWebPageMessage(NetworkMessageReader* msg);
    /**
     * @brief processVMapMessage
     * @param msg
     * @return
     */
//    NetWorkReceiver::SendType processVMapMessage(NetworkMessageReader* msg);
    /**
     * @brief extractCharacter
     * @param map
     * @param msg
     */
    void extractCharacter(Map* map, NetworkMessageReader* msg);
    /**
     * @brief contentToPath
     * @param type
     * @param save
     * @return
     */
    CleverURI* contentToPath(CleverURI::ContentType type, bool save);
    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QDropEvent* event);
    /**
     * @brief dragEnterEvent
     * @param ev
     */
    void dragEnterEvent(QDragEnterEvent* ev);
    /**
     * @brief prepareCharacterSheetWindow
     * @param window
     */
    void prepareCharacterSheetWindow(CharacterSheetWindow* window);
    /**
     * @brief saveAllMediaContainer
     */
    void saveAllMediaContainer();
    /**
     * @brief saveMedia
     * @param mediaC
     * @param AskPath
     * @param saveAs
     */
    void saveMedia(MediaContainer* mediaC, bool saveAs);
    void readStory(QString fileName);
    void processSharedNoteMessage(NetworkMessageReader* msg);
    void showTipChecker();
    virtual void mouseMoveEvent(QMouseEvent* event);
    void createPostSettings();
protected slots:
    /**
     * @brief closeMediaContainer
     * @param id
     */
    void closeMediaContainer(QString id, bool redo);
    void cleanUpData();
    MediaContainer* newDocument(CleverURI::ContentType type, bool addMdi= true);
private slots:
    void showSupportPage();
    /**
     * @brief userNatureChange
     * @param isGM
     */
    void userNatureChange(bool isGM);
    /**
     * @brief activeWindowChanged
     * @param widget
     */
    void activeWindowChanged(QMdiSubWindow* widget);
    /**
     * @brief openStory
     */
    void openStory();
    /**
     * @brief closeCurrentSubWindow
     */
    void closeCurrentSubWindow();
    /**
     * @brief showUpdateNotification
     */
    void showUpdateNotification();
    /**
     * @brief receiveData
     * @param readData
     * @param size
     */
    void receiveData(quint64 readData, quint64 size);
    /**
     * @brief openContent
     */
    void openContent();
    // save methods
    /**
     * @brief saveCurrentMedia
     */
    void saveCurrentMedia();
    // Network private Slot
    /**
     * @brief stopReconnection
     */
    void stopReconnection();
    /**
     * @brief closeConnection
     */
    void closeConnection();
    /**
     * @brief openContentFromType
     * @param type
     */
    void openContentFromType(CleverURI::ContentType type);
    /**
     * @brief openCleverURI
     * @param uri
     * @param force
     */
    void openCleverURI(CleverURI* uri, bool force= false);
    void openResource(ResourcesNode* node, bool force);
    /**
     * @brief updateRecentFileActions
     */
    void updateRecentFileActions();
    /**
     * @brief openRecentFile
     */
    void openRecentFile();

    /// \brief open the Qt assistant with the rolisteam documentation
    /**
     * @brief helpOnLine
     */
    void helpOnLine();
    /**
     * @brief saveStory - saves all media contener into the current story file.
     *
     */
    bool saveStory(bool saveAs);
    /**
     * @brief saveMinutes saves notes into file.
     * @return
     */
    bool saveMinutes();
    /**
     * @brief notifyAboutAddedPlayer
     * @param player
     */
    void notifyAboutAddedPlayer(Player* player) const;
    /**
     * @brief notifyAboutDeletedPlayer
     * @param player
     */
    void notifyAboutDeletedPlayer(Player* player) const;

    void showShortCutEditor();

private:
    /**
     * @brief createNotificationZone
     */
    void createNotificationZone();
    /**
     * @brief linkActionToMenu
     */
    void linkActionToMenu();
    /**
     * @brief getContentType
     * @param str
     * @return
     */
    CleverURI::ContentType getContentType(QString str);
    /**
     * @brief findCharacterSheetWindowById
     * @param id
     * @return
     */
    CharacterSheetWindow* findCharacterSheetWindowById(const QString& idMedia, const QString& idSheet);
    /**
     * @brief getShortNameFromPath generic tool to translate filepath to short name.
     * @param path
     * @return
     */
    QString getShortNameFromPath(QString path);

private:
    Workspace* m_mdiArea= nullptr;
    PlayersPanel* m_playersListWidget= nullptr;

    // toolbar
    ToolsBar* m_toolBar;
    VToolsBar* m_vToolBar;
    QStackedWidget* m_toolBarStack;

    QHash<QString, MediaContainer*> m_mediaHash;
    QMap<MediaContainer*, QAction*>* m_mapAction;
#ifndef NULL_PLAYER
    AudioPlayer* m_audioPlayer;
#endif

    PreferencesDialog* m_preferencesDialog;
    PreferencesManager* m_preferences;
    ChatListWidget* m_chatListWidget;

    QDockWidget* m_dockLogUtil;
    LogPanel* m_notifierDisplay;
    PlayerModel* m_playerModel;

    // subwindow
    QProgressBar* m_downLoadProgressbar;
    bool m_shownProgress;
    Ui::MainWindow* m_ui;
    std::unique_ptr<SessionDock> m_sessionDock;
    bool m_resetSettings;

    // Recent files managment
    int m_maxSizeRecentFile;
    QList<QAction*> m_recentFileActs;
    std::vector<QAction*> m_recentScenarioActions;
    QStringList m_recentScenarios;
    QAction* m_separatorAction;
    CleverUriList m_recentFiles;

    VmapToolBar* m_vmapToolBar;

    ConnectionProfile* m_currentConnectionProfile= nullptr;
    QList<GameMasterTool*> m_gmToolBoxList;
    SelectConnectionProfileDialog* m_dialog= nullptr;
    bool m_profileDefined;
    QDockWidget* m_roomPanelDockWidget;
    ChannelListPanel* m_roomPanel;
    QString m_connectionAddress;
    bool m_isOut= false;
    QByteArray m_passwordAdmin;

    std::unique_ptr<CommandLineProfile> m_commandLineProfile;
    std::unique_ptr<GameController> m_gameController;
};

#endif
