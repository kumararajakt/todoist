import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

pragma ComponentBehavior: Bound

Kirigami.Dialog {
    id: dialog

    property int currentTaskId: -1
    property bool editMode: false
    property var taskData: null

    padding: Kirigami.Units.largeSpacing
    
    // Date picker popup
    Popup {
        id: dueDatePopup
        width: 300
        height: 350
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing
            
            Label {
                text: "Select Due Date"
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                Layout.alignment: Qt.AlignHCenter
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                GridLayout {
                    id: calendarGrid
                    columns: 7
                    rowSpacing: 5
                    columnSpacing: 5
                    
                    property date currentDate: dialog.taskData && dialog.taskData.dueDate ? new Date(dialog.taskData.dueDate) : new Date()
                    property int currentMonth: currentDate.getMonth()
                    property int currentYear: currentDate.getFullYear()
                    
                    // Month/Year header
                    Label {
                        Layout.columnSpan: 7
                        Layout.fillWidth: true
                        text: Qt.formatDate(calendarGrid.currentDate, "MMMM yyyy")
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
                        font.weight: Font.Bold
                        horizontalAlignment: Text.AlignHCenter
                    }
                    
                    // Day headers
                    Repeater {
                        model: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
                        delegate: Label {
                            required property string modelData
                            text: modelData
                            font.weight: Font.Bold
                            horizontalAlignment: Text.AlignHCenter
                            Layout.preferredWidth: 35
                            Layout.preferredHeight: 25
                        }
                    }
                    
                    // Calendar days
                    Repeater {
                        model: 42 // 6 weeks * 7 days
                        
                        delegate: Button {
                            required property int index
                            property date buttonDate: {
                                let firstDay = new Date(calendarGrid.currentYear, calendarGrid.currentMonth, 1)
                                let startDate = new Date(firstDay)
                                startDate.setDate(startDate.getDate() - firstDay.getDay())
                                let result = new Date(startDate)
                                result.setDate(result.getDate() + index)
                                return result
                            }
                            
                            text: buttonDate.getDate()
                            Layout.preferredWidth: 35
                            Layout.preferredHeight: 35
                            
                            enabled: buttonDate.getMonth() === calendarGrid.currentMonth
                            
                            onClicked: {
                                if (dialog.taskData) {
                                    dialog.taskData.dueDate = buttonDate.toISOString()
                                }
                                dueDatePopup.close()
                            }
                        }
                    }
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                Button {
                    text: "Today"
                    Layout.fillWidth: true
                    onClicked: {
                        let today = new Date()
                        if (dialog.taskData) {
                            dialog.taskData.dueDate = today.toISOString()
                        }
                        dueDatePopup.close()
                    }
                }
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: dueDatePopup.close()
                }
            }
        }
    }

    // Add subtask dialog
    SubtaskDialog {
        id: addSubtaskDialog
        parentTaskId: dialog.currentTaskId
        parentTaskContent: dialog.taskData ? dialog.taskData.content : ""

        // onSubtaskAdded: {
        //     todoModel.refresh();
        //     loadTask(currentTaskId); // Reload to update subtask counts
        // }
    }

    function loadTask(task) {
        console.log("Loading task:", task);
        taskData = task;
        currentTaskId = task.id;
    }

    function saveTask() {
        if (currentTaskId === -1 || !taskData)
            return;

        let dueDate = taskData.dueDate ? new Date(taskData.dueDate) : new Date(0);
        
        todoModel.updateTask(
            currentTaskId,
            taskData.content,
            taskData.description,
            dueDate,
            taskData.priority,
            taskData.isRecurring,
            taskData.repeatInterval,
            taskData.repeatFrequency,
            taskData.repeatStartDate ? new Date(taskData.repeatStartDate) : new Date(0),
            taskData.repeatEndType,
            taskData.repeatEndDate ? new Date(taskData.repeatEndDate) : new Date(0),
            taskData.repeatEndCount
        );
        editMode = false;
    }

    function formatDate(dateString) {
        if (!dateString) return "";
        let date = new Date(dateString);
        return date.toLocaleDateString('en-US', { 
            month: 'short', 
            day: 'numeric', 
            year: 'numeric' 
        });
    }

    title: "Task Detail"


    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            // Task Title
            Kirigami.Heading {
                Layout.fillWidth: true
                id: taskTitle
                text: dialog.taskData ? dialog.taskData.content : ""
                level: 1
                wrapMode: Text.WordWrap
                Layout.topMargin: Kirigami.Units.largeSpacing
                visible: !dialog.editMode
            }
            
            TextField {
                Layout.fillWidth: true
                id: taskTitleEdit
                text: dialog.taskData ? dialog.taskData.content : ""
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                font.weight: Font.Bold
                Layout.topMargin: Kirigami.Units.largeSpacing
                visible: dialog.editMode
                onTextChanged: {
                    if (dialog.taskData) {
                        dialog.taskData.content = text
                    }
                }
            }

            // Task Description
            Label {
                id: taskDescription
                text: dialog.taskData ? dialog.taskData.description : ""
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
                color: Kirigami.Theme.disabledTextColor
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                visible: text.length > 0 && !dialog.editMode
            }
            
            TextField {
                Layout.fillWidth: true
                id: taskDescriptionEdit
                text: dialog.taskData ? dialog.taskData.description : ""
                placeholderText: "Add description..."
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
                visible: dialog.editMode
                onTextChanged: {
                    if (dialog.taskData) {
                        dialog.taskData.description = text
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            // Due Date Section - View Mode
            RowLayout {
                Layout.fillWidth: true
                visible: dialog.taskData && dialog.taskData.dueDate && !dialog.editMode
                Layout.topMargin: Kirigami.Units.largeSpacing

                Kirigami.Icon {
                    source: "view-calendar"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                }

                Label {
                    text: "Due: " + (dialog.taskData ? formatDate(dialog.taskData.dueDate) : "")
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize
                    color: Kirigami.Theme.textColor
                }
            }
            
            // Due Date Section - Edit Mode
            RowLayout {
                Layout.fillWidth: true
                visible: dialog.editMode
                Layout.topMargin: Kirigami.Units.largeSpacing

                Kirigami.Icon {
                    source: "view-calendar"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                }

                Label {
                    text: "Due Date:"
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize
                    color: Kirigami.Theme.textColor
                }
                
                Button {
                    id: dueDateButton
                    text: dialog.taskData && dialog.taskData.dueDate ? formatDate(dialog.taskData.dueDate) : "Select Date"
                    onClicked: {
                        dueDatePopup.open()
                    }
                }
                
                Button {
                    text: "Clear"
                    visible: dialog.taskData && dialog.taskData.dueDate
                    onClicked: {
                        if (dialog.taskData) {
                            dialog.taskData.dueDate = null
                        }
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            // Repeat Settings - View Mode
            RowLayout {
                Layout.fillWidth: true
                visible: dialog.taskData && dialog.taskData.isRecurring && !dialog.editMode

                Kirigami.Icon {
                    source: "view-refresh"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                    color: Kirigami.Theme.textColor
                }

                Label {
                    text: {
                        if (!dialog.taskData || !dialog.taskData.isRecurring) return "";
                        let intervalText = "";
                        if (dialog.taskData.repeatInterval === "daily") intervalText = "Daily";
                        else if (dialog.taskData.repeatInterval === "weekly") intervalText = "Weekly";
                        else if (dialog.taskData.repeatInterval === "monthly") intervalText = "Monthly";
                        else if (dialog.taskData.repeatInterval === "yearly") intervalText = "Yearly";
                        else intervalText = "Custom";
                        
                        if (dialog.taskData.repeatFrequency > 1) {
                            intervalText = "Every " + dialog.taskData.repeatFrequency + " " + dialog.taskData.repeatInterval;
                        }
                        
                        return "Repeats: " + intervalText;
                    }
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize
                    color: Kirigami.Theme.textColor
                }
            }
            
            // Repeat Settings - Edit Mode
            ColumnLayout {
                Layout.fillWidth: true
                visible: dialog.editMode
                spacing: Kirigami.Units.smallSpacing
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Kirigami.Icon {
                        source: "view-refresh"
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                        color: Kirigami.Theme.textColor
                    }
                    
                    CheckBox {
                        id: repeatCheckBox
                        text: "Repeat task"
                        checked: dialog.taskData ? dialog.taskData.isRecurring : false
                        onCheckedChanged: {
                            if (dialog.taskData) {
                                dialog.taskData.isRecurring = checked
                                if (!checked) {
                                    dialog.taskData.repeatInterval = ""
                                    dialog.taskData.repeatFrequency = 0
                                }
                            }
                        }
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    visible: repeatCheckBox.checked
                    
                    Label {
                        text: "Every"
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize
                        color: Kirigami.Theme.textColor
                    }
                    
                    SpinBox {
                        id: frequencySpinBox
                        from: 1
                        to: 999
                        value: dialog.taskData ? Math.max(1, dialog.taskData.repeatFrequency) : 1
                        onValueChanged: {
                            if (dialog.taskData) {
                                dialog.taskData.repeatFrequency = value
                            }
                        }
                    }
                    
                    ComboBox {
                        id: intervalComboBox
                        model: ["day(s)", "week(s)", "month(s)", "year(s)"]
                        currentIndex: {
                            if (!dialog.taskData) return 0;
                            switch(dialog.taskData.repeatInterval) {
                                case "daily": return 0;
                                case "weekly": return 1;
                                case "monthly": return 2;
                                case "yearly": return 3;
                                default: return 0;
                            }
                        }
                        onCurrentIndexChanged: {
                            if (dialog.taskData) {
                                const intervals = ["daily", "weekly", "monthly", "yearly"];
                                dialog.taskData.repeatInterval = intervals[currentIndex];
                            }
                        }
                    }
                }
            }

            // Add Subtask Section - Edit Mode Only
            RowLayout {
                Layout.fillWidth: true
                visible: dialog.editMode
                Layout.topMargin: Kirigami.Units.largeSpacing

                Kirigami.Icon {
                    source: "list-add"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                    color: Kirigami.Theme.textColor
                }

                Button {
                    text: "Add Subtask"
                    Layout.fillWidth: true
                    onClicked: {
                        addSubtaskDialog.parentTaskId = dialog.currentTaskId
                        addSubtaskDialog.open()
                    }
                }
            }

            // Subtasks Section
            ColumnLayout {
                Layout.fillWidth: true
                visible: dialog.taskData && dialog.taskData.hasSubtasks
                Layout.topMargin: Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    text: "Subtasks"
                    level: 3
                }

                // Example subtasks based on the image
                RowLayout {
                    Layout.fillWidth: true
                    
                    CheckBox {
                        checked: true
                    }
                    
                    Label {
                        text: "Draft initial layout"
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    
                    CheckBox {
                        checked: false
                    }
                    
                    Label {
                        text: "Get feedback from the team"
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    
                    CheckBox {
                        checked: false
                    }
                    
                    Label {
                        text: "Implement changes"
                        Layout.fillWidth: true
                    }
                }
            }

            // Spacer
            Item {
                Layout.fillHeight: true
                Layout.minimumHeight: Kirigami.Units.largeSpacing
            }
        }
    }

    // Custom footer with Edit button
    customFooterActions: [
        Kirigami.Action {
            text: "Edit"
            visible: !dialog.editMode
            onTriggered: {
                dialog.editMode = true;
            }
        },
        Kirigami.Action {
            text: "Save"
            visible: dialog.editMode
            onTriggered: {
                dialog.saveTask();
                dialog.editMode = false;
            }
        },
        Kirigami.Action {
            text: "Delete"
            visible: !dialog.editMode
            onTriggered: {
                if (todoModel && dialog.currentTaskId !== -1) {
                    todoModel.deleteTask(dialog.currentTaskId);
                }
                dialog.close();
            }
        },
        Kirigami.Action {
            text: "Close"
            onTriggered: {
                dialog.close();
            }
        }
    ]
}
