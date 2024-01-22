import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt5Compat.GraphicalEffects

Rectangle {

    id: button

    property alias text: textButton.text
    property alias fontSize: textButton.font.pointSize

    width: 300
    height: 50
    radius: 50
    color: "transparent"

    signal clicked()

    function show() {
        if (!opacity) {
            animation.from = 0;
            animation.to = 1;
            animation.start();
        }
    }

    function hide() {
        if (opacity) {
            animation.from = 1;
            animation.to = 0;
            animation.start();
        }
    }

    layer.enabled: true
    layer.effect: DropShadow {
        anchors.fill: button
        horizontalOffset: mouse.containsPress ? button.height / 60.0 : button.height / 30.0
        verticalOffset: mouse.containsPress ? button.height / 50.0 : button.height / 25.0
        radius: 8
        color: "gray"
        source: button
    }

    // Font
    FontLoader {
        id: fontCustom
        source: "../Fonts/comic.ttf"
    }

    Text {
        id: textButton
        anchors.centerIn: parent
        font.family: fontCustom.name
        font.pointSize: parent.width / 10.0
        font.bold: true
        text: "text"
        color: "black"
        opacity: 0.7
    }
    
    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: {
            button.clicked();
        }
    }

    NumberAnimation {

        id: animation

        target: button
        property: "opacity"

        duration: 1000
        easing.type: Easing.InOutQuad
    }
}
