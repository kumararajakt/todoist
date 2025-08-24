import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ContextDrawer {
    id: drawer
    
    property int currentTaskId: -1
    property bool editMode: false
    
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