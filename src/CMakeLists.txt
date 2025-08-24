qt_add_qml_module(todoist_static
    STATIC
    URI org.kde.todoist
    VERSION 1.0
    NO_CACHEGEN
    QML_FILES
    qml/Main.qml
    qml/TaskListView.qml
    qml/TaskItem.qml
    qml/TaskDetailDrawer.qml
    qml/ProjectSidebar.qml
    qml/AddTaskDialog.qml
    qml/AddProjectDialog.qml
    qml/TaskDetailDialog.qml
    qml/SystemTrayWidget.qml
    qml/NotificationOverlay.qml
    qml/SubtaskDialog.qml
    qml/RepeatSettingsCard.qml
)

target_sources(todoist_static PUBLIC
    main.cpp
    todomodel.cpp
    projectmodel.cpp
    labelmodel.cpp
    databasemanager.cpp
    task.cpp
    project.cpp
    label.cpp
    systemtraymanager.cpp

    todomodel.h
    projectmodel.h
    labelmodel.h
    databasemanager.h
    task.h
    project.h
    label.h
    systemtraymanager.h
)

target_link_libraries(todoist_static PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Quick
    Qt6::Svg
    Qt6::Sql
    Qt6::Network
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::Kirigami

)
target_include_directories(todoist_static PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(todoist_static PUBLIC Qt::Widgets)

add_executable(todoist main.cpp)
target_link_libraries(todoist PUBLIC
    todoist_static
    todoist_staticplugin
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    Qt6::Sql
)
install(TARGETS todoist ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
