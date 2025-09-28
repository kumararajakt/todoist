pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ScrollablePage {
    id: taskListViewPage

    signal taskSelected(var task, bool isTimerRequested)
    required property var todoModel
    required property var projectModel
    required property var addTaskDialog

    property int currentTaskId: -1

    title: {
        if (todoModel.currentFilter) {
            switch (todoModel.currentFilter) {
            case "today":
                return "Today";
            case "upcoming":
                return "Upcoming";
            case "priority":
                return "Priority";
            case "completed":
                return "Completed";
            default:
                return "Tasks";
            }
        } else if (todoModel.currentProjectId > 0) {
            // Find project name
            for (let i = 0; i < projectModel.rowCount(); i++) {
                let project = projectModel.data(projectModel.index(i, 0), 257); // NameRole
                let projectId = projectModel.data(projectModel.index(i, 0), 256); // IdRole
                if (projectId === todoModel.currentProjectId) {
                    return project;
                }
            }
            return "Project";
        } else {
            return "All Tasks";
        }
    }

    actions: [
        Kirigami.Action {
            text: "Add Task"
            icon.name: "list-add"
            onTriggered: {
                addTaskField.visible = true;
                searchField.visible = checked;
            }
        },
        Kirigami.Action {
            text: "Search"
            icon.name: "search"
            checkable: true
            onToggled: {
                addTaskField.visible = false;
                searchField.visible = checked;
            }
        },
        Kirigami.Action {
            text: "Show Completed"
            icon.name: "filter"
            checkable: true
            checked: {
                if (taskListViewPage.todoModel.currentFilter === "completed") {
                    return true;
                } else {
                    return false;
                }
            }
            onTriggered: {
                if (checked) {
                    taskListViewPage.todoModel.currentFilter = "completed";
                } else {
                    taskListViewPage.todoModel.currentFilter = "";
                }
            }
        }
    ]

    header: ColumnLayout {
        width: parent.width
        spacing: 0

        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
            visible: false

            placeholderText: "Search tasks..."

            onTextChanged: {
                taskListViewPage.todoModel.searchText = text;
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: searchField.visible
        }

        Kirigami.ActionTextField {
            id: addTaskField
            placeholderText: "Add task..."
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing

            onAccepted: {
                if (text.trim() !== "") {
                    taskListViewPage.todoModel.addTask({
                        "content": text
                    });
                    addTaskField.visible = false;
                }
            }
            visible: false
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: addTaskField.visible
        }
    }

    ListView {
        id: listView
        Layout.fillHeight: true
        Layout.fillWidth: true

        model: taskListViewPage.todoModel
        spacing: 1

        delegate: TaskItem {
            width: listView.width
            required property var model
            required property int index
            projectModel: taskListViewPage.projectModel

            // Time tracking properties
            // totalTimeSpent: model.totalTimeSpent || 0
            // timeTrackingActive: model.timeTrackingActive || false
            // timeTrackingStarted: model.timeTrackingStarted || new Date()

            onClicked: {
                let selectedData = taskListViewPage.todoModel.get(index);
                taskListViewPage.taskSelected(JSON.parse(JSON.stringify(selectedData)), false);
            }

            onToggleCompleted: {
                taskListViewPage.todoModel.toggleTaskCompleted(model.id);
            }

            onDeleteTask: {
                taskListViewPage.currentTaskId = model.id;
                deleteConfirmationDialog.open();
            }

            onStartTimerRequested: {
                let selectedData = taskListViewPage.todoModel.get(index);
                taskListViewPage.taskSelected(JSON.parse(JSON.stringify(selectedData)), true);
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count === 0 && taskListViewPage.todoModel.currentFilter !== 'completed'

            text: {
                if (taskListViewPage.todoModel.searchText) {
                    return "No tasks found matching '" + taskListViewPage.todoModel.searchText + "'";
                } else if (taskListViewPage.todoModel.currentFilter === "completed") {
                    return "No completed tasks";
                } else if (taskListViewPage.todoModel.currentFilter === "today") {
                    return "No tasks due today";
                } else if (taskListViewPage.todoModel.currentFilter === "upcoming") {
                    return "No upcoming tasks";
                } else if (taskListViewPage.todoModel.currentFilter === "priority") {
                    return "No high priority tasks";
                } else {
                    return "No tasks yet";
                }
            }

            helpfulAction: Kirigami.Action {
                text: "Add Task"
                icon.name: "list-add"

            }
        }
    }

    // Delete confirmation dialog
    Kirigami.PromptDialog {
        id: deleteConfirmationDialog
        title: "Delete Task"
        subtitle: "Are you sure you want to delete this task?"
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        onAccepted: {
            taskListViewPage.todoModel.deleteTask(taskListViewPage.currentTaskId);
        }
    }
}
