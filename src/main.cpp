#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>

#include "databasemanager.h"
#include "labelmodel.h"
#include "projectmodel.h"
#include "systemtraymanager.h"
#include "todomodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(QStringLiteral("TodoApp"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("TodoApp"));

    // Initialize database
    DatabaseManager dbManager;
    if (!dbManager.initialize()) {
        qFatal("Failed to initialize database");
        return -1;
    }

    // Create models
    TodoModel todoModel(nullptr, dbManager.database());
    // ProjectModel projectModel(nullptr, dbManager.database());
    LabelModel labelModel;

    // Initialize models and set database manager for LabelModel
    todoModel.initializeDatabase();
    // projectModel.initializeDatabase();
    labelModel.setDatabaseManager(&dbManager);

    // Create system tray manager
    SystemTrayManager systemTrayManager;
    systemTrayManager.setTodoModel(&todoModel);

    QQmlApplicationEngine engine;

    // Register QML types
    qmlRegisterType<TodoModel>("org.kde.todoist", 1, 0, "TodoModel");
    qmlRegisterType<ProjectModel>("org.kde.todoist", 1, 0, "ProjectModel");
    qmlRegisterType<LabelModel>("org.kde.todoist", 1, 0, "LabelModel");
    qmlRegisterType<DatabaseManager>("org.kde.todoist", 1, 0, "DatabaseManager");
    qmlRegisterType<SystemTrayManager>("org.kde.todoist", 1, 0, "SystemTrayManager");

    // Set context properties for the models
    // engine.rootContext()->setContextProperty(QStringLiteral("todoModel"), &todoModel);
    // engine.rootContext()->setContextProperty(QStringLiteral("projectModel"), &projectModel);
    // engine.rootContext()->setContextProperty(QStringLiteral("labelModel"), &labelModel);
    // engine.rootContext()->setContextProperty(QStringLiteral("dbManager"), &dbManager);
    // engine.rootContext()->setContextProperty(QStringLiteral("systemTrayManager"), &systemTrayManager);

    engine.loadFromModule("org.kde.todoist", u"Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    // Get the main window
    QObject *mainWindow = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(mainWindow);

    // Connect system tray signals to window visibility
    if (window) {
        QObject::connect(&systemTrayManager, &SystemTrayManager::showMainWindow, [window]() {
            window->show();
            window->raise();
            window->requestActivate();
        });

        QObject::connect(&systemTrayManager, &SystemTrayManager::hideMainWindow, [window]() {
            window->hide();
        });

        QObject::connect(&systemTrayManager, &SystemTrayManager::quitApplication, &app, &QGuiApplication::quit);

        // Enable system tray if available
        if (systemTrayManager.isSystemTrayAvailable()) {
            systemTrayManager.setVisible(true);
        }
    }

    return app.exec();
}
