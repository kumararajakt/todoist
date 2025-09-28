pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.todoist

Kirigami.ScrollablePage {
    id: taskDetailPage

    property int currentTaskId: -1
    property bool isEdited: false
    property var taskData: null
    required property var todoModel
    property bool isTimerRequested: false

    property int timeSpent: 0
    property int estimate: 0
    property var timerStartTime: null
    property bool isTimerActive: false

    signal focusRequested(bool focus)
    property bool isFocused: false

    onIsFocusedChanged: {
        if (isFocused && taskDetailPage.isTimerActive) {
            taskDetailPage.stopTimer();
        }
    }

    onIsTimerRequestedChanged: {
        if (isTimerRequested) {
            taskDetailPage.startTimer();
        } else {
            taskDetailPage.stopTimer();
        }
    }

    title: taskData ? taskData.content : "Task Details"

    actions: [
        Kirigami.Action {
            text: "Focus"
            icon.name: "focus"
            visible: taskDetailPage.isTimerRequested && !taskDetailPage.isFocused
            onTriggered: {
                taskDetailPage.focusRequested(true);
                taskDetailPage.isFocused = true;
            }
        },
        Kirigami.Action {
            text: "Start Timer"
            icon.name: "media-playback-start"
            visible: !taskDetailPage.isTimerRequested && !taskDetailPage.isFocused
            onTriggered: {
                taskDetailPage.isTimerRequested = true;
            }
        },
        Kirigami.Action {
            text: "Save"
            icon.name: "document-save"
            visible: taskDetailPage.isEdited && !taskDetailPage.isTimerRequested && !taskDetailPage.isFocused
            onTriggered: {
                taskDetailPage.saveTask();
                taskDetailPage.isEdited = false;
            }
        },
        Kirigami.Action {
            text: "Delete"
            icon.name: "delete"
            visible: !taskDetailPage.isTimerRequested && !taskDetailPage.isFocused
            onTriggered: {
                deleteConfirmationDialog.open();
            }
        }
    ]

    function getRepeatConfig(config) {
        let repeatStr = "";
        const repeatConfig = JSON.parse(config);
        if (repeatConfig) {
            if (repeatConfig.interval === "days") {
                repeatStr = `repeat every ${repeatConfig.frequency} days`;
            } else if (repeatConfig.interval === "weeks") {
                repeatStr = `repeat every ${repeatConfig.frequency} weeks`;
            } else if (repeatConfig.interval === "months") {
                repeatStr = `repeat every ${repeatConfig.frequency} months`;
            } else if (repeatConfig.interval === "years") {
                repeatStr = `repeat every ${repeatConfig.frequency} years`;
            }
        }
        return repeatStr;
    }

    // Add subtask dialog
    SubtaskDialog {
        id: addSubtaskDialog
        parentTaskId: taskDetailPage.currentTaskId
        parentTaskContent: taskDetailPage.taskData ? taskDetailPage.taskData.content : ""
    }

    RepeatSettingsCard {
        id: repeatSettingsCard
        todoModel: taskDetailPage.todoModel
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

    function updateProgressBar() {
        if (taskData && taskData.time_tracker) {
            try {
                const timeTracker = JSON.parse(taskData.time_tracker);
                const timeSpent = taskDetailPage.timeSpent || timeTracker.time_spent || 0;
                const estimate = taskDetailPage.estimate || timeTracker.estimate || 0;
                console.log("Time spent:", timeSpent);
                console.log("Estimate:", estimate);
                if (estimate > 0) {
                    const progress = Math.min(timeSpent / estimate, 1.0);
                    console.log("Progress:", progress);
                    progressBar.value = progress;
                } else {
                    progressBar.value = 0;
                }
            } catch (e) {
                console.log("Error parsing timeTracker:", e);
                progressBar.value = 0;
            }
        } else {
            // Use real-time values if no timeTracker data
            if (taskDetailPage.estimate > 0) {
                const progress = Math.min(taskDetailPage.timeSpent / taskDetailPage.estimate, 1.0);
                progressBar.value = progress;
            } else {
                progressBar.value = 0;
            }
        }
    }

    function startTimer() {
        if (!isTimerActive) {
            timerStartTime = new Date();
            isTimerActive = true;
            timer.running = true;
        }
    }

    function updateTimeTrackerData() {
        if (!taskData) {
            console.log("No task data available for time tracker update");
            return;
        }

        try {
            // Create or update time tracker object
            let timeTracker = {};
            if (taskData.time_tracker) {
                timeTracker = JSON.parse(taskData.time_tracker);
            }

            // Update with current values
            timeTracker.time_spent = timeSpent;
            timeTracker.estimate = estimate;

            // Save back to task data
            taskData.time_tracker = JSON.stringify(timeTracker);
            taskDetailPage.isEdited = true;
        } catch (e) {
            console.log("Error updating time tracker data:", e);
        }
    }

    function stopTimer() {
        if (isTimerActive) {
            isTimerActive = false;
            timer.running = false;
            updateTimeTrackerData(); // Save final time to database
        }
    }

    function getTimeinMinutes(time) {
        const timeSpentMs = time || 0;
        const totalMinutes = Math.floor(timeSpentMs / (1000 * 60));
        const hours = Math.floor(totalMinutes / 60);
        const minutes = totalMinutes % 60;

        return {
            hours: hours,
            minutes: minutes
        };
    }

    function loadTask(task) {
        taskData = task;
        currentTaskId = task.id;

        // Initialize time tracker data
        try {
            if (task.time_tracker) {
                const timeTracker = JSON.parse(task.time_tracker);

                // Initialize properties from database
                timeSpent = parseInt(timeTracker.time_spent || 0);
                estimate = parseInt(timeTracker.estimate || 0);

                // Convert to hours/minutes for SpinBoxes
                const timeSpentResult = getTimeinMinutes(timeTracker.time_spent);
                timeSpentHours.value = timeSpentResult.hours;
                timeSpentMinutes.value = timeSpentResult.minutes;

                const estimateTimeResult = getTimeinMinutes(timeTracker.estimate);
                estimateHours.value = estimateTimeResult.hours;
                estimateMinutes.value = estimateTimeResult.minutes;

                console.log("Loaded timeTracker:", timeTracker);
            } else {
                // Initialize empty time tracker
                timeSpent = 0;
                estimate = 0;
                timeSpentHours.value = 0;
                timeSpentMinutes.value = 0;
                estimateHours.value = 0;
                estimateMinutes.value = 0;
            }
        } catch (e) {
            console.log("Error loading timeTracker:", e);
            // Reset on error
            timeSpent = 0;
            estimate = 0;
            timeSpentHours.value = 0;
            timeSpentMinutes.value = 0;
            estimateHours.value = 0;
            estimateMinutes.value = 0;
        }

        // Update progress bar with loaded data
        updateProgressBar();
    }

    function saveTask() {
        if (currentTaskId === -1 || !taskData)
            return;

        // Update timeTracker data before saving
        updateTimeTrackerData();

        const updated = todoModel.updateTask(currentTaskId, taskData);
        if (updated) {
            inlineMessage.text = "Task updated successfully";
            inlineMessage.visible = true;
            inlineMessage.type = Kirigami.MessageType.Positive;
            // inlineMessage.duration = Kirigami.Units.gridUnit * 2;

        } else {
            inlineMessage.text = "Failed to update task";
            inlineMessage.visible = true;
            inlineMessage.type = Kirigami.MessageType.Error;
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

        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            to: taskDetailPage.estimate
            from: 0

            Timer {
                id: timer
                interval: 1000
                running: false
                repeat: true
                onTriggered: {
                    if (taskDetailPage.isTimerActive && taskDetailPage.timerStartTime) {
                        const currentTime = new Date();
                        const elapsedMs = currentTime - taskDetailPage.timerStartTime;

                        // Update timeSpent with current elapsed time
                        taskDetailPage.timeSpent = elapsedMs;
                        taskDetailPage.updateProgressBar();

                        // Save to database every 10 seconds
                        if (Math.floor(elapsedMs / 1000) % 10 === 0) {
                            taskDetailPage.updateTimeTrackerData();
                        }
                    }
                }
            }
        }

        // Task content
        FormCard.FormCard {

            Layout.fillWidth: true

            FormCard.FormTextFieldDelegate {
                id: contentField
                enabled: !taskDetailPage.isFocused
                label: "Task:"
                text: taskDetailPage.taskData ? taskDetailPage.taskData.content : ""

                onTextChanged: {
                    taskDetailPage.isEdited = true;
                    if (taskDetailPage.taskData) {
                        taskDetailPage.taskData.content = text;
                    }
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextAreaDelegate {
                enabled: !taskDetailPage.isFocused
                label: "Description"
                text: taskDetailPage.taskData ? (taskDetailPage.taskData.description || "") : ""
                onTextChanged: {
                    taskDetailPage.isEdited = true;
                    if (taskDetailPage.taskData) {
                        taskDetailPage.taskData.description = text;
                    }
                }
                // readOnly: true
            }

            FormCard.FormDelegateSeparator {}

            FormCard.AbstractFormDelegate {

                contentItem: ColumnLayout {

                    Label {
                        text: "Due Date"
                        Layout.fillWidth: true
                    }

                    FormCard.FormDateTimeDelegate {
                        enabled: !taskDetailPage.isFocused
                        value: taskDetailPage.taskData?.due_date ? new Date(taskDetailPage.taskData.due_date) : ""
                        onValueChanged: {
                            taskDetailPage.isEdited = true;
                            if (taskDetailPage.taskData) {
                                taskDetailPage.taskData.due_date = value;
                            }
                        }
                    }
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.AbstractFormDelegate {

                contentItem: ColumnLayout {

                    Label {
                        text: "Time Spent"
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        SpinBox {
                            id: timeSpentHours
                            enabled: !taskDetailPage.isFocused
                            onValueChanged: {
                                const totalMinutes = (timeSpentHours.value * 60) + timeSpentMinutes.value;
                                taskDetailPage.timeSpent = totalMinutes * 60 * 1000; // Convert to milliseconds
                                taskDetailPage.isEdited = true;
                                taskDetailPage.updateTimeTrackerData();
                            }
                        }
                        Label {
                            text: "Hours"
                        }
                        SpinBox {
                            id: timeSpentMinutes
                            enabled: !taskDetailPage.isFocused
                            onValueChanged: {
                                const totalMinutes = (timeSpentHours.value * 60) + timeSpentMinutes.value;
                                taskDetailPage.timeSpent = totalMinutes * 60 * 1000; // Convert to milliseconds
                                taskDetailPage.isEdited = true;
                                taskDetailPage.updateTimeTrackerData();
                            }
                        }
                        Label {
                            text: "Minutes"
                        }

                        Component.onCompleted: {}
                    }
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.AbstractFormDelegate {

                contentItem: ColumnLayout {

                    Label {
                        text: "Estimate Time"
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        SpinBox {
                            id: estimateHours
                            enabled: !taskDetailPage.isFocused
                            onValueChanged: {
                                const totalMinutes = (estimateHours.value * 60) + estimateMinutes.value;
                                taskDetailPage.estimate = totalMinutes * 60 * 1000; // Convert to milliseconds
                                taskDetailPage.isEdited = true;
                                taskDetailPage.updateTimeTrackerData();
                            }
                        }
                        Label {
                            text: "Hours"
                        }
                        SpinBox {
                            id: estimateMinutes
                            enabled: !taskDetailPage.isFocused
                            onValueChanged: {
                                const totalMinutes = (estimateHours.value * 60) + estimateMinutes.value;
                                taskDetailPage.estimate = totalMinutes * 60 * 1000; // Convert to milliseconds
                                taskDetailPage.isEdited = true;
                                taskDetailPage.updateTimeTrackerData();
                            }
                        }
                        Label {
                            text: "Minutes"
                        }

                        Component.onCompleted: {}
                    }
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                text: `Repeat: ${taskDetailPage.getRepeatConfig(taskDetailPage.taskData.repeat_config)}`

                onClicked: {
                    repeatSettingsCard.loadConfig(taskDetailPage.currentTaskId, taskDetailPage.taskData.repeat_config);
                    repeatSettingsCard.open();
                }
            }
        }
    }
}
