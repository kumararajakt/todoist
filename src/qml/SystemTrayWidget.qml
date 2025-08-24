import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

// Enhanced System Tray Widget with notification management
Item {
    id: root
    
    property bool systemTrayAvailable: systemTrayManager ? systemTrayManager.available : false
    property bool systemTrayVisible: systemTrayManager ? systemTrayManager.visible : false
    property var notificationFunction: null
    
    // Notification queue for managing multiple notifications
    property var notificationQueue: []
    property bool processingNotifications: false
    
    function showTrayNotification(title, message, type, duration) {
        if (notificationFunction) {
            notificationFunction(title, message, type || "info", duration || 5000);
        }
        
        // Also show system tray notification
        if (systemTrayManager && systemTrayAvailable) {
            systemTrayManager.showNotification(title, message, duration || 5000);
        }
    }
    
    function queueNotification(title, message, type, duration) {
        notificationQueue.push({
            title: title,
            message: message,
            type: type || "info",
            duration: duration || 5000
        });
        
        if (!processingNotifications) {
            processNotificationQueue();
        }
    }
    
    function processNotificationQueue() {
        if (notificationQueue.length === 0) {
            processingNotifications = false;
            return;
        }
        
        processingNotifications = true;
        var notification = notificationQueue.shift();
        showTrayNotification(notification.title, notification.message, notification.type, notification.duration);
        
        // Process next notification after a delay
        Qt.callLater(function() {
            processNotificationTimer.start();
        });
    }
    
    Timer {
        id: processNotificationTimer
        interval: 1000 // 1 second delay between notifications
        onTriggered: processNotificationQueue()
    }
    
    // Enhanced system tray controls
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing
        
        // System tray status section
        Kirigami.Card {
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                spacing: Kirigami.Units.smallSpacing
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Kirigami.Icon {
                        source: "preferences-system-notifications"
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    }
                    
                    Label {
                        Layout.fillWidth: true
                        text: "System Tray"
                        font.bold: true
                    }
                    
                    // Status indicator
                    Rectangle {
                        Layout.preferredWidth: 12
                        Layout.preferredHeight: 12
                        radius: 6
                        color: systemTrayAvailable ? 
                            (systemTrayVisible ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.neutralTextColor) : 
                            Kirigami.Theme.negativeTextColor
                        
                        ToolTip.visible: statusMouseArea.containsMouse
                        ToolTip.text: systemTrayAvailable ? 
                            (systemTrayVisible ? "System tray active" : "System tray available but hidden") : 
                            "System tray not available"
                        
                        MouseArea {
                            id: statusMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }
                }
                
                Label {
                    Layout.fillWidth: true
                    text: systemTrayAvailable ? 
                        (systemTrayVisible ? "Running in system tray" : "System tray hidden") : 
                        "System tray not available"
                    opacity: 0.7
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                }
            }
        }
        
        // Control buttons
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing
            
            Button {
                Layout.fillWidth: true
                text: systemTrayVisible ? "Hide Tray" : "Show Tray"
                icon.name: systemTrayVisible ? "visibility" : "hint"
                enabled: systemTrayAvailable
                
                onClicked: {
                    if (systemTrayManager) {
                        systemTrayManager.visible = !systemTrayVisible;
                        showTrayNotification(
                            "System Tray", 
                            systemTrayVisible ? "System tray is now visible" : "System tray is now hidden",
                            "info",
                            3000
                        );
                    }
                }
            }
            
            Button {
                Layout.fillWidth: true
                text: "Test Notification"
                icon.name: "notifications"
                enabled: systemTrayAvailable
                
                onClicked: {
                    showTrayNotification(
                        "Test Notification", 
                        "This is a test notification from TodoApp!\nSystem tray is working correctly.",
                        "success",
                        4000
                    );
                }
            }
            
            Button {
                Layout.fillWidth: true
                text: "Check Reminders"
                icon.name: "appointment-new"
                enabled: systemTrayAvailable
                
                onClicked: {
                    if (systemTrayManager) {
                        systemTrayManager.checkReminders();
                        showTrayNotification(
                            "Reminder Check", 
                            "Checking for due tasks and reminders...",
                            "info",
                            3000
                        );
                    }
                }
            }
        }
        
        // Notification settings
        Kirigami.Card {
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                spacing: Kirigami.Units.smallSpacing
                
                Label {
                    text: "Notification Settings"
                    font.bold: true
                }
                
                CheckBox {
                    id: enableRemindersCheckBox
                    text: "Enable task reminders"
                    checked: true
                    
                    ToolTip.text: "Show notifications for tasks that are due soon"
                }
                
                CheckBox {
                    id: enableCompletionNotificationsCheckBox
                    text: "Task completion notifications"
                    checked: true
                    
                    ToolTip.text: "Show notifications when tasks are completed"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Label {
                        text: "Reminder frequency:"
                        opacity: 0.7
                    }
                    
                    ComboBox {
                        id: reminderFrequencyCombo
                        Layout.fillWidth: true
                        
                        model: [
                            { text: "Every 5 minutes", value: 5 },
                            { text: "Every 10 minutes", value: 10 },
                            { text: "Every 15 minutes", value: 15 },
                            { text: "Every 30 minutes", value: 30 },
                            { text: "Every hour", value: 60 }
                        ]
                        
                        textRole: "text"
                        valueRole: "value"
                        currentIndex: 0
                    }
                }
            }
        }
        
        Item {
            Layout.fillHeight: true
        }
    }
    
    // Connect to system tray manager signals for enhanced notifications
    Connections {
        target: systemTrayManager
        
        function onReminderTriggered(taskContent) {
            queueNotification(
                "📋 Task Reminder",
                "Don't forget: " + taskContent,
                "warning",
                8000
            );
        }
        
        function onAvailabilityChanged() {
            if (systemTrayAvailable) {
                queueNotification(
                    "System Tray Available",
                    "System tray is now available and ready to use.",
                    "success",
                    4000
                );
            }
        }
    }
}