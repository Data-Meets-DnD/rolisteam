import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0
import Customization 1.0

Item {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property alias localPersonModel: imEditText.model
    property alias chatroomModel: repeater.model
    property ChatRoom chatRoom:  chatroomModel.get(tabHeader.currentIndex)
    signal addChat(var title, var all, var recipiants)
    signal split(var uuid, var index)
    signal detach(var uuid, var index)

    SoundEffect {
        id: effect
        source: "qrc:/resources/sounds/Doorbell.wav"
        muted: !sideMenu.sound
        volume: 1.0
    }


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        Menu {
            id: contextMenu
            Action {
                text: qsTr("Split view")
                onTriggered: {
                    root.split(root.chatRoom.uuid, tabHeader.currentIndex)
                    root.chatRoom =  chatroomModel.get(tabHeader.currentIndex)
                }
            }
            Action {
                text: qsTr("Detach")
                onTriggered: root.detach(root.chatRoom.uuid, tabHeader.currentIndex)
            }
            Action {
                text: qsTr("Reatach")
                //onTriggered: root.detach(root.chatRoom.uuid, tabHeader.currentIndex)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            TabBar {
                id: tabHeader
                Layout.fillWidth: true
                Repeater {
                    id: repeater
                    TabButton {
                        id: tabButton
                        property bool current: tabHeader.currentIndex === index
                        background: Rectangle {
                            color: tabButton.current ? "white" : model.unread ? "red" : "#242424"
                        }

                        contentItem: RowLayout {
                            Label {
                                text: model.unread ? "%1 (\*)".arg(model.title) : model.title
                                Layout.fillWidth: true
                                horizontalAlignment: Qt.AlignHCenter
                                color: tabButton.current ? "black" : "white"
                            }
                            ToolButton {
                                visible: model.closable
                                text: "X"
                                ToolTip.text: qsTr("close")
                                ToolTip.visible: down
                            }
                        }
                        Connections {
                            target: model.chatroom
                            function onUnreadMessageChanged(unread) {
                                if(unread && !tabButton.current)
                                    effect.play()
                            }
                        }
                    }
                }
            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: root.styleSheet.sideMargin
            Layout.rightMargin: root.styleSheet.sideMargin
            orientation: Qt.Vertical

            ListView {
                id: listView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                model: root.chatRoom.messageModel
                clip: true
                spacing: 5
                verticalLayoutDirection: ListView.BottomToTop
                delegate: Component {
                    id: delegateComponent
                    Loader {
                        property bool isTextMessage: model.type === MessageInterface.Text
                        anchors.right: (isTextMessage && model.local) ? parent.right : undefined
                        width:isTextMessage ? undefined:  parent.width-10
                        source: isTextMessage ? "TextMessageDelegate.qml" : "DiceMessageDelegate.qml"
                    }
                }
            }

            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: root.styleSheet.preferredHeight

                onSendClicked: root.chatRoom.addMessage(text, imEditText.currentPersonId)
                function updateUnread() {
                    root.chatRoom.unreadMessage = false
                }
                onFocusGained: updateUnread()
            }

        }
    }
}
