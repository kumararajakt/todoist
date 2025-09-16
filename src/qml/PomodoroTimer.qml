import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Item {
    id: root
    
    property int taskId: -1
    property int pomodoroLength: 25 // minutes
    property int shortBreakLength: 5 // minutes
    property int longBreakLength: 15 // minutes
    property int pomodoroCount: 0
    property int pomodoroTarget: 4
    
    property bool isRunning: false
    property bool isBreak: false
    property int timeRemaining: 0 // seconds
    property int currentSession: 1
    
    signal pomodoroCompleted()
    signal breakCompleted()
    signal sessionCompleted()
    
    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: root.isRunning
        
        onTriggered: {
            if (root.timeRemaining > 0) {
                root.timeRemaining--
                // Update progress for global manager
                let totalTime = root.isBreak ? 
                    ((root.pomodoroCount % 4 === 0) && root.pomodoroCount > 0 ? root.longBreakLength : root.shortBreakLength) * 60 :
                    root.pomodoroLength * 60
                let progress = (totalTime - root.timeRemaining) / totalTime
                if (typeof pomodoroManager !== 'undefined') {
                    pomodoroManager.updateProgress(progress)
                }
            } else {
                root.isRunning = false
                if (typeof pomodoroManager !== 'undefined') {
                    pomodoroManager.stopTimer()
                }
                if (root.isBreak) {
                    root.breakCompleted()
                } else {
                    root.pomodoroCompleted()
                    root.pomodoroCount++
                    // Update task in database
                    if (root.taskId !== -1) {
                        todoModel.updatePomodoroCount(root.taskId, root.pomodoroCount)
                    }
                }
            }
        }
    }
    
    function startPomodoro() {
        root.isBreak = false
        root.timeRemaining = root.pomodoroLength * 60
        root.isRunning = true
        if (typeof pomodoroManager !== 'undefined') {
            pomodoroManager.startTimer(root.taskId)
        }
    }
    
    function startBreak() {
        root.isBreak = true
        let isLongBreak = (root.pomodoroCount % 4 === 0) && root.pomodoroCount > 0
        root.timeRemaining = (isLongBreak ? root.longBreakLength : root.shortBreakLength) * 60
        root.isRunning = true
    }
    
    function pause() {
        root.isRunning = false
    }
    
    function resume() {
        root.isRunning = true
    }
    
    function reset() {
        root.isRunning = false
        root.isBreak = false
        root.timeRemaining = root.pomodoroLength * 60
        if (typeof pomodoroManager !== 'undefined') {
            pomodoroManager.stopTimer()
        }
    }
    
    function formatTime(seconds) {
        let minutes = Math.floor(seconds / 60)
        let secs = seconds % 60
        return minutes.toString().padStart(2, '0') + ":" + secs.toString().padStart(2, '0')
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing
        
        // Timer display
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            color: root.isBreak ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.activeBackgroundColor
            radius: Kirigami.Units.cornerRadius
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: Kirigami.Units.smallSpacing
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: root.isBreak ? "Break Time" : "Focus Time"
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                    color: Kirigami.Theme.textColor
                }
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: root.formatTime(root.timeRemaining)
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2.5
                    font.bold: true
                    color: Kirigami.Theme.textColor
                }
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Session " + root.currentSession + " of " + root.pomodoroTarget
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
                    color: Kirigami.Theme.disabledTextColor
                }
            }
        }
        
        // Progress indicator
        ProgressBar {
            Layout.fillWidth: true
            value: {
                let totalTime = root.isBreak ? 
                    ((root.pomodoroCount % 4 === 0) && root.pomodoroCount > 0 ? root.longBreakLength : root.shortBreakLength) * 60 :
                    root.pomodoroLength * 60
                return (totalTime - root.timeRemaining) / totalTime
            }
        }
        
        // Pomodoro counter
        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.smallSpacing
            
            Repeater {
                model: root.pomodoroTarget
                
                Rectangle {
                    width: 20
                    height: 20
                    radius: 10
                    color: index < root.pomodoroCount ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                    border.color: Kirigami.Theme.textColor
                    border.width: 1
                    
                    Label {
                        anchors.centerIn: parent
                        text: "🍅"
                        font.pointSize: 8
                        visible: index < root.pomodoroCount
                    }
                }
            }
        }
        
        // Control buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing
            
            Button {
                Layout.fillWidth: true
                text: root.isRunning ? "Pause" : (root.timeRemaining === (root.isBreak ? 
                    ((root.pomodoroCount % 4 === 0) && root.pomodoroCount > 0 ? root.longBreakLength : root.shortBreakLength) * 60 :
                    root.pomodoroLength * 60) ? "Start" : "Resume")
                icon.name: root.isRunning ? "media-playback-pause" : "media-playback-start"
                
                onClicked: {
                    if (root.isRunning) {
                        root.pause()
                    } else {
                        if (root.timeRemaining === (root.isBreak ? 
                            ((root.pomodoroCount % 4 === 0) && root.pomodoroCount > 0 ? root.longBreakLength : root.shortBreakLength) * 60 :
                            root.pomodoroLength * 60)) {
                            if (root.isBreak) {
                                root.startBreak()
                            } else {
                                root.startPomodoro()
                            }
                        } else {
                            root.resume()
                        }
                    }
                }
            }
            
            Button {
                Layout.fillWidth: true
                text: "Reset"
                icon.name: "view-refresh"
                
                onClicked: root.reset()
            }
        }
        
        // Quick actions
        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing
            
            Button {
                Layout.fillWidth: true
                text: "Start Break"
                enabled: !root.isRunning && !root.isBreak
                
                onClicked: root.startBreak()
            }
            
            Button {
                Layout.fillWidth: true
                text: "Skip Break"
                enabled: root.isBreak
                
                onClicked: {
                    root.reset()
                    root.currentSession++
                }
            }
        }
    }
    
    // Initialize timer
    Component.onCompleted: {
        root.reset()
    }
    
    // Handle completed pomodoros and breaks
    onPomodoroCompleted: {
        // Show notification or play sound
        if (root.pomodoroCount >= root.pomodoroTarget) {
            root.sessionCompleted()
        } else {
            // Auto-start break
            root.startBreak()
        }
    }
    
    onBreakCompleted: {
        root.currentSession++
        root.reset()
    }
    
    onSessionCompleted: {
        // All pomodoros completed for this task
        root.reset()
        root.currentSession = 1
    }
}