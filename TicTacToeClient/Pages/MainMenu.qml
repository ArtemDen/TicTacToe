import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import ClientState 1.0
import "../Components"

Page {
    
    id: menu

    signal sigPlay();

    Rectangle {

        id: background

        anchors.fill: parent
        gradient: "OldHat"
    }
    
    StyleText {

        id: title

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: font.pointSize * 1.5

        font.pointSize: 40
        text: "TicTacToe"
    }

    StyleText {

        id: labelID

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: font.pointSize * 2

        font.pointSize: 25
        opacity: 0
    }

    TransparentButton {

        id: buttonPlay

        width: 300
        height: 200

        opacity: 0

        text: "Push to start!"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.bottomMargin: title.font.pointSize * 2

        onClicked: {
            buttonPlay.enabled = false;
            infoBox.show("Search for opponent...");
            timerSearch.start();
            worker.vChangeState(ClientState.Searching);
        }
    }

    TransparentButton {

        id: buttonIP

        width: 100
        height: 30

        text: "Server IP"
        fontSize: 14

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 15

        onClicked: {
            widgetIP.visible = true;
        }
    }


    IP {

         id: widgetIP

         anchors.fill: parent
         visible: false
         z: 1
    }


    Timer {

        id: timerSearch

        repeat: false
        interval: 10000

        onTriggered: {
           buttonPlay.enabled = true;
           infoBox.hide("No players found :(");
           worker.vChangeState(ClientState.Default);
        }
    }

    InfoBox {

        id: infoBox

        anchors.horizontalCenter: parent.horizontalCenter;
        y: parent.height;
    }

    Connections {

        target: worker

        function onSigConnecting() {
            labelID.hide();
            buttonPlay.hide();
            buttonPlay.enabled = false;
            infoBox.show("Connecting...");
            buttonIP.show();
        }

        function onSigConnected() {
            buttonPlay.show();
            buttonPlay.enabled = true;
            infoBox.hide("Connected succesfully!");
            buttonIP.hide();
        }

        function onSigID(ID) {
            labelID.text = "ID: " + ID;
            labelID.show();
        }

        function onSigStartGame() {
            infoBox.hide("Opponent found! ID: " + worker.uiGetOpponentID());
            buttonPlay.enabled = true;
            timerSearch.stop();
            sigPlay();
        }
    }
}
