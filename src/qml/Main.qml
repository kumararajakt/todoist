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

    // Settings backend
    AppSettings {
        id: settings
    }

    // Restore window size from settings and persist on changes
    width: settings.windowWidth
    height: settings.windowHeight
    onWidthChanged: settings.windowWidth = width
    onHeightChanged: settings.windowHeight = height

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

        root.raise();
    }

    globalDrawer: ProjectSidebar {
        id: sidebar
        width: root.sidebarWidth
        projectModel: root.projectModel
        addProjectDialog: root.addProjectDialog
        modal: !root.wideScreen
        lastSelectedProjectId: settings.lastSelectedProjectId


        onProjectSelected: function (projectId) {
            root.currentProjectId = projectId;
            todoModel.currentProjectId = projectId;
            todoModel.currentFilter = "";
            settings.lastSelectedProjectId = projectId;
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

            onFocusRequested: {
                sidebar.visible = false;
            }
        }
        
    }

    pageStack.initialPage: taskListComponent

    TaskListView {
        id: taskListComponent
        todoModel: root.todoModel
        projectModel: root.projectModel
        addTaskDialog: root.addTaskDialog

        onTaskSelected: function (task, isTimerRequested) {
            let taskDetailPage = taskDetailPageComponent.createObject(root);
            taskDetailPage.loadTask(task);
            taskDetailPage.isTimerRequested = isTimerRequested;
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



    SystemTrayManager {
        id: systemTrayManager
        todoModel: todoModel

        Component.onCompleted: {
            systemTrayManager.checkReminders();
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
