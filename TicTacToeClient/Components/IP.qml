import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import Qt5Compat.GraphicalEffects

import ClientState 1.0
//import "../Components"

Item {
    
    id: widget
    
    Rectangle {

        id: background

        anchors.fill: parent

        color: "lightgray"
        opacity: 0.75
    }

    TransparentButton {

        width: 25
        height: 25

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20

        text: "x"
        fontSize: 17

        onClicked: {
            Qt.inputMethod.hide();
            widget.visible = false;
        }
    }

    Column {

        anchors.centerIn: parent
        spacing: 10

        Rectangle {

            id: field

            width: 300
            height: 50

            radius: 25
            color: "white"

            layer.enabled: true
            layer.effect: DropShadow {
                anchors.fill: field
                horizontalOffset: 5
                verticalOffset: 5
                radius: 8
                color: "gray"
                source: field
            }

            TextInput {

                id: address

                anchors.fill: parent

                verticalAlignment: "AlignVCenter"
                horizontalAlignment: "AlignHCenter"

                text: worker.crstrGetServerAddress()
                font.pointSize: 13

                selectByMouse: true

                onActiveFocusOnPressChanged: {
                  if (widget.visible) {
                    Qt.inputMethod.show();
                  }
                }
            }
        }

        TransparentButton {

            anchors.horizontalCenter: parent.horizontalCenter

            width: 100
            height: 50

            text: "Submit"
            fontSize: 18

            onClicked: {
                Qt.inputMethod.hide();
                widget.visible = false;
                worker.vUpdateServerAddress(address.text);
            }
        }
    }

}
