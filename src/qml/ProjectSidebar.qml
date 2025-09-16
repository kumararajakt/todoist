pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.OverlayDrawer {
    id: drawer

    modal: false
    signal projectSelected(int projectId)
    signal filterSelected(string filter)
    required property var projectModel
    required property var addProjectDialog

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
            id: projectCustomList
            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight

            model: [
                {
                    name: "Today",
                    filter: "today",
                    icon: "view-calendar-day"
                },
                {
                    name: "Upcoming",
                    filter: "upcoming",
                    icon: "view-calendar-upcoming-days"
                },
                {
                    name: "Completed",
                    filter: "completed",
                    icon: "task-complete"
                }
            ]

            delegate: PlaceItem {
                id: projectCustomListDelegate
                // required property string name
                required property string name
                required property string filter
                required property int index

                text: name

                onClicked: {
                    drawer.filterSelected(filter);
                    projectCustomList.currentIndex = index;
                    projectListView.currentIndex = -1;
                    if (drawer.modal) {
                        drawer.close();
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            PlaceHeading {
                text: "Projects"
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

                text: name
                onClicked: {
                    projectCustomList.currentIndex = -1;
                    projectListView.currentIndex = index;
                    drawer.projectSelected(id);
                    if (drawer.modal) {
                        drawer.close();
                    }
                }
            }
        }

        Component.onCompleted: {
            drawer.filterSelected("today");
            projectListView.currentIndex = -1;
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
