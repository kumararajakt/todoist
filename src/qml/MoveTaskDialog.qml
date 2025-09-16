pragma ComponentBehavior: Bound

import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: moveTaskDialog

    property var task: null
    property int currentProjectId: -1

    signal taskMoved(var task, int newProjectId)

    required property var projectModel

    title: "Move Task to Project"
    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        if (projectComboBox.currentIndex >= 0) {
            let selectedProjectId = projectModel.data(projectModel.index(projectComboBox.currentIndex, 0), 256); // IdRole
            if (selectedProjectId !== currentProjectId) {
                taskMoved(task, selectedProjectId);
            }
        }
    }

    function loadTask(projectId) {
        currentProjectId = projectId;

        // Find and set current project in combo box
        for (let i = 0; i < projectModel.rowCount(); i++) {
            let projectId = projectModel.data(projectModel.index(i, 0), 256); // IdRole
            if (projectId === currentProjectId) {
                projectComboBox.currentIndex = i;
                break;
            }
        }
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Label {
            text: "Select project:"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ComboBox {
            id: projectComboBox
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 15

            model: moveTaskDialog.projectModel
            textRole: "name"
            valueRole: "id"
        }
    }
}
