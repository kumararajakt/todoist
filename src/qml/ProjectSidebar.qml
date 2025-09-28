pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.OverlayDrawer {
    id: drawer

    signal projectSelected(int projectId)
    signal filterSelected(string filter)
    required property var projectModel
    required property var addProjectDialog
    property int lastSelectedProjectId: -1
    property int selectedProjectIndex: lastSelectedProjectId

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    handleClosedIcon.source: null
    handleOpenIcon.source: null

    contentItem: ColumnLayout {
        width: parent.width
        height: parent.height
        spacing: 0

        Kirigami.AbstractApplicationHeader {
            id: sidebarHeader
            topPadding: Kirigami.Units.smallSpacing
            bottomPadding: Kirigami.Units.smallSpacing
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            contentItem: RowLayout {
                Layout.fillWidth: true
                width: parent.width

                Kirigami.Heading {
                    text: 'Projects'
                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillWidth: true
                }

                Controls.ToolButton {
                    id: addButton
                    visible: !Kirigami.Settings.isMobile
                    display: Controls.AbstractButton.IconOnly
                    icon.name: "list-add-symbolic"

                    onClicked: {
                        drawer.addProjectDialog.open();
                    }

                    // ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: qsTr("Add Project")
                }
            }
        }

        ListView {
            id: projectListView
            model: drawer.projectModel
            Layout.fillHeight: true
            Layout.fillWidth: true

            delegate: PlaceItem {
                required property string name
                required property int id
                required property int index
                highlighted: drawer.selectedProjectIndex === index

                text: name
                onClicked: {
                    drawer.lastSelectedProjectId = id;
                    drawer.selectedProjectIndex = index;
                    drawer.projectSelected(id);
                    if (drawer.modal) {
                        drawer.close();
                    }
                }
            }
        }

        Component.onCompleted: {
            if (drawer.lastSelectedProjectId !== -1) {
                drawer.projectSelected(drawer.lastSelectedProjectId);
            }
        }
    }

    component PlaceItem: Delegates.RoundedItemDelegate {
        id: item

        Layout.fillWidth: true
        Keys.onDownPressed: nextItemInFocusChain().forceActiveFocus(Qt.TabFocusReason)
        Keys.onUpPressed: nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocusReason)
        Accessible.role: Accessible.MenuItem
    }

    component PlaceHeading: Kirigami.ListSectionHeader {
        Layout.fillWidth: true
    }
}
