import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt5Compat.GraphicalEffects

    
Text {

    id: title

    property string firstColor: "#fff1eb"
    property string secondColor: "#ace0f9"

    font.pointSize: 15
    font.bold: true
    font.family: fontCustom.name
    color: "white"

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
    layer.effect: LinearGradient {

        id: gradient

        cached: true
        anchors.fill: title
        gradient: Gradient {
            GradientStop { position: 0; color: firstColor }
            GradientStop { position: 1; color: secondColor }
        }
        source: title

        layer.enabled: true
        layer.effect: DropShadow {

            anchors.fill: gradient
            radius: 8
            horizontalOffset: 5
            verticalOffset: 5
            color: "gray"
            source: gradient
          }
    }


    //style: Text.Outline
    //styleColor: "gray"

    // Font
    FontLoader {
        id: fontCustom
        source: "../Fonts/comic.ttf"
    }

    NumberAnimation {

        id: animation

        target: title
        property: "opacity"

        duration: 1000
        easing.type: Easing.InOutQuad
    }
}


