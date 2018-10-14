import QtQuick 2.9

MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    property real lastX: 0
    property real lastY: 0
    property var dragTarget
    onContainsMouseChanged: {
        if (containsMouse) {
            cursorShape = Qt.OpenHandCursor
        } else {
            cursorShape = Qt.ArrowCursor
        }
    }
    onPressedChanged: {
        if (containsPress) {
            lastX = mouseX;
            lastY = mouseY;
        }
    }
    onPositionChanged: {
        if (pressed && dragTarget) {
            dragTarget.x += mouseX - lastX
            dragTarget.y += mouseY - lastY
        }
    }
}
