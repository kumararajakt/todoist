pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ScrollablePage {
    id: taskListViewPage

    signal taskSelected(var task)
    required property var todoModel
    required property var projectModel
    required property var addTaskDialog

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
            onTriggered: taskListViewPage.addTaskDialog.open()
        },
        Kirigami.Action {
            text: "Search"
            icon.name: "search"
            checkable: true
            onToggled: searchField.visible = checked
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
    }



    ListView {
        id: listView

        model: taskListViewPage.todoModel
        spacing: 1
        section.property: "isCompleted"
        section.delegate: Kirigami.ListSectionHeader {
            Layout.fillWidth: true
            text: (listView.section === 1) ? "COMPLETED" : "INPROGRESS"
            Component.onCompleted: {
                console.log("Section: ", listView.section);
            }
        }
        delegate: TaskItem {
            width: listView.width
            required property var model
            required property int index
            projectModel: taskListViewPage.projectModel
            
            // Time tracking properties
            totalTimeSpent: model.totalTimeSpent || 0
            timeTrackingActive: model.timeTrackingActive || false
            timeTrackingStarted: model.timeTrackingStarted || new Date()

            onClicked: {
                let selectedData = taskListViewPage.todoModel.get(index);
                taskListViewPage.taskSelected(JSON.parse(JSON.stringify(selectedData)));
            }

            onToggleCompleted: {
                taskListViewPage.todoModel.toggleTaskCompleted(model.id);
            }

            onDeleteTask: {
                taskListViewPage.todoModel.deleteTask(model.id);
            }
            
            onStartTimerRequested: {
                console.log("Timer start requested for task:", model.id);
                // The actual time tracking is handled by the Task object methods
                // This signal can be used for additional UI updates if needed
                taskListViewPage.todoModel.setData(taskListViewPage.todoModel.index(index, 0), true, taskListViewPage.todoModel.TimeTrackingStartedRole)

            }

            Component.onCompleted: {
                console.log("TaskItem completed", model.dueDate);
                taskListViewPage.checkForReminder(model.dueDate);
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

                onTriggered: taskListViewPage.addTaskDialog.open()
            }
        }
    }

    function checkForReminder(dueDate) {
        const now = new Date();
        const dueDateTime = new Date(dueDate);
        if (dueDateTime && dueDateTime < now) {
            systemTray.showMessage("Task Due", "Task is due today");
        }
    }
}
