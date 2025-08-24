#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
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
    TodoModel todoModel;
    ProjectModel projectModel;
    LabelModel labelModel;

    // Set database manager for all models
    todoModel.setDatabaseManager(&dbManager);
    projectModel.setDatabaseManager(&dbManager);
    labelModel.setDatabaseManager(&dbManager);

    // Create system tray manager
    SystemTrayManager systemTrayManager;
    systemTrayManager.setTodoModel(&todoModel);
    systemTrayManager.setDatabaseManager(&dbManager);

    QQmlApplicationEngine engine;

    // Register models with QML
    engine.rootContext()->setContextProperty(QStringLiteral("todoModel"), &todoModel);
    engine.rootContext()->setContextProperty(QStringLiteral("projectModel"), &projectModel);
    engine.rootContext()->setContextProperty(QStringLiteral("labelModel"), &labelModel);
    engine.rootContext()->setContextProperty(QStringLiteral("dbManager"), &dbManager);
    engine.rootContext()->setContextProperty(QStringLiteral("systemTrayManager"), &systemTrayManager);

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

            // Show welcome notification
            QTimer::singleShot(2000, [&systemTrayManager]() {
                systemTrayManager.showNotification(QStringLiteral("TodoApp Started"),
                                                   QStringLiteral("TodoApp is running in the system tray. Click the tray icon to show the window."),
                                                   5000);
            });
        }
    }

    return app.exec();
}
