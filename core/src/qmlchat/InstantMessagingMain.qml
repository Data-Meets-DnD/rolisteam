import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0
import Customization 1.0

Item {
    id: root
    anchors.fill: parent
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")

    SideMenu {
        id: sideMenu
        height: parent.height
        edge: Qt.RightEdge
    }

    AddChatRoomDialog {
        id: newChatRoom
        x: -width+parent.width
        model: _ctrl.playerModel
        onChatRoomAdded: {
            root.addChat(newChatRoom.title, newChatRoom.all, newChatRoom.recipiants)
        }
    }
    Menu {
        id: menu
        x: -width+parent.width
    Action {
        text: qsTr("Settings")
        onTriggered: sideMenu.open()
    }
    Action {
        text: qsTr("Add chatroom")
        onTriggered: newChatRoom.open()
    }
    }

    SplitView {
        anchors.fill: parent
        Repeater {
            id: reaper
            model: _ctrl.mainModel
            onCountChanged: console.log("count:"+count)
            delegate: ChatView {
                chatroomModel: model.filterModel
                styleSheet: root.styleSheet
                SplitView.minimumWidth: root.styleSheet.minimumWidth
                SplitView.preferredWidth: root.width/reaper.count
                SplitView.fillHeight: true
                SplitView.fillWidth: true
                localPersonModel: _ctrl.localPersonModel
                onAddChat: _ctrl.addExtraChatroom(title, all, recipiants)
                onSplit: _ctrl.splitScreen(uuid, model.index)
                onDetach: _ctrl.detach(uuid, index, model.index)
            }
        }
    }

    ToolButton {
        anchors.top: parent.top
        anchors.right: parent.right
        icon.source: "qrc:/resources/images/menu-rounded-solid.svg"
        onClicked: menu.open()
    }
}
