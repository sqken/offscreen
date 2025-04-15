import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 800
    height: 600
    color: "white"

    // 添加一个测试矩形，确认渲染正常
    Rectangle {
        id: testRect
        width: 200
        height: 200
        color: "red"
        anchors.centerIn: parent
    }

    // 中间显示图片 - 使用完整路径
    Image {
        id: centerImage
        source: "qrc:/assets_0/Component/safe_belt.png"
        anchors.centerIn: parent
        visible: true
        
        // 添加调试信息
        onStatusChanged: {
            if (status === Image.Ready) {
                console.log("图片加载成功:", source)
                console.log("图片尺寸:", sourceSize.width, "x", sourceSize.height)
            } else if (status === Image.Error) {
                console.log("图片加载失败:", source)
            } else if (status === Image.Loading) {
                console.log("图片正在加载:", source)
            }
        }
        
        // 控制图片闪烁的计时器
        Timer {
            id: blinkTimer
            interval: 500
            running: true
            repeat: true
            onTriggered: {
                centerImage.visible = !centerImage.visible
                console.log("图片可见性切换:", centerImage.visible)
                
                // 触发场景变动信号
                root.contentChanged()
            }
        }
    }

    // 右上角显示时间
    Rectangle {
        id: timeBackground
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        width: 120
        height: 40
        color: "lightblue"
        
        Text {
            id: timeDisplay
            anchors.centerIn: parent
            font.pixelSize: 24
            color: "black"
            
            // 确保时间立即显示
            text: {
                var date = new Date();
                return Qt.formatDateTime(date, "hh:mm:ss");
            }
            
            // 定时更新时间文本
            Timer {
                interval: 1000
                running: true
                repeat: true
                triggeredOnStart: true
                onTriggered: {
                    var date = new Date();
                    timeDisplay.text = Qt.formatDateTime(date, "hh:mm:ss");
                    console.log("时间更新:", timeDisplay.text);
                }
            }
        }
    }
    
    // 添加组件加载完成信号
    Component.onCompleted: {
        console.log("QML组件加载完成")
    }
    
    // 添加内容变动信号，用于通知C++保存图片
    signal contentChanged()
}
