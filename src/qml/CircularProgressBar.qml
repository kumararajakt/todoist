import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.kirigami 2.19 as Kirigami

Item {
    id: root
    
    property real progress: 0.0 // 0.0 to 1.0
    property int size: 24
    property color backgroundColor: Kirigami.Theme.disabledTextColor
    property color progressColor: Kirigami.Theme.highlightColor
    property int lineWidth: 3
    
    width: size
    height: size
    
    Canvas {
        id: canvas
        anchors.fill: parent
        
        onPaint: {
            var ctx = getContext("2d")
            var centerX = width / 2
            var centerY = height / 2
            var radius = (Math.min(width, height) / 2) - root.lineWidth
            
            ctx.clearRect(0, 0, width, height)
            
            // Background circle
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI)
            ctx.strokeStyle = root.backgroundColor
            ctx.lineWidth = root.lineWidth
            ctx.stroke()
            
            // Progress arc
            if (root.progress > 0) {
                ctx.beginPath()
                ctx.arc(centerX, centerY, radius, -Math.PI / 2, (-Math.PI / 2) + (2 * Math.PI * root.progress))
                ctx.strokeStyle = root.progressColor
                ctx.lineWidth = root.lineWidth
                ctx.lineCap = "round"
                ctx.stroke()
            }
        }
        
        Connections {
            target: root
            function onProgressChanged() { canvas.requestPaint() }
            function onBackgroundColorChanged() { canvas.requestPaint() }
            function onProgressColorChanged() { canvas.requestPaint() }
        }
    }
    
    // Timer icon in center
    Label {
        anchors.centerIn: parent
        text: "⏱"
        font.pointSize: root.size * 0.4
        color: root.progress > 0 ? root.progressColor : root.backgroundColor
    }
}