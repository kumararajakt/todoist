import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Item {
    id: root
    
    property alias notifications: notificationModel
    
    // Function to show a notification
    function showNotification(title, message, type, duration) {
        notificationModel.append({
            "title": title || "",
            "message": message || "",
            "type": type || "info", // info, warning, error, success
            "timestamp": Date.now(),
            "duration": duration || 5000
        });
    }
    
    // Notification model
    ListModel {
        id: notificationModel
    }
    
    // Notification container
    Column {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.smallSpacing
        z: 1000
        
        Repeater {
            model: notificationModel
            
            delegate: Kirigami.Card {
                id: notificationCard
                
                width: Math.min(350, root.width * 0.4)
                
                property int notificationIndex: index
                property string notificationType: model.type || "info"
                
                // Auto-remove after duration
                Timer {
                    interval: model.duration || 5000
                    running: true
                    repeat: false
                    onTriggered: {
                        hideAnimation.start();
                    }
                }
                
                // Show animation
                NumberAnimation on opacity {
                    id: showAnimation
                    from: 0
                    to: 1
                    duration: 300
                    easing.type: Easing.OutQuad
                    running: true
                }
                
                // Hide animation
                NumberAnimation on opacity {
                    id: hideAnimation
                    from: 1
                    to: 0
                    duration: 300
                    easing.type: Easing.InQuad
                    running: false
                    onFinished: {
                        notificationModel.remove(notificationIndex);
                    }
                }
                
                // Slide in animation
                NumberAnimation on x {
                    from: root.width
                    to: root.width - notificationCard.width - Kirigami.Units.largeSpacing
                    duration: 300
                    easing.type: Easing.OutQuad
                    running: true
                }
                
                // Color based on notification type
                background: Rectangle {
                    color: {
                        switch (notificationType) {
                        case "warning":
                            return Kirigami.Theme.neutralBackgroundColor;
                        case "error":
                            return Kirigami.Theme.negativeBackgroundColor;
                        case "success":
                            return Kirigami.Theme.positiveBackgroundColor;
                        default:
                            return Kirigami.Theme.backgroundColor;
                        }
                    }
                    border.color: {
                        switch (notificationType) {
                        case "warning":
                            return Kirigami.Theme.neutralTextColor;
                        case "error":
                            return Kirigami.Theme.negativeTextColor;
                        case "success":
                            return Kirigami.Theme.positiveTextColor;
                        default:
                            return Kirigami.Theme.textColor;
                        }
                    }
                    border.width: 1
                    radius: Kirigami.Units.cornerRadius
                }
                
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing
                    
                    // Icon based on type
                    Kirigami.Icon {
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        Layout.alignment: Qt.AlignTop
                        
                        source: {
                            switch (notificationType) {
                            case "warning":
                                return "dialog-warning";
                            case "error":
                                return "dialog-error";
                            case "success":
                                return "dialog-ok-apply";
                            default:
                                return "dialog-information";
                            }
                        }
                        
                        color: {
                            switch (notificationType) {
                            case "warning":
                                return Kirigami.Theme.neutralTextColor;
                            case "error":
                                return Kirigami.Theme.negativeTextColor;
                            case "success":
                                return Kirigami.Theme.positiveTextColor;
                            default:
                                return Kirigami.Theme.textColor;
                            }
                        }
                    }
                    
                    // Content
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing
                        
                        // Title
                        Label {
                            Layout.fillWidth: true
                            text: model.title
                            font.bold: true
                            wrapMode: Text.WordWrap
                            visible: text.length > 0
                        }
                        
                        // Message
                        Label {
                            Layout.fillWidth: true
                            text: model.message
                            wrapMode: Text.WordWrap
                            opacity: 0.8
                        }
                    }
                    
                    // Close button
                    ToolButton {
                        Layout.alignment: Qt.AlignTop
                        icon.name: "window-close"
                        flat: true
                        
                        onClicked: {
                            hideAnimation.start();
                        }
                    }
                }
                
                // Click to dismiss
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hideAnimation.start();
                    }
                }
            }
        }
    }
}