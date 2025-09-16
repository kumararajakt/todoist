import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.todoist 1.0

Kirigami.Dialog {
    id: dialog
    
    signal taskAdded()
    
    title: "Add New Task"
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    onAccepted: {
        if (contentField.text.trim().length > 0) {
            let dueDate = dueDateField.checked ? new Date(dueDatePicker.text) : new Date()
            if (!dueDateField.checked) {
                dueDate = new Date(0) // Invalid date to represent no due date
            }
            
            let startDate = repeatSettings.isRecurring ? repeatSettings.repeatStartDate : new Date(0);
            let endDate = repeatSettings.repeatEndType === "on" ? repeatSettings.repeatEndDate : new Date(0);
            
            TodoModel.addTask(
                contentField.text.trim(),
                descriptionField.text.trim(),
                dueDate,
                priorityCombo.currentValue,
                repeatSettings.isRecurring,
                repeatSettings.repeatInterval,
                repeatSettings.repeatFrequency,
                startDate,
                repeatSettings.repeatEndType,
                endDate,
                repeatSettings.repeatEndCount
            )
            
            taskAdded()
            clearFields()
        }
    }
    
    onRejected: {
        clearFields()
    }
    
    function clearFields() {
        contentField.text = ""
        descriptionField.text = ""
        dueDateField.checked = false
        priorityCombo.currentIndex = 0
        repeatSettings.reset()
    }
    
    ColumnLayout {
        width: Math.min(400, parent.width)
        spacing: Kirigami.Units.largeSpacing
        
        // Task content
        Kirigami.FormLayout {
            Layout.fillWidth: true
            
            TextField {
                id: contentField
                Kirigami.FormData.label: "Task:"
                placeholderText: "What needs to be done?"
                focus: true
            }
            
            ScrollView {
                id: descriptionScroll
                Kirigami.FormData.label: "Description:"
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                
                TextArea {
                    id: descriptionField
                    placeholderText: "Add details..."
                    wrapMode: TextArea.Wrap
                }
            }
            
            ComboBox {
                id: priorityCombo
                Kirigami.FormData.label: "Priority:"
                
                model: [
                    { text: "Low", value: 1 },
                    { text: "Medium", value: 2 },
                    { text: "Medium-High", value: 3 },
                    { text: "High", value: 4 }
                ]
                
                textRole: "text"
                valueRole: "value"
                currentIndex: 0
            }
            
            CheckBox {
                id: dueDateField
                Kirigami.FormData.label: "Due Date:"
                text: "Set due date"
            }
            
            TextField {
                id: dueDatePicker
                visible: dueDateField.checked
                Kirigami.FormData.label: ""
                placeholderText: "YYYY-MM-DD"
                inputMask: "9999-99-99"
                
                // Set default to today
                Component.onCompleted: {
                    let today = new Date()
                    text = today.getFullYear() + "-" + 
                           String(today.getMonth() + 1).padStart(2, '0') + "-" + 
                           String(today.getDate()).padStart(2, '0')
                }
            }
        }
        
        // Repeat settings
        RepeatSettingsCard {
            id: repeatSettings
            Layout.fillWidth: true
        }
    }
}