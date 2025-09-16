import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

ItemDelegate {
    id: taskItem

    signal toggleCompleted
    signal deleteTask
    signal moveTask(var task, int newProjectId)
    signal startTimerRequested

    required property var projectModel

    required property var dueDate
    required property bool isCompleted
    required property var id
    required property var content
    required property var description
    required property var labels
    required property bool hasSubtasks
    required property int completedSubtaskCount
    required property int subtaskCount
    required property bool pomodoroEnabled
    required property int pomodoroCount
    required property int pomodoroTarget
    required property bool isRecurring
    required property string repeatFrequency
    required property string repeatInterval
    required property int priority
    required property int projectId
    required property int totalTimeSpent
    required property bool timeTrackingActive
    required property var timeTrackingStarted

    // required property var pauseTimeTracking
    // required property var startTimeTracking

    property bool isOverdue: dueDate > 0 && dueDate < new Date() && !isCompleted
    property bool isDueToday: {
        if (!dueDate || dueDate === 0)
            return false;
        let today = new Date();
        let due = new Date(dueDate);
        return due.toDateString() === today.toDateString();
    }
    
    // Time tracking function
    function startTimer() {
        taskItem.timeTrackingStarted = true;

        // if (timeTrackingActive) {
        //     // If already active, pause the timer
        //     console.log("Pausing timer for task:", content);
        //     if (taskItem) {
        //         // taskItem.pauseTimeTracking();
        //     }
        // } else {
        //     // Start the timer
        //     console.log("Starting timer for task:", content);
        //     if (taskItem) {
        //     }
        // }
        // // Also emit the signal for any parent components that need to know
        startTimerRequested();
    }
    
    // Subtask dialog
    SubtaskDialog {
        id: subtaskDialog
        parentTaskId: taskItem.id
        parentTaskContent: taskItem.content
    }
    
    // Move task dialog
    MoveTaskDialog {
        id: moveTaskDialog
        projectModel: taskItem.projectModel

        
        onTaskMoved: function(task, newProjectId) {
            taskItem.moveTask(task, newProjectId);
        }
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        // Completion checkbox
        CheckBox {
            id: completionCheckBox
            checked: taskItem.isCompleted

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
                text: taskItem.content
                font.strikeout: taskItem.isCompleted
                opacity: taskItem.isCompleted ? 0.6 : 1.0
                wrapMode: Text.WordWrap

                color: {
                    if (taskItem.isCompleted)
                        return Kirigami.Theme.disabledTextColor;
                    if (taskItem.isOverdue)
                        return Kirigami.Theme.negativeTextColor;
                    return Kirigami.Theme.textColor;
                }
            }

            // Task description (if exists)
            Label {
                Layout.fillWidth: true
                visible: taskItem.description && taskItem.description.length > 0
                text: taskItem.description
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
                // Repeater {
                //     model: taskItem.labels

                //     delegate: Rectangle {
                //         height: Kirigami.Units.gridUnit
                //         width: labelText.width + Kirigami.Units.smallSpacing * 2
                //         radius: height / 2
                //         color: Kirigami.Theme.backgroundColor
                //         border.color: Kirigami.Theme.separatorColor
                //         border.width: 1

                //         Label {
                //             id: labelText
                //             anchors.centerIn: parent
                //             text: modelData
                //             font.pointSize: Kirigami.Theme.smallFont.pointSize
                //         }
                //     }
                // }
                
                // Subtask indicator
                // Rectangle {
                //     visible: taskItem.hasSubtasks
                //     Layout.preferredHeight: Kirigami.Units.gridUnit
                //     Layout.preferredWidth: subtaskLabel.width + Kirigami.Units.smallSpacing * 2
                //     radius: height / 2
                //     color: Kirigami.Theme.highlightColor
                //     opacity: 0.8
                    
                //     Label {
                //         id: subtaskLabel
                //         anchors.centerIn: parent
                //         text: model.completedSubtaskCount + "/" + model.subtaskCount + " subtasks"
                //         font.pointSize: Kirigami.Theme.smallFont.pointSize
                //         color: "white"
                //     }
                    
                //     MouseArea {
                //         anchors.fill: parent
                //         onClicked: subtaskDialog.open()
                //         cursorShape: Qt.PointingHandCursor
                //     }
                // }
                
                // Pomodoro progress indicator
                // Row {
                //     spacing: Kirigami.Units.smallSpacing
                //     visible: model.pomodoroEnabled
                    
                //     // Small tomato indicators for completed sessions
                //     Repeater {
                //         model: Math.min(taskItem.model.pomodoroTarget || 4, 4) // Limit display to 4 tomatoes
                        
                //         Rectangle {
                //             width: 10
                //             height: 10
                //             radius: 5
                //             color: index < (taskItem.model.pomodoroCount || 0) ? 
                //                    Kirigami.Theme.positiveTextColor : 
                //                    Kirigami.Theme.disabledTextColor
                //             border.color: Kirigami.Theme.textColor
                //             border.width: 1
                            
                //             Label {
                //                 anchors.centerIn: parent
                //                 text: "🍅"
                //                 font.pointSize: 5
                //                 visible: index < (taskItem.model.pomodoroCount || 0)
                //             }
                            
                //             ToolTip.visible: ma.containsMouse
                //             ToolTip.text: "Pomodoro " + (index + 1) + " of " + (taskItem.model.pomodoroTarget || 4)
                            
                //             MouseArea {
                //                 id: ma
                //                 anchors.fill: parent
                //                 hoverEnabled: true
                //             }
                //         }
                //     }
                    
                //     Label {
                //         text: (taskItem.model.pomodoroCount || 0) + "/" + (taskItem.model.pomodoroTarget || 4)
                //         font.pointSize: Kirigami.Theme.smallFont.pointSize
                //         color: Kirigami.Theme.disabledTextColor
                //         visible: (taskItem.model.pomodoroTarget || 0) > 4
                //     }
                // }

                Item {
                    Layout.fillWidth: true
                } // Spacer

                // Due date
                Label {
                    visible: taskItem.dueDate && taskItem.dueDate > 0
                    text: {
                        if (!taskItem.dueDate || taskItem.dueDate === 0)
                            return "";

                        let due = new Date(taskItem.dueDate);
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
                    visible: taskItem.isRecurring
                    source: "view-refresh"
                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    color: Kirigami.Theme.highlightColor
                    
                    ToolTip.visible: recurringMouseArea.containsMouse
                    ToolTip.text: "Recurring task: every " + taskItem.repeatFrequency + " " + taskItem.repeatInterval
                    
                    MouseArea {
                        id: recurringMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                // Priority indicator
                Rectangle {
                    visible:    taskItem.priority >= 3
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    radius: width / 2
                    color: {
                        switch (taskItem.priority) {
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

        // Circular progress bar for active pomodoro
        // CircularProgressBar {
        //     id: progressBar
        //     visible: model.pomodoroActive
        //     size: 32
        //     progress: {
        //         if (typeof root.pomodoroManager !== 'undefined') {
        //             return root.pomodoroManager.getProgressForTask(model.id)
        //         }
        //         return 0.0
        //     }
        //     progressColor: Kirigami.Theme.highlightColor
            
        //     ToolTip.visible: progressMouseArea.containsMouse
        //     ToolTip.text: "Pomodoro timer active - Click to view"
            
        //     MouseArea {
        //         id: progressMouseArea
        //         anchors.fill: parent
        //         hoverEnabled: true
        //         onClicked: taskItem.clicked() // Open task details to see timer
        //     }
            
        //     Connections {
        //         target: typeof root.pomodoroManager !== 'undefined' ? root.pomodoroManager : null
        //         function onProgressChanged(taskId, progress) {
        //             if (taskId === model.id) {
        //                 progressBar.progress = progress
        //             }
        //         }
        //     }
        // }

        // Time Tracker
        // TimeTracker {
        //     id: timeTracker
        //     task: taskItem
        //     compact: true
        //     Layout.preferredWidth: 150
            
        //     onTimeTrackingStarted: {
        //         console.log("Time tracking started for:", taskItem.content)
        //     }
            
        //     onTimeTrackingStopped: {
        //         console.log("Time tracking stopped for:", taskItem.content)
        //     }
            
        //     onTimeTrackingPaused: {
        //         console.log("Time tracking paused for:", taskItem.content)
        //     }
        // }

        // Actions
        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            // ToolButton {
            //     icon.name: "view-list-tree"
            //     flat: true
                
            //     ToolTip.text: "Manage subtasks"

            //     onClicked: {
            //         subtaskDialog.open();
            //     }
            // }

            ToolButton {
                icon.name: taskItem.timeTrackingActive ? "media-playback-pause" : "media-playback-start"
                flat: true
                
                ToolTip.text: taskItem.timeTrackingActive ? "Pause Timer" : "Start Timer"

                onClicked: {
                    taskItem.startTimer();
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

             ToolButton {
                icon.name: "edit-tag-symbolic"
                flat: true
                
                ToolTip.text: "Move to project"

                onClicked: {
                    moveTaskDialog.loadTask(taskItem.projectId);
                    moveTaskDialog.open();
                }
            }
        }
    }
}
