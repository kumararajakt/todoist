pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: root
    
    property date selectedDate: new Date()
    
    signal dateSelected(date selectedDate)
    
    padding: Kirigami.Units.largeSpacing
    
    contentItem: ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing
        
        DatePicker {
            id: datePicker
            Layout.fillWidth: true
            Layout.fillHeight: true
            selectedDate: root.selectedDate
            onSelectedDateChanged: root.selectedDate = selectedDate
        }
        
        Button {
            text: "Today"
            Layout.fillWidth: true
            onClicked: {
                datePicker.goToToday()
                root.selectedDate = datePicker.selectedDate
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Cancel"
                Layout.fillWidth: true
                onClicked: root.close()
            }
            
            Button {
                text: "Select"
                Layout.fillWidth: true
                onClicked: {
                    root.dateSelected(datePicker.selectedDate)
                    root.close()
                }
            }
        }
    }
    
    function openWithDate(date) {
        selectedDate = date
        datePicker.selectedDate = date
        datePicker.currentDate = date
        open()
    }
}
