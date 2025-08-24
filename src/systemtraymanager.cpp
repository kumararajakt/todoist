#include "systemtraymanager.h"
#include "databasemanager.h"
#include "task.h"
#include "todomodel.h"
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QObject>

SystemTrayManager::SystemTrayManager(QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_showAction(nullptr)
    , m_hideAction(nullptr)
    , m_quitAction(nullptr)
    , m_addTaskAction(nullptr)
    , m_reminderTimer(nullptr)
    , m_todoModel(nullptr)
    , m_dbManager(nullptr)
    , m_visible(true)
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        createTrayIcon();
        createContextMenu();
        setupReminderTimer();
    }
}

SystemTrayManager::~SystemTrayManager()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

bool SystemTrayManager::isSystemTrayAvailable() const
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

bool SystemTrayManager::isVisible() const
{
    return m_visible;
}

void SystemTrayManager::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        if (m_trayIcon) {
            m_trayIcon->setVisible(visible);
        }
        Q_EMIT visibilityChanged();
    }
}

void SystemTrayManager::setTodoModel(TodoModel *model)
{
    m_todoModel = model;
}

void SystemTrayManager::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;
}

void SystemTrayManager::showNotification(const QString &title, const QString &message, int timeout)
{
    if (m_trayIcon && isSystemTrayAvailable()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, timeout);
    }
}

void SystemTrayManager::showReminderNotification(const QString &taskContent, const QString &dueTime)
{
    QString title = QStringLiteral("Task Reminder");
    QString message = QStringLiteral("Due %1: %2").arg(dueTime, taskContent);

    if (m_trayIcon && isSystemTrayAvailable()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Warning, 10000);
    }

    Q_EMIT reminderTriggered(taskContent);
}

void SystemTrayManager::checkReminders()
{
    checkForDueReminders();
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        Q_EMIT showMainWindow();
        break;
    case QSystemTrayIcon::MiddleClick:
        showNotification(QStringLiteral("TodoApp"), QStringLiteral("Middle click detected"));
        break;
    default:
        break;
    }
}

void SystemTrayManager::onShowMainWindow()
{
    Q_EMIT showMainWindow();
}

void SystemTrayManager::onHideMainWindow()
{
    Q_EMIT hideMainWindow();
}

void SystemTrayManager::onQuitApplication()
{
    Q_EMIT quitApplication();
}

void SystemTrayManager::checkForDueReminders()
{
    if (!m_dbManager) {
        return;
    }

    // Get tasks that are due within the next hour and not completed
    QDateTime now = QDateTime::currentDateTime();
    QDateTime oneHourLater = now.addSecs(3600);

    // This would need to be implemented in DatabaseManager
    // For now, we'll use a simple approach
    auto tasks = m_dbManager->getTasks(); // Get all tasks

    for (Task *task : tasks) {
        if (task && !task->isCompleted() && task->dueDate().isValid()) {
            QDateTime dueDate = task->dueDate();

            // Check if task is due within the next hour
            if (dueDate >= now && dueDate <= oneHourLater) {
                QString timeString = dueDate.toString(QStringLiteral("hh:mm"));
                showReminderNotification(task->content(), timeString);
            }

            // Check if task is overdue
            else if (dueDate < now) {
                QString overdueString = QStringLiteral("overdue");
                showReminderNotification(task->content(), overdueString);
            }
        }
    }
}

void SystemTrayManager::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);

    // Set icon - you might want to add a proper icon file
    QIcon icon = QIcon::fromTheme(QStringLiteral("task-manager"));
    if (icon.isNull()) {
        // Fallback to a simple colored icon
        QPixmap pixmap(16, 16);
        pixmap.fill(Qt::blue);
        icon = QIcon(pixmap);
    }
    m_trayIcon->setIcon(icon);

    m_trayIcon->setToolTip(QStringLiteral("TodoApp - Task Manager"));

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayManager::onTrayIconActivated);

    connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, &SystemTrayManager::onShowMainWindow);
}

void SystemTrayManager::createContextMenu()
{
    if (!m_trayIcon) {
        return;
    }

    m_trayMenu = new QMenu();

    m_showAction = new QAction(QStringLiteral("&Show TodoApp"), this);
    connect(m_showAction, &QAction::triggered, this, &SystemTrayManager::onShowMainWindow);

    m_hideAction = new QAction(QStringLiteral("&Hide TodoApp"), this);
    connect(m_hideAction, &QAction::triggered, this, &SystemTrayManager::onHideMainWindow);

    m_addTaskAction = new QAction(QStringLiteral("&Add Task"), this);
    connect(m_addTaskAction, &QAction::triggered, this, &SystemTrayManager::onShowMainWindow);

    m_quitAction = new QAction(QStringLiteral("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, this, &SystemTrayManager::onQuitApplication);

    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addAction(m_hideAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_addTaskAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
}

void SystemTrayManager::setupReminderTimer()
{
    m_reminderTimer = new QTimer(this);
    connect(m_reminderTimer, &QTimer::timeout, this, &SystemTrayManager::checkForDueReminders);

    // Check for reminders every 5 minutes
    m_reminderTimer->start(5 * 60 * 1000);

    // Also check immediately
    QTimer::singleShot(1000, this, &SystemTrayManager::checkForDueReminders);
}