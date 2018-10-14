import QtQuick 2.9
import QtQuick.Controls 2.2
Rectangle {
    id: root
    implicitWidth: 50
    implicitHeight: 40
    property alias text: btnText.text
    signal clicked()
    color: btnArea.pressed ? "red" : (btnArea.containsMouse ? "darkGray" : "white")
    property alias pressed: btnArea.pressed
    property alias containsMouse: btnArea.containsMouse
    Text {
        id: btnText
        anchors.centerIn: parent
    }
    MouseArea {
        id: btnArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
