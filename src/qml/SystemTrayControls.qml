import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

// System Tray Controls Component
RowLayout {
    id: root
    
    property bool systemTrayAvailable: systemTrayManager ? systemTrayManager.available : false
    property bool systemTrayVisible: systemTrayManager ? systemTrayManager.visible : false
    property var notificationFunction: null
    
    spacing: Kirigami.Units.smallSpacing
    
    // System tray status indicator
    Rectangle {
        Layout.preferredWidth: 12
        Layout.preferredHeight: 12
        radius: 6
        color: systemTrayAvailable ? (systemTrayVisible ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.neutralTextColor) : Kirigami.Theme.negativeTextColor
        
        ToolTip.visible: systemTrayMouseArea.containsMouse
        ToolTip.text: systemTrayAvailable ? 
            (systemTrayVisible ? "System tray active" : "System tray available but hidden") : 
            "System tray not available"
        
        MouseArea {
            id: systemTrayMouseArea
            anchors.fill: parent
            hoverEnabled: true
        }
    }
    
    // Toggle system tray visibility
    Button {
        text: systemTrayVisible ? "Hide Tray" : "Show Tray"
        enabled: systemTrayAvailable
        flat: true
        
        onClicked: {
            if (systemTrayManager) {
                systemTrayManager.visible = !systemTrayVisible;
            }
        }
    }
    
    // Test notification button
    Button {
        text: "Test Notification"
        enabled: systemTrayAvailable
        flat: true
        
        onClicked: {
            // Show both system tray and in-app notification
            if (notificationFunction) {
                notificationFunction(
                    "Test Notification", 
                    "This is a test notification from TodoApp!", 
                    "info",
                    3000
                );
            } else if (systemTrayManager) {
                systemTrayManager.showNotification(
                    "Test Notification", 
                    "This is a test notification from TodoApp!", 
                    3000
                );
            }
        }
    }
    
    // Check reminders button
    Button {
        text: "Check Reminders"
        enabled: systemTrayAvailable
        flat: true
        
        onClicked: {
            if (systemTrayManager) {
                systemTrayManager.checkReminders();
            }
        }
    }
}