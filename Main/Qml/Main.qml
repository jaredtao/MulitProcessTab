import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "./Comp"
Item {
    id: root
    Rectangle {
        id: titleRect
        objectName: "titleRect"
        width: parent.width
        height: 40
        color: "blue"
        CDragArea {
            anchors.fill: parent
            dragTarget: rootView
        }
        CButton {
            id: mainPageBtn
            text: "首页"
            width: 150
            anchors.left: parent.left
            color: pressed ? "red" :
                             (tabMgr.currentTab === "main" ? "green" :
                                                             (containsMouse ? "darkGray" : "white"))
            onClicked: {
                tabMgr.activeTab("main")
            }
        }
        ListView {
            id: tabListView
            height: parent.height
            anchors.left: mainPageBtn.right
            anchors.right: toolBtnRow.left
            model: tabMgr.tabList
            orientation: Qt.Horizontal
            interactive: false;
            delegate: CButton {
                height: tabListView.height
                width: tabListView.count > 4 ? tabListView.width / tabListView.count : 150
                color: pressed ? "red" : ( tabMgr.currentTab === modelData ? "green":
                                                          (containsMouse ? "darkGray" : "white"))
                text: modelData
                onClicked: {
                    tabMgr.activeTab(modelData)
                }
                CButton {
                    width: 8
                    height: 8
                    anchors.right: parent.right
                    anchors.top: parent.top
                    text: "X"
                    color: pressed ? "red" : "gray"
                    onClicked: {
                        tabMgr.closeTab(modelData)
                    }
                }
            }
        }
        Row {
            id: toolBtnRow
            anchors.right: parent.right
            CButton {
                text: "一"
                onClicked: {
                    tabMgr.showMinimized()
                }
            }
            CButton {
                text: "口"
                property bool isMaxed: false
                onClicked: {
                    if (isMaxed)
                    {
                        tabMgr.showNormal();
                    }
                    else
                    {
                        tabMgr.showMaximized();
                    }

                    isMaxed = ! isMaxed;
                }
            }
            CButton {
                text: "X"
                onClicked: {
                    Qt.quit()
                }
            }
        }

    }
    Rectangle {
        id: contentRect
        objectName: "contentRect"
        width: parent.width
        anchors.top: titleRect.bottom
        anchors.bottom: parent.bottom
        color: "gray"
        GridLayout {
            anchors.fill: parent
            anchors.margins: 20
            columns: 4
            rows: 4
            Repeater {
                model: 16
                CButton {
                    implicitWidth: 200
                    implicitHeight: 150
                    color: randColor()
                    text: index
                    onClicked: {
                        tabMgr.activeTab(index)
                    }
                }
            }
        }

    }
    function randColor() {
        return Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
    }
}
