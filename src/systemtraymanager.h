#pragma once

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QQmlEngine>
#include <QSystemTrayIcon>
#include <QTimer>

#include "duedate.h"
#include "todomodel.h"

class DatabaseManager;
class KNotification;

class SystemTrayManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(TodoModel *todoModel READ todoModel WRITE setTodoModel NOTIFY todoModelChanged)
    // Q_PROPERTY(NotificationManager *notificationManager READ notificationManager WRITE setNotificationManager NOTIFY notificationManagerChanged)

public:
    explicit SystemTrayManager(QObject *parent = nullptr);
    ~SystemTrayManager();

    void setTodoModel(TodoModel *model);
    TodoModel *todoModel() const;

    // void setNotificationManager(NotificationManager *manager);
    // NotificationManager *notificationManager() const;

    // Invokable methods for QML
    Q_INVOKABLE void showNotification(const QString &title, const QString &message, int timeout = 5000);
    Q_INVOKABLE void showReminderNotification(int taskIndex, const QString &taskContent, const QString &dueTime);
    Q_INVOKABLE void checkReminders();

public Q_SLOTS:
    void checkReminder(int taskId);

Q_SIGNALS:
    void showMainWindow();
    void hideMainWindow();
    void quitApplication();
    void todoModelChanged(TodoModel *model);
    // void notificationManagerChanged(NotificationManager *manager);

private Q_SLOTS:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowMainWindow();
    void onHideMainWindow();
    void onQuitApplication();
    void showReminder(int taskId);
    void onSnoozeTriggered();
    void onCompleteTriggered();

private:
    void createTrayIcon();
    void createContextMenu();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QAction *m_addTaskAction;

    TodoModel *m_todoModel;
    DueDate *m_dueDate;
    // NotificationManager *m_notificationManager;
    KNotification *m_notification;
};