import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.1
import QtGraphicalEffects 1.0
import io.qt.backend 1.0

ApplicationWindow {
    id: root
    width: 1200
    height: 600
    visible: true
    background: LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(0, 300)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#8e9eab" }
            GradientStop { position: 1.0; color: "#eef2f3" }
        }
    }

    BackEnd {
        id: backend
        onFinished: finishedPopup.open()
        onFinishedh5: {
            progressh5.close()
            apidSelect.open()
        }
    }

    Label {
        id: title
        x: 491
        y: 44
        text: "Decom Utility"
        font.pixelSize: 32
        font.bold: true
    }

    Popup {
        id: runSelectWindow
        x: 300
        y: 133
        width: 600
        height: 334
        modal: true
        focus: true
        Column {
            Text {
                text: "Select files to Decom"
                font.pixelSize: 22
            }

            ListView {
                width: 100; height: 100

                model: backend.ofiles
                delegate: CheckBox {
                    id: cb
                    anchors.left: parent.left
                    text: modelData
                    onClicked: cb.checked ? backend.addPacketFile(modelData) : backend.removePacketFile(modelData)
                }
            }
        }

        Button {
            text: "Execute"
            font.pixelSize: 22
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onClicked: {
                runSelectWindow.close()
                progress.open()
                backend.runDecom()
            }
        }
    }

    Popup {
        id: progress
        x: 10
        y: 133
        width: 1180
        height: 334
        modal: true
        focus: true
        Column {
            Text {
                text: "Decom Current File: "
                font.pixelSize: 22
            }
            Text {
                text: backend.currentFile
                font.pixelSize: 22
            }
            Text {
                text: backend.progress
                font.pixelSize: 22
            }
        }
        Button {
            text: "Exit"
            id: exitButton
            visible: false
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            font.pixelSize: 22
            onClicked: Qt.quit()
        }
    }

    Popup {
        id: progressh5
        x: 10
        y: 133
        width: 1180
        height: 334
        modal: true
        focus: true
        Column {
            Text {
                text: backend.progress
                font.pixelSize: 22
            }
        }
    }

    Popup {
        id: apidSelect
        x: 300
        y: 133
        width: 600
        height: 334
        modal: true
        focus: true
        Column {
            spacing: 30
            Text {
                text: "Enter specific APIDs (NOT IMPLEMENTED YET)"
                font.pixelSize: 22
            }
            TextField {
                placeholderText: qsTr("Comma-Separated APIDs")
            }
            Text {
                text: "Or Select All APIDs"
                font.pixelSize: 22
            }
            CheckBox {
                text: "All APIDs"
                font.pixelSize: 22
                checked: true
            }
        }

        Button {
            text: "Execute"
            font.pixelSize: 22
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onClicked: {
                apidSelect.close()
                runSelectWindow.open()
            }
        }
    }

    Column {
        anchors.verticalCenterOffset: 66
        anchors.horizontalCenterOffset: 0
        spacing: 80
        anchors.centerIn: parent

        Button {
            id: fileDialogButton
            text: "Select h5 folder"
            scale: 2
            onClicked: fileDialog.visible = true
        }

        Button {
            id: h5DecodeButton
            text: "Run h5 Decode"
            scale: 2
            onClicked: {
                progressh5.open()
                backend.decodeh5()
            }
        }
    }

    FileDialog {
        id: fileDialog
        selectFolder: true
        title: "Select h5 folder"
        folder: "data"
        visible: false
        onAccepted: {
            backend.folderName = fileDialog.fileUrl
            console.log(fileDialog.fileUrl)
            visible: false
        }
        onRejected: {
            visible: false
        }
    }

    MessageDialog {
        id: finishedPopup
        title: "Finished"
        text: "Decom has finished."
        onAccepted: exitButton.visible = true
        Component.onCompleted: visible = false
    }
}