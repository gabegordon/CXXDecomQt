import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import io.qt.backend 1.0

ApplicationWindow {
    id: root
    width: 800
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
    }

    Label {
        x: 325
        y: 47
        text: "Decom Utility"
        font.pixelSize: 22
    }

    Column {
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
            onClicked: backend.decodeh5()
        }

        Button {
            id: mainRunButton
            text: "Run Decom"
            scale: 2
            onClicked: backend.runDecom()
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
            visible: false
        }
        onRejected: {
            visible: false
        }
    }
}