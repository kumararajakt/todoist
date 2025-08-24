import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Card {
    id: root
    
    property bool isRecurring: false
    property string repeatInterval: "days"
    property int repeatFrequency: 1
    property date repeatStartDate: new Date()
    property string repeatEndType: "never"
    property date repeatEndDate: new Date()
    property int repeatEndCount: 1
    
    // Function to reset all values
    function reset() {
        isRecurring = false;
        repeatInterval = "days";
        repeatFrequency = 1;
        repeatStartDate = new Date();
        repeatEndType = "never";
        repeatEndDate = new Date();
        repeatEndCount = 1;
    }
    
    // Function to load values from a task
    function loadFromTask(task) {
        if (task) {
            isRecurring = task.isRecurring || false;
            repeatInterval = task.repeatInterval || "days";
            repeatFrequency = task.repeatFrequency || 1;
            repeatStartDate = task.repeatStartDate || new Date();
            repeatEndType = task.repeatEndType || "never";
            repeatEndDate = task.repeatEndDate || new Date();
            repeatEndCount = task.repeatEndCount || 1;
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.largeSpacing
        
        // Header with repeat toggle
        RowLayout {
            Layout.fillWidth: true
            
            Kirigami.Icon {
                source: "view-refresh"
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
            }
            
            Label {
                Layout.fillWidth: true
                text: "Repeat Settings"
                font.bold: true
            }
            
            Switch {
                id: repeatSwitch
                checked: root.isRecurring
                onToggled: {
                    root.isRecurring = checked;
                }
            }
        }
        
        // Repeat configuration (visible when repeat is enabled)
        ColumnLayout {
            Layout.fillWidth: true
            visible: root.isRecurring
            spacing: Kirigami.Units.smallSpacing
            
            Kirigami.FormLayout {
                Layout.fillWidth: true
                
                // Repeat frequency and interval
                RowLayout {
                    Kirigami.FormData.label: "Repeat every:"
                    Layout.fillWidth: true
                    
                    SpinBox {
                        id: frequencySpinBox
                        from: 1
                        to: 999
                        value: root.repeatFrequency
                        onValueChanged: {
                            root.repeatFrequency = value;
                        }
                    }
                    
                    ComboBox {
                        id: intervalCombo
                        Layout.fillWidth: true
                        
                        model: [
                            { text: "day(s)", value: "days" },
                            { text: "week(s)", value: "weeks" },
                            { text: "month(s)", value: "months" },
                            { text: "year(s)", value: "years" }
                        ]
                        
                        textRole: "text"
                        valueRole: "value"
                        
                        currentIndex: {
                            for (let i = 0; i < model.length; i++) {
                                if (model[i].value === root.repeatInterval) {
                                    return i;
                                }
                            }
                            return 0;
                        }
                        
                        onActivated: {
                            root.repeatInterval = model[currentIndex].value;
                        }
                    }
                }
                
                // Start date
                TextField {
                    id: startDateField
                    Kirigami.FormData.label: "Start date:"
                    placeholderText: "YYYY-MM-DD"
                    inputMask: "9999-99-99"
                    text: {
                        if (root.repeatStartDate) {
                            let date = new Date(root.repeatStartDate);
                            return date.getFullYear() + "-" + 
                                   String(date.getMonth() + 1).padStart(2, '0') + "-" + 
                                   String(date.getDate()).padStart(2, '0');
                        }
                        return "";
                    }
                    
                    onTextChanged: {
                        if (text.length === 10) {
                            root.repeatStartDate = new Date(text);
                        }
                    }
                }
                
                // End condition
                ComboBox {
                    id: endTypeCombo
                    Kirigami.FormData.label: "End:"
                    
                    model: [
                        { text: "Never", value: "never" },
                        { text: "After", value: "after" },
                        { text: "On date", value: "on" }
                    ]
                    
                    textRole: "text"
                    valueRole: "value"
                    
                    currentIndex: {
                        for (let i = 0; i < model.length; i++) {
                            if (model[i].value === root.repeatEndType) {
                                return i;
                            }
                        }
                        return 0;
                    }
                    
                    onActivated: {
                        root.repeatEndType = model[currentIndex].value;
                    }
                }
                
                // End count (visible when "After" is selected)
                RowLayout {
                    visible: root.repeatEndType === "after"
                    Kirigami.FormData.label: "Occurrences:"
                    
                    SpinBox {
                        id: endCountSpinBox
                        from: 1
                        to: 999
                        value: root.repeatEndCount
                        onValueChanged: {
                            root.repeatEndCount = value;
                        }
                    }
                    
                    Label {
                        text: "times"
                        opacity: 0.7
                    }
                }
                
                // End date (visible when "On date" is selected)
                TextField {
                    id: endDateField
                    visible: root.repeatEndType === "on"
                    Kirigami.FormData.label: "End date:"
                    placeholderText: "YYYY-MM-DD"
                    inputMask: "9999-99-99"
                    text: {
                        if (root.repeatEndDate && root.repeatEndType === "on") {
                            let date = new Date(root.repeatEndDate);
                            return date.getFullYear() + "-" + 
                                   String(date.getMonth() + 1).padStart(2, '0') + "-" + 
                                   String(date.getDate()).padStart(2, '0');
                        }
                        return "";
                    }
                    
                    onTextChanged: {
                        if (text.length === 10) {
                            root.repeatEndDate = new Date(text);
                        }
                    }
                }
            }
            
            // Preview text
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: previewLabel.implicitHeight + Kirigami.Units.smallSpacing * 2
                color: Kirigami.Theme.alternateBackgroundColor
                radius: Kirigami.Units.cornerRadius
                
                Label {
                    id: previewLabel
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.smallSpacing
                    
                    text: {
                        if (!root.isRecurring) return "";
                        
                        let preview = "Repeats every ";
                        if (root.repeatFrequency > 1) {
                            preview += root.repeatFrequency + " ";
                        }
                        
                        switch (root.repeatInterval) {
                        case "days":
                            preview += root.repeatFrequency === 1 ? "day" : "days";
                            break;
                        case "weeks":
                            preview += root.repeatFrequency === 1 ? "week" : "weeks";
                            break;
                        case "months":
                            preview += root.repeatFrequency === 1 ? "month" : "months";
                            break;
                        case "years":
                            preview += root.repeatFrequency === 1 ? "year" : "years";
                            break;
                        }
                        
                        if (root.repeatEndType === "after") {
                            preview += ", " + root.repeatEndCount + " times";
                        } else if (root.repeatEndType === "on") {
                            preview += ", until " + Qt.formatDate(root.repeatEndDate, "MMM dd, yyyy");
                        }
                        
                        return preview;
                    }
                    
                    wrapMode: Text.WordWrap
                    opacity: 0.8
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                }
            }
        }
    }
}