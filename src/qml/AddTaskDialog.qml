import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.todoist 1.0
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: dialog

    signal taskAdded

    title: "Add New Task"
    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        if (contentField.text.trim().length > 0) {
            let dueDate = dueDateField.checked ? new Date(dueDateField.text) : new Date(0);

            todoModel.addTask({
                content: contentField.text.trim(),
                description: descriptionField.text.trim(),
                dueDate: dueDate,
                priority: priorityCombo.currentValue
            });

            taskAdded();
            clearFields();
        }
    }

    onRejected: {
        clearFields();
    }

    function clearFields() {
        contentField.text = "";
        descriptionField.text = "";
        dueDateField.checked = false;
        priorityCombo.currentIndex = 0;
        repeatSettings.reset();
    }

    FormCard.FormCard {

        FormCard.FormTextFieldDelegate {
            id: contentField
            label: "Task:"
            placeholderText: "What needs to be done?"
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextAreaDelegate {
            id: descriptionField
            label: "Description"
            placeholderText: "Add details..."
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: priorityCombo
            text: "Priority:"

            model: [
                {
                    text: "Low",
                    value: 1
                },
                {
                    text: "Medium",
                    value: 2
                },
                {
                    text: "Medium-High",
                    value: 3
                },
                {
                    text: "High",
                    value: 4
                }
            ]
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormDateTimeDelegate {
            id: dueDateField
            text: "Due Date:"
            dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Date
        }
    }
}
