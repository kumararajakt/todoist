import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: dialog
    
    property int parentTaskId: -1
    property string parentTaskContent: ""
    
    title: "Subtasks for: " + parentTaskContent
    standardButtons: Dialog.Close
    
    width: Math.min(Kirigami.Units.gridUnit * 40, parent.width * 0.9)
    height: Math.min(Kirigami.Units.gridUnit * 30, parent.height * 0.8)
    
    function loadSubtasks() {
        if (parentTaskId > 0) {
            subtaskModel.clear();
            var subtasks = dbManager.getSubtasks(parentTaskId);
            for (var i = 0; i < subtasks.length; i++) {
                var subtask = subtasks[i];
                subtaskModel.append({
                    "id": subtask.id,
                    "content": subtask.content,
                    "description": subtask.description,
                    "isCompleted": subtask.isCompleted,
                    "priority": subtask.priority,
                    "dueDate": subtask.dueDate
                });
            }
        }
    }
    
    function addSubtask() {
        if (newSubtaskField.text.trim().length > 0) {
            if (dbManager.addSubtask(parentTaskId, newSubtaskField.text.trim())) {
                newSubtaskField.text = "";
                loadSubtasks();
                todoModel.refresh(); // Refresh main model to update counts
            }
        }
    }
    
    onOpened: {
        loadSubtasks();
        newSubtaskField.forceActiveFocus();
    }
    
    ListModel {
        id: subtaskModel
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing
        
        // Add new subtask section
        Kirigami.Card {
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                
                TextField {
                    id: newSubtaskField
                    Layout.fillWidth: true
                    placeholderText: "Add a new subtask..."
                    
                    onAccepted: addSubtask()
                }
                
                Button {
                    text: "Add"
                    icon.name: "list-add"
                    onClicked: addSubtask()
                }
            }
        }
        
        // Subtasks list
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: subtaskListView
                model: subtaskModel
                spacing: Kirigami.Units.smallSpacing
                
                delegate: Kirigami.Card {
                    width: subtaskListView.width
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Kirigami.Units.smallSpacing
                        
                        CheckBox {
                            id: completedCheckBox
                            checked: model.isCompleted
                            
                            onToggled: {
                                todoModel.toggleTaskCompleted(model.id);
                                loadSubtasks();
                                todoModel.refresh();
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: Kirigami.Units.smallSpacing
                            
                            Label {
                                Layout.fillWidth: true
                                text: model.content
                                font.strikeout: model.isCompleted
                                opacity: model.isCompleted ? 0.6 : 1.0
                                wrapMode: Text.WordWrap
                            }
                            
                            Label {
                                Layout.fillWidth: true
                                text: model.description
                                visible: text.length > 0
                                opacity: 0.7
                                font.pointSize: Kirigami.Theme.smallFont.pointSize
                                wrapMode: Text.WordWrap
                            }
                            
                            RowLayout {
                                visible: model.dueDate && model.dueDate.getTime() > 0
                                
                                Kirigami.Icon {
                                    source: "appointment-new"
                                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                                }
                                
                                Label {
                                    text: model.dueDate ? Qt.formatDateTime(model.dueDate, "MMM dd, yyyy") : ""
                                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                                    opacity: 0.7
                                }
                            }
                        }
                        
                        // Priority indicator
                        Rectangle {
                            Layout.preferredWidth: 4
                            Layout.preferredHeight: 30
                            color: {
                                switch (model.priority) {
                                case 4: return Kirigami.Theme.negativeTextColor; // High
                                case 3: return Kirigami.Theme.neutralTextColor;  // Medium-High
                                case 2: return Kirigami.Theme.positiveTextColor; // Medium
                                default: return Kirigami.Theme.disabledTextColor; // Low
                                }
                            }
                            radius: 2
                        }
                        
                        ToolButton {
                            icon.name: "edit-delete"
                            onClicked: {
                                todoModel.deleteTask(model.id);
                                loadSubtasks();
                                todoModel.refresh();
                            }
                        }
                    }
                }
                
                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    visible: subtaskListView.count === 0
                    text: "No subtasks yet"
                    helpfulAction: Kirigami.Action {
                        text: "Add Subtask"
                        icon.name: "list-add"
                        onTriggered: newSubtaskField.forceActiveFocus()
                    }
                }
            }
        }
    }
}