import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: dialog
    
    property int parentTaskId: -1
    property string parentTaskContent: ""
    
    signal subtaskAdded()
    
    title: "Add Subtask"
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    width: Math.min(Kirigami.Units.gridUnit * 25, parent.width * 0.8)
    
    function reset() {
        contentField.text = "";
        descriptionField.text = "";
        priorityCombo.currentIndex = 0;
        dueDateField.checked = false;
        dueDatePicker.text = "";
    }
    
    onAccepted: {
        if (contentField.text.trim().length > 0) {
            var dueDate = dueDateField.checked ? new Date(dueDatePicker.text) : new Date(0);
            
            if (todoModel.addSubtask(
                parentTaskId,
                contentField.text.trim(),
                descriptionField.text.trim(),
                dueDate,
                priorityCombo.currentValue
            )) {
                subtaskAdded();
                reset();
            }
        }
    }
    
    onRejected: {
        reset();
    }
    
    onOpened: {
        contentField.forceActiveFocus();
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing
        
        Label {
            Layout.fillWidth: true
            text: "Adding subtask to: " + parentTaskContent
            opacity: 0.7
            wrapMode: Text.WordWrap
        }
        
        Kirigami.FormLayout {
            Layout.fillWidth: true
            
            TextField {
                id: contentField
                Kirigami.FormData.label: "Subtask:"
                placeholderText: "Enter subtask description..."
            }
            
            ScrollView {
                Kirigami.FormData.label: "Description:"
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                
                TextArea {
                    id: descriptionField
                    placeholderText: "Optional description..."
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
            }
        }
    }
}