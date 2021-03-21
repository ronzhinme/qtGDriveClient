import QtQuick 2.0
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1

import GDrive 1.0

Item {
    GDrive {
        id: gdrive
        Component.onCompleted: {
            gdrive.listFilesRequest();
        }
    }

    Connections {
        target: gdrive

        function onSigRequestError(type, itemUrl, info) {
            console.log("Request error:"+ type +" Url: "+ itemUrl +" Info: "+ info)
        }

        function onSigRequestCompleted(type, itemUrl, info) {
            console.log("Request done:"+ type +" Url: "+ itemUrl +" Info: "+ info)

            if(type === 0)
            {
                gdrive.uploadItemRequest(itemUrl, info);
            }
        }
    }

    FileDialog {
        id: openFileDialog
        fileMode: FileDialog.OpenFiles

        onAccepted: {
            console.log(openFileDialog.files)
            for (var item in openFileDialog.files) {
                gdrive.createNewFileRequest(openFileDialog.files[item]);
            }
        }
    }

    Button {
        id: updateListBtn
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        text: "Update"

        onClicked: {
            gdrive.listFilesRequest()
        }
    }

    Rectangle {
        width: listView.width + 5
        height: listView.height
        anchors.centerIn: listView
        border.width: 1
    }

    ListView {
        id: listView
        model: gdrive.files
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: updateListBtn.bottom
        anchors.bottom: uploadBtn.top
        anchors.margins: 15
        clip: true

        delegate: Text {
            text: modelData
        }

    }

    Button {
        id: uploadBtn
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        text: "Upload"

        onClicked: {
            openFileDialog.open()
        }
    }
}
