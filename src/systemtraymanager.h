#pragma once

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QQmlEngine>
#include <QSystemTrayIcon>
#include <QTimer>

class TodoModel;
class DatabaseManager;

class SystemTrayManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool available READ isSystemTrayAvailable NOTIFY availabilityChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)

public:
    explicit SystemTrayManager(QObject *parent = nullptr);
    ~SystemTrayManager();

    bool isSystemTrayAvailable() const;
    bool isVisible() const;
    void setVisible(bool visible);

    void setTodoModel(TodoModel *model);

    // Invokable methods for QML
    Q_INVOKABLE void showNotification(const QString &title, const QString &message, int timeout = 5000);
    Q_INVOKABLE void showReminderNotification(const QString &taskContent, const QString &dueTime);
    Q_INVOKABLE void checkReminders();

Q_SIGNALS:
    void availabilityChanged();
    void visibilityChanged();
    void showMainWindow();
    void hideMainWindow();
    void quitApplication();
    void reminderTriggered(const QString &taskContent);

private Q_SLOTS:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowMainWindow();
    void onHideMainWindow();
    void onQuitApplication();
    void checkForDueReminders();

private:
    void createTrayIcon();
    void createContextMenu();
    void setupReminderTimer();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QAction *m_addTaskAction;
    QTimer *m_reminderTimer;

    TodoModel *m_todoModel;
    bool m_visible;
};