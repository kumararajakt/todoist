import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: dialog
    
    property int taskId: -1
    property int pomodoroTarget: 4
    property int pomodoroLength: 25
    property int shortBreakLength: 5
    property int longBreakLength: 15
    
    signal settingsChanged(int target, int length, int shortBreak, int longBreak)
    
    title: "Pomodoro Settings"
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    onAccepted: {
        settingsChanged(targetSpinBox.value, lengthSpinBox.value, shortBreakSpinBox.value, longBreakSpinBox.value)
        if (taskId !== -1) {
            todoModel.updatePomodoroSettings(taskId, targetSpinBox.value, lengthSpinBox.value, shortBreakSpinBox.value, longBreakSpinBox.value)
        }
    }
    
    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        
        Kirigami.FormLayout {
            Layout.fillWidth: true
            
            SpinBox {
                id: targetSpinBox
                Kirigami.FormData.label: "Target Pomodoros:"
                from: 1
                to: 12
                value: dialog.pomodoroTarget
                
                ToolTip.visible: hovered
                ToolTip.text: "Number of pomodoro sessions to complete for this task"
            }
            
            SpinBox {
                id: lengthSpinBox
                Kirigami.FormData.label: "Pomodoro Length (min):"
                from: 15
                to: 60
                stepSize: 5
                value: dialog.pomodoroLength
                
                ToolTip.visible: hovered
                ToolTip.text: "Duration of each focus session in minutes"
            }
            
            SpinBox {
                id: shortBreakSpinBox
                Kirigami.FormData.label: "Short Break (min):"
                from: 3
                to: 15
                value: dialog.shortBreakLength
                
                ToolTip.visible: hovered
                ToolTip.text: "Duration of short breaks between pomodoros"
            }
            
            SpinBox {
                id: longBreakSpinBox
                Kirigami.FormData.label: "Long Break (min):"
                from: 10
                to: 30
                stepSize: 5
                value: dialog.longBreakLength
                
                ToolTip.visible: hovered
                ToolTip.text: "Duration of long break after 4 pomodoros"
            }
        }
        
        Label {
            Layout.fillWidth: true
            text: "The Pomodoro Technique uses 25-minute focused work sessions followed by short breaks. After 4 sessions, take a longer break."
            wrapMode: Text.WordWrap
            font.pointSize: Kirigami.Theme.smallFont.pointSize
            color: Kirigami.Theme.disabledTextColor
        }
    }
}