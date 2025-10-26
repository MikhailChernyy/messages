import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "Просмотрщик сообщений"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "Сообщения"
                font.pixelSize: 18
                font.bold: true
                Layout.leftMargin: 10
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Обновить"
                onClicked: messageModel.refresh()
            }
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true

        ListView {
            id: listView
            model: messageModel
            spacing: 15

            delegate: Column {
                width: listView.width
                spacing: 5

                Row {
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Text {
                        text: model.sender || ""
                        font.bold: true
                        font.pixelSize: 14
                        color: "#2c3e50"
                        width: 150
                        elide: Text.ElideRight
                    }

                    Text {
                        text: model.timestamp || ""
                        font.pixelSize: 12
                        color: "#7f8c8d"
                    }
                }

                Rectangle {
                    width: parent.width - 20
                    height: messageText.implicitHeight + 20
                    color: "#f8f9fa"
                    border.color: "#e9ecef"
                    border.width: 1
                    radius: 8
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        id: messageText
                        text: model.text || ""
                        font.pixelSize: 14
                        color: "#2c3e50"
                        wrapMode: Text.Wrap
                        width: parent.width - 20
                        anchors.centerIn: parent
                    }
                }
            }

            onContentYChanged: {
                if (contentY + height >= contentHeight - 100) {
                    messageModel.loadMoreMessages()
                }
            }

            footer: Item {
                width: listView.width
                height: 50

                BusyIndicator {
                    id: loadingIndicator
                    running: listView.contentY + listView.height >= listView.contentHeight - 100
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                }

                Text {
                    anchors.top: loadingIndicator.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Загрузка..."
                    visible: loadingIndicator.running
                }
            }
        }
    }

    footer: Label {
        padding: 10
        text: "Загружено сообщений: " + listView.count
        color: "gray"
    }
}


