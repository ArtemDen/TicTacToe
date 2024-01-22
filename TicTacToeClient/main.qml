import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import "Pages"

Window {

    id: window

    visible: true
    width: 640
    height: 480
    title: "XO"

    function start() {
        var componentMainMenu = Qt.createComponent("Pages/MainMenu.qml");
        if(componentMainMenu.status === Component.Ready) {
            var pageMainMenu = componentMainMenu.createObject(stack);
            pageMainMenu.sigPlay.connect(play);
            stack.push(pageMainMenu);
            worker.vStart();
        }
    }

    function play() {
        var componentGame = Qt.createComponent("Pages/Game.qml");
        if(componentGame.status === Component.Ready) {
            var pageGame = componentGame.createObject(stack);
            pageGame.opponentID = worker.uiGetOpponentID();
            pageGame.myFigure = worker.uiGetFigure();
            pageGame.currentStatus = worker.bCanTurn() ? pageGame.stringYourTurn : pageGame.stringOpponentTurn;
            stack.push(pageGame);
        }
    }

    StackView {

        id: stack
        anchors.fill: parent

        Component.onCompleted: {
            start();
        }
    }
}
