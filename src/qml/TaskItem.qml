import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

ItemDelegate {
    id: taskItem

    signal toggleCompleted
    signal deleteTask

    property bool isOverdue: model.dueDate.getTime() > 0 && model.dueDate < new Date() && !model.isCompleted
    property bool isDueToday: {
        if (!model.dueDate || model.dueDate.getTime() === 0)
            return false;
        let today = new Date();
        let due = new Date(model.dueDate);
        return due.toDateString() === today.toDateString();
    }
    
    // Subtask dialog
    SubtaskDialog {
        id: subtaskDialog
        parentTaskId: model.id
        parentTaskContent: model.content
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        // Completion checkbox
        CheckBox {
            id: completionCheckBox
            checked: model.isCompleted

            onToggled: {
                taskItem.toggleCompleted();
            }
        }

        // Task content
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            // Task title
            Label {
                Layout.fillWidth: true
                text: model.content
                font.strikeout: model.isCompleted
                opacity: model.isCompleted ? 0.6 : 1.0
                wrapMode: Text.WordWrap

                color: {
                    if (model.isCompleted)
                        return Kirigami.Theme.disabledTextColor;
                    if (taskItem.isOverdue)
                        return Kirigami.Theme.negativeTextColor;
                    return Kirigami.Theme.textColor;
                }
            }

            // Task description (if exists)
            Label {
                Layout.fillWidth: true
                visible: model.description && model.description.length > 0
                text: model.description
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                opacity: 0.7
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            // Labels and metadata row
            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                // Labels
                Repeater {
                    model: taskItem.model.labels

                    delegate: Rectangle {
                        height: Kirigami.Units.gridUnit
                        width: labelText.width + Kirigami.Units.smallSpacing * 2
                        radius: height / 2
                        color: Kirigami.Theme.backgroundColor
                        border.color: Kirigami.Theme.separatorColor
                        border.width: 1

                        Label {
                            id: labelText
                            anchors.centerIn: parent
                            text: modelData
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                        }
                    }
                }
                
                // Subtask indicator
                Rectangle {
                    visible: model.hasSubtasks
                    height: Kirigami.Units.gridUnit
                    width: subtaskLabel.width + Kirigami.Units.smallSpacing * 2
                    radius: height / 2
                    color: Kirigami.Theme.highlightColor
                    opacity: 0.8
                    
                    Label {
                        id: subtaskLabel
                        anchors.centerIn: parent
                        text: model.completedSubtaskCount + "/" + model.subtaskCount + " subtasks"
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        color: "white"
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: subtaskDialog.open()
                        cursorShape: Qt.PointingHandCursor
                    }
                }

                Item {
                    Layout.fillWidth: true
                } // Spacer

                // Due date
                Label {
                    visible: model.dueDate && model.dueDate.getTime() > 0
                    text: {
                        if (!model.dueDate || model.dueDate.getTime() === 0)
                            return "";

                        let due = new Date(model.dueDate);
                        let today = new Date();
                        let tomorrow = new Date(today);
                        tomorrow.setDate(today.getDate() + 1);

                        if (due.toDateString() === today.toDateString()) {
                            return "Today";
                        } else if (due.toDateString() === tomorrow.toDateString()) {
                            return "Tomorrow";
                        } else {
                            return due.toLocaleDateString();
                        }
                    }
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    color: {
                        if (taskItem.isOverdue)
                            return Kirigami.Theme.negativeTextColor;
                        if (taskItem.isDueToday)
                            return Kirigami.Theme.neutralTextColor;
                        return Kirigami.Theme.disabledTextColor;
                    }
                }

                // Recurring indicator
                Kirigami.Icon {
                    visible: model.isRecurring
                    source: "view-refresh"
                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    color: Kirigami.Theme.highlightColor
                    
                    ToolTip.visible: recurringMouseArea.containsMouse
                    ToolTip.text: "Recurring task: every " + model.repeatFrequency + " " + model.repeatInterval
                    
                    MouseArea {
                        id: recurringMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                // Priority indicator
                Rectangle {
                    visible: model.priority >= 3
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    radius: width / 2
                    color: {
                        switch (model.priority) {
                        case 4:
                            return Kirigami.Theme.negativeTextColor; // High
                        case 3:
                            return Kirigami.Theme.neutralTextColor; // Medium-High
                        default:
                            return Kirigami.Theme.disabledTextColor;
                        }
                    }

                    Kirigami.Icon {
                        anchors.centerIn: parent
                        width: parent.width * 0.6
                        height: parent.height * 0.6
                        source: "emblem-important"
                        color: "white"
                    }
                }
            }
        }

        // Actions
        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            ToolButton {
                icon.name: "view-list-tree"
                flat: true
                
                ToolTip.text: "Manage subtasks"

                onClicked: {
                    subtaskDialog.open();
                }
            }

            ToolButton {
                icon.name: "document-edit"
                flat: true
                
                ToolTip.text: "Edit task"

                onClicked: {
                    taskItem.clicked();
                }
            }

            ToolButton {
                icon.name: "edit-delete"
                flat: true
                
                ToolTip.text: "Delete task"

                onClicked: {
                    taskItem.deleteTask();
                }
            }
        }
    }
}
