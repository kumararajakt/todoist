pragma ComponentBehavior: Bound

import org.kde.kirigami as Kirigami
import Qt.labs.platform

Kirigami.ApplicationWindow {
    id: root

    title: "Todoist"
    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    property int currentProjectId: -1
    property string currentFilter: ""

    readonly property int sidebarWidth: Kirigami.Units.gridUnit * 14

    globalDrawer: ProjectSidebar {
        id: sidebar
        width: root.sidebarWidth
        onProjectSelected: function (projectId) {
            root.currentProjectId = projectId;
            todoModel.currentProjectId = projectId;
            todoModel.currentFilter = "";
            root.applicationWindow().pageStack.clear();
            root.applicationWindow().pageStack.push(taskListComponent);
        }

        onFilterSelected: function (filter) {
            root.currentFilter = filter;
            todoModel.currentFilter = filter;
            todoModel.currentProjectId = -1;
            root.applicationWindow().pageStack.clear();
            root.applicationWindow().pageStack.push(taskListComponent);
        }
    }

    TaskDetailDialog {
        id: taskDetailDialog
    }

    pageStack.initialPage: taskListComponent

    TaskListView {
        id: taskListComponent
        onTaskSelected: function (task) {
            taskDetailDialog.loadTask(task);
            taskDetailDialog.open();
        }
    }

    AddTaskDialog {
        id: addTaskDialog
        onTaskAdded: {
            todoModel.refresh();
        }
    }

    AddProjectDialog {
        id: addProjectDialog
        onProjectAdded: {
            projectModel.refresh();
        }
    }

    SystemTrayIcon {
        id: systemTray
        visible: true
        icon.source: "org.kde.todoist"

        menu: Menu {
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }

        onActivated: {
            root.show();
            root.raise();
            root.requestActivate();
        }
    }

    // Component.onCompleted: {
    //     todoModel.currentProjectId = 1;
    // }
}
