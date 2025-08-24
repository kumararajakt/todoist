import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ScrollablePage {
    id: page

    signal taskSelected(var task)

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
            onTriggered: addTaskDialog.open()
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
                todoModel.searchText = text;
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: searchField.visible
        }
    }

    ListView {
        id: listView

        model: todoModel
        spacing: 1

        delegate: TaskItem {
            width: listView.width

            onClicked: {
                console.log("Task clicked:", index);

                let selectedData = todoModel.get(index);
                console.log("Task clicked:", (JSON.stringify(selectedData)), index);
                page.taskSelected(JSON.parse(JSON.stringify(selectedData)));
            }

            onToggleCompleted: {
                todoModel.toggleTaskCompleted(model.id);
            }

            onDeleteTask: {
                todoModel.deleteTask(model.id);
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count === 0 && todoModel.currentFilter !== 'completed'

            text: {
                if (todoModel.searchText) {
                    return "No tasks found matching '" + todoModel.searchText + "'";
                } else if (todoModel.currentFilter === "completed") {
                    return "No completed tasks";
                } else if (todoModel.currentFilter === "today") {
                    return "No tasks due today";
                } else if (todoModel.currentFilter === "upcoming") {
                    return "No upcoming tasks";
                } else if (todoModel.currentFilter === "priority") {
                    return "No high priority tasks";
                } else {
                    return "No tasks yet";
                }
            }

            helpfulAction: Kirigami.Action {
                text: "Add Task"
                icon.name: "list-add"

                onTriggered: addTaskDialog.open()
            }
        }
    }
}
