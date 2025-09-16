pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

Kirigami.ScrollablePage {
    id: taskDetailPage

    property int currentTaskId: -1
    property bool isEdited: false
    property var taskData: null
    required property var todoModel

    title: taskData ? taskData.content : "Task Details"

    actions: [
        Kirigami.Action {
            text: "Save"
            icon.name: "document-save"
            visible: taskDetailPage.isEdited
            onTriggered: {
                taskDetailPage.saveTask();
                taskDetailPage.isEdited = false;
            }
        },
        Kirigami.Action {
            text: "Delete"
            icon.name: "delete"
            onTriggered: {
                deleteConfirmationDialog.open();
            }
        }
    ]

    // Add subtask dialog
    SubtaskDialog {
        id: addSubtaskDialog
        parentTaskId: taskDetailPage.currentTaskId
        parentTaskContent: taskDetailPage.taskData ? taskDetailPage.taskData.content : ""
    }

    // Delete confirmation dialog
    Kirigami.PromptDialog {
        id: deleteConfirmationDialog
        title: "Delete Task"
        subtitle: "Are you sure you want to delete this task?"
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        onAccepted: {
            taskDetailPage.deleteTask();
            applicationWindow().pageStack.pop();
        }
    }

    function loadTask(task) {
        taskData = task;
        currentTaskId = task.id;
    }

    function saveTask() {
        if (currentTaskId === -1 || !taskData)
            return;

        console.log('Saving task: ', JSON.stringify(taskData));
        const updated = todoModel.updateTask(currentTaskId, taskData);
        console.log('Task updated: ', updated);
        if (updated) {
            inlineMessage.text = "Task updated successfully";
            inlineMessage.visible = true;
            inlineMessage.type = Kirigami.MessageType.Positive;
            inlineMessage.duration = Kirigami.Units.gridUnit * 2;
        } else {
            inlineMessage.text = "Failed to update task";
            inlineMessage.visible = true;
            inlineMessage.type = Kirigami.MessageType.Error;
            inlineMessage.duration = Kirigami.Units.gridUnit * 2;
        }
    }

    function deleteTask() {
        if (currentTaskId === -1)
            return;

        todoModel.deleteTask(currentTaskId);
    }

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Kirigami.InlineMessage {
            id: inlineMessage
            Layout.fillWidth: true
            visible: false
        }

        // Task content
        FormCard.FormCard {
            Layout.fillWidth: true

            FormCard.FormTextFieldDelegate {
                id: contentField
                label: "Task:"
                text: taskDetailPage.taskData ? taskDetailPage.taskData.content : ""

                onTextChanged: {
                    taskDetailPage.isEdited = true;
                    if (taskDetailPage.taskData) {
                        taskDetailPage.taskData.content = text;
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            FormCard.FormTextAreaDelegate {
                label: "Description"
                text: taskDetailPage.taskData ? (taskDetailPage.taskData.description || "") : ""
                onTextChanged: {
                    taskDetailPage.isEdited = true;
                    if (taskDetailPage.taskData) {
                        taskDetailPage.taskData.description = text;
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            FormCard.AbstractFormDelegate {

                contentItem: ColumnLayout {

                    Label {
                        text: "Due Date"
                        Layout.fillWidth: true
                    }

                    FormCard.FormDateTimeDelegate {
                        dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Date
                        value: taskDetailPage.taskData ? taskDetailPage.taskData.due_date : ""
                        onValueChanged: {
                            taskDetailPage.isEdited = true;
                            if (taskDetailPage.taskData) {
                                taskDetailPage.taskData.due_date = value;
                                console.log('Due date changed to: ', value, taskDetailPage.taskData);
                            }
                        }
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            // Priority
            FormCard.AbstractFormDelegate {
                contentItem: ColumnLayout {
                    Label {
                        text: "Priority:"
                    }
                    ComboBox {
                        id: priorityCombo
                        model: ["None", "Low", "Medium", "High"]
                        currentIndex: taskDetailPage.taskData ? (taskDetailPage.taskData.priority || 0) : 0
                        onCurrentIndexChanged: {
                            taskDetailPage.isEdited = true;
                            if (taskDetailPage.taskData) {
                                taskDetailPage.taskData.priority = currentIndex;
                            }
                        }
                    }
                }
            }
        }

        // Subtasks section
        // Kirigami.Card {
        //     Layout.fillWidth: true
        //     visible: taskData && taskData.subtaskCount > 0

        //     header: RowLayout {
        //         Kirigami.Heading {
        //             text: "Subtasks (" + (taskData ? taskData.subtaskCount : 0) + ")"
        //             level: 3
        //         }

        //         Item {
        //             Layout.fillWidth: true
        //         }

        //         Button {
        //             text: "Add Subtask"
        //             icon.name: "list-add"
        //             onClicked: addSubtaskDialog.open()
        //         }
        //     }

        //     contentItem: Label {
        //         text: "Subtasks will be displayed here"
        //         color: Kirigami.Theme.disabledTextColor
        //     }
        // }

        // Add subtask button (when no subtasks exist)
        // Button {
        //     Layout.fillWidth: true
        //     text: "Add Subtask"
        //     icon.name: "list-add"
        //     visible: !taskData || taskData.subtaskCount === 0
        //     onClicked: addSubtaskDialog.open()
        // }

        // // Task metadata
        // Kirigami.Card {
        //     Layout.fillWidth: true

        //     header: Kirigami.Heading {
        //         text: "Details"
        //         level: 3
        //     }

        //     contentItem: Kirigami.FormLayout {
        //         Label {
        //             Kirigami.FormData.label: "Created:"
        //             text: taskData ? new Date(taskData.createdAt).toLocaleString() : ""
        //         }

        //         Label {
        //             Kirigami.FormData.label: "Updated:"
        //             text: taskData ? new Date(taskData.updatedAt).toLocaleString() : ""
        //         }

        //         Label {
        //             Kirigami.FormData.label: "Status:"
        //             text: taskData ? (taskData.isCompleted ? "Completed" : "In Progress") : ""
        //         }
        //     }
        // }
    }

    Component.onCompleted: {
        console.log('TaskDetailPage completed', taskData);
    }
}
