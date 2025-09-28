import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

FormCard.FormCardDialog {
    id: repeatSettingDialog
    title: "Repeat Settings"

    readonly property IntValidator intValidator: IntValidator {}
    readonly property int maxValue: intValidator.top
    readonly property int minValue: intValidator.bottom
    required property var todoModel

    property var repeatConfig: {}
    property int taskId: -1
    property string repeatEndConfig: "never"
    property string repeatIntervalConfig: "days"

    property var monthDays: []

    standardButtons: Dialog.Save | Dialog.Cancel
    onAccepted: {
        updateRepeatConfig();
    }
    onRejected: {
        repeatSettingDialog.close();
    }
    
    function loadConfig(taskId, config) {
        this.taskId = taskId;
        repeatConfig = JSON.parse(config);
        
        // Set default values if properties are missing
        if (!repeatConfig.interval) {
            repeatConfig.interval = "days";
        }
        if (!repeatConfig.days) {
            repeatConfig.days = [];
        }
        if (!repeatConfig.end) {
            repeatConfig.end = "never";
        }
        
        // Update the UI to reflect the loaded config
        repeatIntervalConfig = repeatConfig.interval;
        monthDays = repeatConfig.days;
        repeatEndConfig = repeatConfig.end;
    }

    function setRepeatConfig(key, value) {
        // Ensure repeatConfig is initialized
        if (!repeatConfig) {
            repeatConfig = {
                interval: "days",
                days: [],
                end: "never"
            };
        }
        repeatConfig[key] = value;
        console.log('kumar', JSON.stringify(repeatConfig));

        repeatSettingDialog.loadConfig(taskId, JSON.stringify(repeatConfig));
    }

    function updateRepeatConfig() {
        console.log('kumar', JSON.stringify(repeatConfig));
        todoModel.updateTask(taskId,  {repeatConfig: JSON.stringify(repeatConfig)})
    }

    FormCard.FormCard {

        FormCard.AbstractFormDelegate {

            contentItem: ColumnLayout {
                Label {
                    text: "Repeat every"
                }
                RowLayout {
                    SpinBox {
                        id: frequencySpinBox
                        from: 1
                        to: repeatSettingDialog.maxValue
                        value: repeatSettingDialog.repeatConfig?.frequency || 1
                        onValueChanged: {
                            repeatSettingDialog.setRepeatConfig('frequency', frequencySpinBox.value);
                        }
                    }

                    ComboBox {
                        id: intervalComboBox
                        model: [
                            {
                                text: "day",
                                value: "days"
                            },
                            {
                                text: "week",
                                value: "weeks"
                            },
                            {
                                text: "month",
                                value: "months"
                            },
                            {
                                text: "year",
                                value: "years"
                            }
                        ]
                        currentIndex: {
                            const index = model.findIndex(item => item.value === repeatSettingDialog.repeatIntervalConfig);
                            return index >= 0 ? index : 0;
                        }
                        textRole: "text"
                        valueRole: "value"

                        onActivated: {
                            repeatSettingDialog.setRepeatConfig('interval', intervalComboBox.currentValue);
                        }
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            visible: repeatSettingDialog.repeatIntervalConfig === "weeks"
            contentItem: Components.SegmentedButton {
                actions: [
                    Kirigami.Action {
                        text: qsTr("S")

                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(0)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 0));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(0));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("M")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(1)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 1));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(1));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("T")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(2)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 2));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(2));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("W")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(3)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 3));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(3));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("T")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(4)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 4));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(4));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("F")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(5)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 5));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(5));
                            }
                        }
                    },
                    Kirigami.Action {
                        text: qsTr("S")
                        checkable: true
                        checked: repeatSettingDialog.repeatConfig?.days?.includes(6)
                        onTriggered: {
                            repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.filter(day => day !== 6));
                            if (checked) {
                                repeatSettingDialog.setRepeatConfig('days', repeatSettingDialog.repeatConfig.days.push(6));
                            }
                        }
                    }
                ]
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: dayOfMonthComboBox
            visible: repeatSettingDialog.repeatIntervalConfig === "months"
            model: monthDaysModel
            textRole: "text"
            valueRole: "value"
            currentIndex: {
                if (!repeatSettingDialog.repeatConfig?.day)
                    return 0;
                if (repeatSettingDialog.repeatConfig.day === "month_end")
                    return 0;
                return repeatSettingDialog.repeatConfig?.day || 0;
            }
            onActivated: repeatSettingDialog.setRepeatConfig('day', dayOfMonthComboBox.currentValue)

            ListModel {
                id: monthDaysModel

                ListElement {
                    text: "Month End"
                    value: "month_end"
                }
            }

            Component.onCompleted: {
                for (let i = 1; i <= 31; i++) {
                    monthDaysModel.append({
                        text: `Day ${i}`,
                        value: i
                    });
                }
            }
        }

        FormCard.FormDateTimeDelegate {
            dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Time
            onValueChanged: {
                repeatSettingDialog.setRepeatConfig('time', value);
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormDateTimeDelegate {
            text: "Start From"
            dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Date
            onValueChanged: {
                repeatSettingDialog.setRepeatConfig('start', value);
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormRadioSelectorDelegate {
            text: "Ends On"
            actions: [
                Kirigami.Action {
                    text: qsTr("Never")
                    onTriggered: {
                        repeatSettingDialog.setRepeatConfig('end', "never");
                        repeatSettingDialog.repeatEndConfig = "never";
                    }
                },
                Kirigami.Action {
                    text: qsTr("After")
                    onTriggered: {
                        repeatSettingDialog.setRepeatConfig('end', "after");
                        repeatSettingDialog.repeatEndConfig = "after";
                    }
                },
                Kirigami.Action {
                    text: qsTr("On date")
                    onTriggered: {
                        repeatSettingDialog.setRepeatConfig('end', "on");
                        repeatSettingDialog.repeatEndConfig = "on";
                    }
                }
            ]
        }

        FormCard.FormSpinBoxDelegate {
            visible: repeatSettingDialog.repeatEndConfig === "after"
            label: 'After'
            from: 1
            to: repeatSettingDialog.maxValue
            value: repeatSettingDialog.repeatConfig.after
            onValueChanged: {
                repeatSettingDialog.setRepeatConfig('after', value);
            }
        }

        FormCard.FormDateTimeDelegate {
            visible: repeatSettingDialog.repeatEndConfig === "on"
            text: "End On"
            dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Date
            onValueChanged: {
                repeatSettingDialog.setRepeatConfig('on', value);
            }
        }
    }
}
