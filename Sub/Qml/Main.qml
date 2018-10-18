import QtQuick 2.0

Rectangle {
    color: Qt.rgba(Math.random(),Math.random(),Math.random(),Math.random())

    Rectangle{
        width: 100
        height: 100
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: Qt.rgba(Math.random(),Math.random(),Math.random(),Math.random())
    }
}
