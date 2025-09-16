import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ContextDrawer {
    id: drawer
    
    property int currentTaskId: -1
    property bool editMode: false
    
    PomodoroSettingsDialog {
        id: pomodoroSettingsDialog
        taskId: drawer.currentTaskId
        
        onSettingsChanged: function(target, length, shortBreak, longBreak) {
            // Update the timer with new settings
            pomodoroTimer.pomodoroTarget = target
            pomodoroTimer.pomodoroLength = length
            pomodoroTimer.shortBreakLength = shortBreak
            pomodoroTimer.longBreakLength = longBreak
        }
    }
    
    // Warning dialog for active pomodoro conflict
    Kirigami.Dialog {
        id: warningDialog
        title: "Pomodoro Timer Active"
        standardButtons: Dialog.Ok
        
        Label {
            text: "Another task already has an active pomodoro timer. Only one pomodoro can be active at a time."
            wrapMode: Text.WordWrap
        }
    }
    
    function loadTask(taskId) {
        currentTaskId = taskId
        editMode = false
        
        // Load task data from model
        for (let i = 0; i < todoModel.rowCount(); i++) {
            let index = todoModel.index(i, 0)
            let id = todoModel.data(index, 256) // IdRole
            if (id === taskId) {
                contentField.text = todoModel.data(index, 258) // ContentRole
                descriptionField.text = todoModel.data(index, 259) // DescriptionRole
                let dueDate = todoModel.data(index, 260) // DueDateRole
                if (dueDate && dueDate.getTime() > 0) {
                    dueDateField.checked = true
                    dueDatePicker.text = dueDate.getFullYear() + "-" + 
                                       String(dueDate.getMonth() + 1).padStart(2, '0') + "-" + 
                                       String(dueDate.getDate()).padStart(2, '0')
                } else {
                    dueDateField.checked = false
                    dueDatePicker.text = ""
                }
                priorityCombo.currentIndex = todoModel.data(index, 261) - 1 // PriorityRole (1-based to 0-based)
                completedCheckBox.checked = todoModel.data(index, 262) // IsCompletedRole
                pomodoroEnabledSwitch.checked = todoModel.data(index, 282) || false // PomodoroEnabledRole
                break
            }
        }
    }
    
    function saveTask() {
        if (currentTaskId === -1 || contentField.text.trim().length === 0) return
        
        let dueDate = dueDateField.checked ? new Date(dueDatePicker.text) : new Date(0)
        
        todoModel.updateTask(
            currentTaskId,
            contentField.text.trim(),
            descriptionField.text.trim(),
            dueDate,
            priorityCombo.currentValue
        )
        
        editMode = false
    }
    
    title: editMode ? "Edit Task" : "Task Details"
    
    actions: [
        Kirigami.Action {
            text: editMode ? "Save" : "Edit"
            icon.name: editMode ? "document-save" : "document-edit"
            onTriggered: {
                if (editMode) {
                    saveTask()
                } else {
                    editMode = true
                }
            }
        },
        Kirigami.Action {
            text: "Delete"
            icon.name: "edit-delete"
            onTriggered: {
                todoModel.deleteTask(currentTaskId)
                drawer.close()
            }
        }
    ]
    
    ScrollView {
        anchors.fill: parent
        
        ColumnLayout {
            width: parent.width
            spacing: Kirigami.Units.largeSpacing
            
            // Completion status
            CheckBox {
                id: completedCheckBox
                Layout.fillWidth: true
                text: "Completed"
                enabled: !editMode
                
                onToggled: {
                    if (!editMode) {
                        todoModel.toggleTaskCompleted(currentTaskId)
                        drawer.close()
                    }
                }
            }
            
            Kirigami.Separator {
                Layout.fillWidth: true
            }
            
            // Task details form
            Kirigami.FormLayout {
                Layout.fillWidth: true
                enabled: editMode
                
                TextField {
                    id: contentField
                    Kirigami.FormData.label: "Task:"
                    readOnly: !editMode
                    background: editMode ? undefined : null
                }
                
                ScrollView {
                    Kirigami.FormData.label: "Description:"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    
                    TextArea {
                        id: descriptionField
                        readOnly: !editMode
                        wrapMode: TextArea.Wrap
                        background: editMode ? undefined : null
                    }
                }
                
                ComboBox {
                    id: priorityCombo
                    Kirigami.FormData.label: "Priority:"
                    enabled: editMode
                    
                    model: [
                        { text: "Low", value: 1 },
                        { text: "Medium", value: 2 },
                        { text: "Medium-High", value: 3 },
                        { text: "High", value: 4 }
                    ]
                    
                    textRole: "text"
                    valueRole: "value"
                }
                
                CheckBox {
                    id: dueDateField
                    Kirigami.FormData.label: "Due Date:"
                    text: "Set due date"
                    enabled: editMode
                }
                
                TextField {
                    id: dueDatePicker
                    visible: dueDateField.checked
                    Kirigami.FormData.label: ""
                    placeholderText: "YYYY-MM-DD"
                    inputMask: "9999-99-99"
                    readOnly: !editMode
                    background: editMode ? undefined : null
                }
            }
            
            Kirigami.Separator {
                Layout.fillWidth: true
                visible: !editMode
            }
            
            // Pomodoro Section
            ColumnLayout {
                Layout.fillWidth: true
                visible: !editMode
                spacing: Kirigami.Units.smallSpacing
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: "Pomodoro Timer"
                        font.bold: true
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Switch {
                        id: pomodoroEnabledSwitch
                        text: "Enable"
                        
                        onToggled: {
                            if (checked) {
                                // Check if another task already has an active pomodoro
                                let activeTaskId = todoModel.getActivePomodoroTaskId()
                                if (activeTaskId !== -1 && activeTaskId !== drawer.currentTaskId) {
                                    // Show warning and revert
                                    checked = false
                                    warningDialog.open()
                                    return
                                }
                            }
                            
                            todoModel.enablePomodoro(drawer.currentTaskId, checked)
                            if (!checked) {
                                // Also deactivate if disabling
                                todoModel.setActivePomodoroTask(-1)
                            }
                        }
                    }
                }
                
                // Timer controls and settings (only when enabled)
                ColumnLayout {
                    Layout.fillWidth: true
                    visible: pomodoroEnabledSwitch.checked
                    spacing: Kirigami.Units.smallSpacing
                    
                    RowLayout {
                        Layout.fillWidth: true
                        
                        Button {
                            text: "Settings"
                            icon.name: "configure"
                            onClicked: pomodoroSettingsDialog.open()
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            id: startStopButton
                            text: pomodoroTimer.isRunning ? "Stop Timer" : "Start Timer"
                            icon.name: pomodoroTimer.isRunning ? "media-playback-stop" : "media-playback-start"
                            
                            onClicked: {
                                if (pomodoroTimer.isRunning) {
                                    pomodoroTimer.reset()
                                    todoModel.setActivePomodoroTask(-1)
                                } else {
                                    // Check if another task is active
                                    let activeTaskId = todoModel.getActivePomodoroTaskId()
                                    if (activeTaskId !== -1 && activeTaskId !== drawer.currentTaskId) {
                                        warningDialog.open()
                                        return
                                    }
                                    
                                    todoModel.setActivePomodoroTask(drawer.currentTaskId)
                                    pomodoroTimer.startPomodoro()
                                }
                            }
                        }
                    }
                    
                    PomodoroTimer {
                        id: pomodoroTimer
                        Layout.fillWidth: true
                        Layout.preferredHeight: 350
                        taskId: drawer.currentTaskId
                        
                        Component.onCompleted: {
                            loadPomodoroSettings()
                        }
                        
                        function loadPomodoroSettings() {
                            // Load pomodoro settings from task
                            for (let i = 0; i < todoModel.rowCount(); i++) {
                                let index = todoModel.index(i, 0)
                                let id = todoModel.data(index, 256) // IdRole
                                if (id === drawer.currentTaskId) {
                                    pomodoroCount = todoModel.data(index, 277) || 0 // PomodoroCountRole
                                    pomodoroTarget = todoModel.data(index, 278) || 4 // PomodoroTargetRole
                                    pomodoroLength = todoModel.data(index, 279) || 25 // PomodoroLengthRole
                                    shortBreakLength = todoModel.data(index, 280) || 5 // ShortBreakLengthRole
                                    longBreakLength = todoModel.data(index, 281) || 15 // LongBreakLengthRole
                                    
                                    // Also update the settings dialog
                                    pomodoroSettingsDialog.pomodoroTarget = pomodoroTarget
                                    pomodoroSettingsDialog.pomodoroLength = pomodoroLength
                                    pomodoroSettingsDialog.shortBreakLength = shortBreakLength
                                    pomodoroSettingsDialog.longBreakLength = longBreakLength
                                    break
                                }
                            }
                        }
                        
                        onIsRunningChanged: {
                            // Update active state in database
                            if (isRunning) {
                                todoModel.setActivePomodoroTask(drawer.currentTaskId)
                            } else {
                                todoModel.setActivePomodoroTask(-1)
                            }
                        }
                    }
                }
            }
            
            // Cancel button when editing
            Button {
                Layout.fillWidth: true
                visible: editMode
                text: "Cancel"
                
                onClicked: {
                    editMode = false
                    loadTask(currentTaskId) // Reload original data
                }
            }
        }
    }
}