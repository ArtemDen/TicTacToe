import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import Qt5Compat.GraphicalEffects

Item {
    
    id: infoBox
    
    width: 200
    height: 50

    property string text: "Connecting..."

    function show(information) {

        if (animation.running) {
            animation.complete();
        }

        text = information;
        animation.from = y;
        animation.to = parent.height / 2 - infoBox.height / 2;
        animation.start();
    }

    function hide(information) {

        if (animation.running) {
            animation.complete();
        }

        text = information;
        animation.from = parent.height / 2 - infoBox.height / 2;;
        animation.to = parent.height;
        animation.start();
    }
    
    Rectangle {
        
        id: border
        
        anchors.fill: parent
        
        radius: 10

        //border.color: "gray"
        //border.width: 1
        
        gradient: "NewYork"

        Text {
            anchors.centerIn: parent
            font.pointSize: 10
            text: infoBox.text
        }

        layer.enabled: true
        layer.effect: DropShadow {
            anchors.fill: infoBox
            horizontalOffset: 5
            verticalOffset: 5
            radius: 8
            color: "gray"
            source: border
        }
    }

    NumberAnimation {

        id: animation

        target: infoBox
        property: "y"

        duration: 1000

        easing.type: Easing.InOutQuad
    }
    
}
