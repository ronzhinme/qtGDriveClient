import QtQuick 2.0
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import QtQuick.Layouts 1.12

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
            //            console.log("Request error:"+ type +" Url: "+ itemUrl +" Info: "+ info)
        }

        function onSigRequestCompleted(type, itemUrl, info) {
            //            console.log("Request done:"+ type +" Url: "+ itemUrl +" Info: "+ info)

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

    ColumnLayout {
        spacing: 5
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        RowLayout {
            Text {
                text: qsTr("Google Access Token: ")
            }

            TextField {
                placeholderText: qsTr("Enter Google drive access token")
                Layout.fillWidth: true
                text: gdrive.token
            }

            Button {
                id: updateListBtn
                text: "Update"

                onClicked: {
                    gdrive.listFilesRequest()
                }
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: gdrive.files
            clip: true

            delegate: Text {
                text: modelData
            }

            Rectangle {
                anchors.fill: parent
                border.width: 1
                color: "transparent"
            }
        }

        Button {
            id: uploadBtn
            text: "Upload"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                openFileDialog.open()
            }
        }
    }
}
