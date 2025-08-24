import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: dialog
    
    signal projectAdded()
    
    title: "Add New Project"
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    onAccepted: {
        if (nameField.text.trim().length > 0) {
            projectModel.addProject(
                nameField.text.trim(),
                colorRepeater.itemAt(selectedColorIndex).color,
                favoriteField.checked
            )
            
            projectAdded()
            clearFields()
        }
    }
    
    onRejected: {
        clearFields()
    }
    
    property int selectedColorIndex: 0
    
    function clearFields() {
        nameField.text = ""
        favoriteField.checked = false
        selectedColorIndex = 0
    }
    
    ColumnLayout {
        width: Math.min(400, parent.width)
        spacing: Kirigami.Units.largeSpacing
        
        Kirigami.FormLayout {
            Layout.fillWidth: true
            
            TextField {
                id: nameField
                Kirigami.FormData.label: "Project Name:"
                placeholderText: "Enter project name"
                focus: true
            }
            
            CheckBox {
                id: favoriteField
                Kirigami.FormData.label: "Favorite:"
                text: "Add to favorites"
            }
            
            Label {
                Kirigami.FormData.label: "Color:"
                text: "Choose a color"
            }
        }
        
        // Color picker
        GridLayout {
            Layout.fillWidth: true
            columns: 6
            columnSpacing: Kirigami.Units.smallSpacing
            rowSpacing: Kirigami.Units.smallSpacing
            
            property var colors: [
                "#3498db", "#e74c3c", "#2ecc71", "#f39c12",
                "#9b59b6", "#1abc9c", "#34495e", "#e67e22",
                "#f1c40f", "#95a5a6", "#e91e63", "#607d8b"
            ]
            
            Repeater {
                id: colorRepeater
                model: parent.colors
                
                delegate: Rectangle {
                    width: 40
                    height: 40
                    radius: 20
                    color: modelData
                    border.width: dialog.selectedColorIndex === index ? 3 : 1
                    border.color: dialog.selectedColorIndex === index ? Kirigami.Theme.highlightColor : Kirigami.Theme.separatorColor
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            dialog.selectedColorIndex = index
                        }
                    }
                    
                    Kirigami.Icon {
                        anchors.centerIn: parent
                        width: 20
                        height: 20
                        source: "checkmark"
                        color: "white"
                        visible: dialog.selectedColorIndex === index
                    }
                }
            }
        }
    }
}