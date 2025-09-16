import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    
    property var task: null
    property bool compact: false
    
    signal timeTrackingStarted()
    signal timeTrackingStopped()
    signal timeTrackingPaused()
    
    implicitHeight: compact ? 40 : 60
    implicitWidth: compact ? 200 : 300
    
    Rectangle {
        anchors.fill: parent
        color: task && task.timeTrackingActive ? "#e8f5e8" : "#f8f9fa"
        border.color: task && task.timeTrackingActive ? "#28a745" : "#dee2e6"
        border.width: 1
        radius: 8
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: compact ? 6 : 12
            spacing: compact ? 8 : 12
            
            // Time display
            Column {
                Layout.fillWidth: true
                spacing: 2
                
                Text {
                    text: if (task) {
                        if (task.timeTrackingActive) {
                            task.formatTimeSpent(task.totalTimeSpent + task.getCurrentSessionTime())
                        } else {
                            task.formatTimeSpent(task.totalTimeSpent)
                        }
                    } else {
                        "0s"
                    }
                    font.pixelSize: compact ? 14 : 16
                    font.weight: Font.Medium
                    color: task && task.timeTrackingActive ? "#28a745" : "#495057"
                }
                
                Text {
                    visible: !compact && task && task.timeTrackingActive
                    text: "Session: " + (task ? task.formatTimeSpent(task.getCurrentSessionTime()) : "0s")
                    font.pixelSize: 11
                    color: "#6c757d"
                }
            }
            
            // Control buttons
            RowLayout {
                spacing: 4
                
                Button {
                    id: playPauseButton
                    Layout.preferredWidth: compact ? 30 : 36
                    Layout.preferredHeight: compact ? 30 : 36
                    
                    background: Rectangle {
                        color: parent.pressed ? "#218838" : (parent.hovered ? "#34ce57" : "#28a745")
                        radius: width / 2
                        border.color: "#1e7e34"
                        border.width: 1
                    }
                    
                    contentItem: Text {
                        text: task && task.timeTrackingActive ? "⏸" : "▶"
                        color: "white"
                        font.pixelSize: compact ? 12 : 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    enabled: task !== null
                    
                    onClicked: {
                        if (task) {
                            if (task.timeTrackingActive) {
                                task.pauseTimeTracking()
                                root.timeTrackingPaused()
                            } else {
                                task.startTimeTracking()
                                root.timeTrackingStarted()
                            }
                        }
                    }
                }
                
                Button {
                    id: stopButton
                    Layout.preferredWidth: compact ? 30 : 36
                    Layout.preferredHeight: compact ? 30 : 36
                    
                    background: Rectangle {
                        color: parent.pressed ? "#c82333" : (parent.hovered ? "#e0a800" : "#ffc107")
                        radius: width / 2
                        border.color: "#d39e00"
                        border.width: 1
                    }
                    
                    contentItem: Text {
                        text: "⏹"
                        color: "white"
                        font.pixelSize: compact ? 12 : 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    enabled: task !== null && task.timeTrackingActive
                    
                    onClicked: {
                        if (task) {
                            task.stopTimeTracking()
                            root.timeTrackingStopped()
                        }
                    }
                }
            }
        }
    }
    
    // Timer to update the display every second when active
    Timer {
        id: updateTimer
        interval: 1000
        repeat: true
        running: task && task.timeTrackingActive
        
        onTriggered: {
            // Force property binding update by accessing the properties
            if (task) {
                task.getCurrentSessionTime()
            }
        }
    }
}