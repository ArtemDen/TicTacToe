import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import ClientState 1.0
import Figure 1.0

import "../Components"

Page {
    
    id: pageGame

    property int opponentID: 0
    property int myFigure: Figure.None
    property string currentStatus: stringStart
    property bool gameOver: false

    property string stringNone:   ""
    property string stringCross:  "x"
    property string stringCircle: "o"

    property string stringStart:        "Starting game..."
    property string stringOpponentTurn: "Opponent turn!"
    property string stringYourTurn:     "Your turn!"
    property string stringDisconnect:   "Opponent disconnected!"
    property string stringWin:          "You win!"
    property string stringLose:         "You lose!"
    property string stringDraw:         "It's a draw!"

    function getFigure(figureID) {
        switch (figureID)
        {
            case Figure.None:
                return "";
            case Figure.Circle:
                return "o";
            case Figure.Cross:
                return "x";
        }
    }

    function getGradient(figure, sublight) {
        if (sublight) {
            if (figure === myFigure) {
                return "NightFade";
            }
            else {
                return "AmourAmour"
            }
        }
        else {
            return "HeavyRain"
        }
    }

    Rectangle {

        id: background

        anchors.fill: parent
        gradient: "NewYork"
    }

    TableView {

        id: battlefieldView

        anchors.centerIn: parent

        width: 250
        height: 250

        contentWidth: width
        contentHeight: height

        model: battlefieldModel

        rowSpacing: 10
        columnSpacing: 10

        delegate: Rectangle {

            radius: 10

            implicitWidth: 80
            implicitHeight: 80

            gradient: getGradient(display, decoration)
            //color: "transparent"

            border.color: "gray"
            border.width: 1

            StyleText {

                id: delegateText

                anchors.centerIn: parent

                firstColor: "gray"
                secondColor: "black"

                text: getFigure(display)

                font.pointSize: 40
                font.bold: false

                opacity: 0

                onTextChanged: {
                    currentStatus = worker.bCanTurn() ? stringYourTurn : stringOpponentTurn;
                    show();
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (worker.bCanTurn() && (display === Figure.None)) {
                        worker.vMakeTurn(row, column);
                        display = worker.uiGetFigure();
                        currentStatus = stringOpponentTurn;
                        worker.vCheckGameOver();
                    }
                }
            }
        }

        Component.onDestruction: {
            battlefieldModel.clear();
        }
    }
    
    TransparentButton {
        
        id: exit
        
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        
        width: 50
        height: 50
        
        text: "X"
        fontSize: 25
        
        onClicked: {
            pageGame.parent.pop();
            pageGame.destroy();
            worker.vChangeState(ClientState.Default);
        }
    }
    
    StyleText {
        
        id: opponent
        
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        
        font.pointSize: 18
        text: "Opponent ID: " + opponentID

        firstColor: "gray"
        secondColor: "black"
    }

    StyleText {

        id: status

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        font.pointSize: 18
        text: currentStatus

        firstColor: "gray"
        secondColor: "black"
    }

    Connections {

        target: worker

        function onSigStopGame() {
            if (!gameOver) {
                status.text = stringDisconnect;
                battlefieldView.enabled = false;
            }
            worker.vChangeState(ClientState.Default);
        }

        function onSigGameOver(figure) {
            if (figure === myFigure) {
                status.text = stringWin;
            }
            else if (figure === Figure.None) {
                status.text = stringDraw;
            }
            else {
               status.text = stringLose;
            }
            battlefieldView.enabled = false;
            gameOver = true;
        }
    }
}
