pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

GridLayout {
    id: root

    property date selectedDate: new Date()
    property date currentDate: new Date()

    columns: 7
    rowSpacing: 5
    columnSpacing: 5

    property int currentMonth: currentDate.getMonth()
    property int currentYear: currentDate.getFullYear()

    // Month/Year header with navigation
    RowLayout {
        id: headerRow
        Layout.columnSpan: 7
        Layout.fillWidth: true

        ToolButton {
            text: "<"
            onClicked: {
                let date = new Date(root.currentDate);
                date.setMonth(date.getMonth() - 1);
                root.currentDate = date;
            }
        }

        Label {
            text: Qt.formatDate(root.currentDate, "MMMM yyyy")
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        ToolButton {
            text: ">"
            onClicked: {
                let date = new Date(root.currentDate);
                date.setMonth(date.getMonth() + 1);
                root.currentDate = date;
            }
        }
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
                let firstDay = new Date(root.currentYear, root.currentMonth, 1);
                let startDate = new Date(firstDay);
                startDate.setDate(startDate.getDate() - firstDay.getDay());
                let result = new Date(startDate);
                result.setDate(result.getDate() + index);
                return result;
            }

            text: buttonDate.getDate()
            Layout.preferredWidth: 35
            Layout.preferredHeight: 35

            enabled: buttonDate.getMonth() === root.currentMonth
            highlighted: buttonDate.getDate() === root.selectedDate.getDate() && buttonDate.getMonth() === root.selectedDate.getMonth() && buttonDate.getFullYear() === root.selectedDate.getFullYear()

            onClicked: {
                root.selectedDate = buttonDate;
            }
        }
    }

    function goToToday() {
        const today = new Date();
        root.selectedDate = today;
        currentDate = today;
    }
}
