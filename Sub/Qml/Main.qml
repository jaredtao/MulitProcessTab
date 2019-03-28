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
    Rectangle {
        id: center
        width: 100
        height: width
        color: Qt.rgba(Math.random(),Math.random(),Math.random(),Math.random())
        anchors.centerIn: parent
        RotationAnimation {
            from: 0
            to: 360
            duration: 1000
            target: center
            loops: Animation.Infinite
            running: true
        }
    }
}
