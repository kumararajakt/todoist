pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.todoist

Kirigami.ApplicationWindow {
    id: root

    title: "Todoist"
    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    property int currentProjectId: -1
    property string currentFilter: ""
    property var projectModel: null
    property var todoModel: null
    property var addTaskDialog: null
    property var addProjectDialog: null

    readonly property int sidebarWidth: Kirigami.Units.gridUnit * 14

    property alias pomodoroManager: pomodoroManager

    PomodoroManager {
        id: pomodoroManager
    }

    ProjectModel {
        id: projectModel
    }

    TodoModel {
        id: todoModel
    }

    Component.onCompleted: {
        root.projectModel = projectModel;
        root.todoModel = todoModel;
        root.addTaskDialog = addTaskDialog;
        root.addProjectDialog = addProjectDialog;
    }

    globalDrawer: ProjectSidebar {
        id: sidebar
        width: root.sidebarWidth
        projectModel: root.projectModel
        addProjectDialog: root.addProjectDialog

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

    Component {
        id: taskDetailPageComponent
        TaskDetailPage {
            todoModel: root.todoModel
        }
    }

    pageStack.initialPage: taskListComponent

    TaskListView {
        id: taskListComponent
        todoModel: root.todoModel
        projectModel: root.projectModel
        addTaskDialog: root.addTaskDialog

        onTaskSelected: function (task) {
            let taskDetailPage = taskDetailPageComponent.createObject(root);
            taskDetailPage.loadTask(task);
            root.pageStack.layers.push(taskDetailPage);
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

    // SystemTrayManager {
    //     id: systemTray

    //     // menu: Menu {
    //     //     MenuItem {
    //     //         text: qsTr("Quit")
    //     //         onTriggered: Qt.quit()
    //     //     }
    //     // }

    //     // onActivated: {
    //     //     root.show();
    //     //     root.raise();
    //     //     root.requestActivate();
    //     // }
    // }


}
