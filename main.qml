import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import io.qt.backend 1.0

ApplicationWindow {
    id: root
    width: 300
    height: 480
    visible: true

    BackEnd {
        id: backend
    }

    Row {
        spacing: 5

        Button {
            id: fileDialogButton
            text: "Select h5 folder"
            onClicked: fileDialog.visible = true
        }

        Button {
            id: h5DecodeButton
            text: "Run h5 Decode"
            onClicked: backend.decodeh5()
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
