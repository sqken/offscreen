import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 800
    height: 600
    color: "white"

    // 中间显示图片
    Image {
        id: centerImage
        source: "assets_0/Component/park_in_space.png"
        anchors.centerIn: parent
        width: sourceSize.width
        height: sourceSize.height
    }

    // 右上角显示时间
    Text {
        id: timeDisplay
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20
        font.pixelSize: 24
        color: "black"
        
        // 定时更新时间文本
        Timer {
            interval: 1000
            running: true
            repeat: true
            triggeredOnStart: true
            onTriggered: {
                var date = new Date();
                timeDisplay.text = Qt.formatDateTime(date, "hh:mm:ss");
            }
        }
    }
}
