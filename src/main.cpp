#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>
#include <qobject.h>

#include "appsettings.h"
#include "databasemanager.h"
#include "duedate.h"
#include "labelmodel.h"
#include "notificationmanager.h"
#include "projectmodel.h"
#include "systemtraymanager.h"
#include "todomodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(QStringLiteral("TodoApp"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("TodoApp"));
    app.setDesktopFileName(QStringLiteral("org.kde.todoist"));

    // Initialize database
    DatabaseManager dbManager;
    if (!dbManager.initialize()) {
        qFatal("Failed to initialize database");
        return -1;
    }

    QQmlApplicationEngine engine;

    qmlRegisterType<TodoModel>("org.kde.todoist", 1, 0, "TodoModel");
    qmlRegisterType<ProjectModel>("org.kde.todoist", 1, 0, "ProjectModel");
    qmlRegisterType<SystemTrayManager>("org.kde.todoist", 1, 0, "SystemTrayManager");
    // qmlRegisterType<NotificationManager>("org.kde.todoist", 1, 0, "NotificationManager");
    qmlRegisterType<AppSettings>("org.kde.todoist", 1, 0, "AppSettings");

    engine.loadFromModule("org.kde.todoist", u"Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
