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

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
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
                        addProjectDialog.open();
                    }

                    // ToolTip.delay: Kirigami.Units.toolTipDelay
                    // ToolTip.visible: hovered
                    // ToolTip.text: qsTr("Add Project")
                }
            }
        }

        Repeater {
            Layout.fillWidth: true

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
                text: modelData.name

                action: Kirigami.Action {
                    onTriggered: {
                        drawer.filterSelected(modelData.filter);
                        if (drawer.modal) {
                            drawer.close();
                        }
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
            model: projectModel
            Layout.fillHeight: true
            Layout.fillWidth: true

            delegate: PlaceItem {
                text: name
                onClicked: {
                    drawer.projectSelected(id);
                    if (drawer.modal) {
                        drawer.close();
                    }
                }
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
