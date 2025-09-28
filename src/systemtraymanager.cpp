#include "systemtraymanager.h"
#include "notificationmanager.h"
#include "todomodel.h"
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QSystemTrayIcon>
#include <knotification.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qnamespace.h>

#include <KNotification>

SystemTrayManager::SystemTrayManager(QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_showAction(nullptr)
    , m_hideAction(nullptr)
    , m_quitAction(nullptr)
    , m_addTaskAction(nullptr)
    , m_todoModel(nullptr)
    , m_dueDate(nullptr)
// , m_notificationManager(nullptr)
{
    m_dueDate = new DueDate(this);
    QObject::connect(m_dueDate, &DueDate::remindTrigger, this, &SystemTrayManager::checkReminder);
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        createTrayIcon();
        createContextMenu();
    }
    QString eventId = QStringLiteral("general");
    m_notification = new KNotification(eventId);
}

SystemTrayManager::~SystemTrayManager()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

void SystemTrayManager::setTodoModel(TodoModel *model)
{
    m_todoModel = model;
    Q_EMIT todoModelChanged(model);

    connect(model, &TodoModel::taskUpdated, this, &SystemTrayManager::checkReminder);
}

TodoModel *SystemTrayManager::todoModel() const
{
    return m_todoModel;
}

// void SystemTrayManager::setNotificationManager(NotificationManager *manager)
// {
//     if (m_notificationManager == manager)
//         return;

//     m_notificationManager = manager;
//     Q_EMIT notificationManagerChanged(manager);
// }

// NotificationManager *SystemTrayManager::notificationManager() const
// {
//     return m_notificationManager;
// }

void SystemTrayManager::showNotification(const QString &title, const QString &message, int timeout)
{
    if (m_trayIcon && QSystemTrayIcon::isSystemTrayAvailable()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, timeout);
    }
}

void SystemTrayManager::showReminderNotification(int taskIndex, const QString &taskContent, const QString &dueTime)
{
    QString title = QStringLiteral("Task Reminder");
    QString message = QStringLiteral("%1\n %2").arg(taskContent, dueTime);

    if (m_notification) {
        m_notification->setText(message);
        auto snoozeAction = m_notification->addAction(QStringLiteral("Snooze"));
        auto completeAction = m_notification->addAction(QStringLiteral("Complete"));

        m_notification->setProperty("taskIndex", taskIndex);

        // connect(m_notification, QOverload<unsigned int>::of(&KNotification::activ), this, &SystemTrayManager::onSnoozeTriggered);
        QObject::connect(snoozeAction, &KNotificationAction::activated, this, &SystemTrayManager::onSnoozeTriggered);
        QObject::connect(completeAction, &KNotificationAction::activated, this, &SystemTrayManager::onCompleteTriggered);
        m_notification->sendEvent();
    } else if (m_trayIcon && QSystemTrayIcon::isSystemTrayAvailable()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 10000);
    }
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        Q_EMIT showMainWindow();
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

void SystemTrayManager::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);

    QIcon icon = QIcon::fromTheme(QStringLiteral("org.kde.todoist"));
    if (icon.isNull()) {
        QPixmap pixmap(16, 16);
        pixmap.fill(Qt::blue);
        icon = QIcon(pixmap);
    }
    m_trayIcon->setIcon(icon);

    m_trayIcon->setToolTip(QStringLiteral("TodoApp - Task Manager"));

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayManager::onTrayIconActivated);
    connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, &SystemTrayManager::onShowMainWindow);

    m_trayIcon->show();
}

void SystemTrayManager::createContextMenu()
{
    if (!m_trayIcon) {
        return;
    }

    m_trayMenu = new QMenu();

    m_quitAction = new QAction(QStringLiteral("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, this, &SystemTrayManager::onQuitApplication);

    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
}

void SystemTrayManager::checkReminders()
{
    if (!m_todoModel) {
        return;
    }

    // Get tasks that are due within the next hour and not completed
    for (int i = 0; i < m_todoModel->rowCount(); i++) {
        auto dueDateStr = m_todoModel->data(m_todoModel->index(i, 0), TodoModel::DueDateRole).toString();
        auto isCompleted = m_todoModel->data(m_todoModel->index(i, 0), TodoModel::IsCompletedRole).toBool();
        auto content = m_todoModel->data(m_todoModel->index(i, 0), TodoModel::ContentRole).toString();
        auto dueDate = QDateTime::fromString(dueDateStr, Qt::ISODateWithMs);

        if (dueDateStr.trimmed().isEmpty() || !dueDate.isValid() || isCompleted) {
            continue;
        }

        QDateTime now = QDateTime::currentDateTime();

        if (now >= dueDate) {
            QString timeString = dueDate.toString(QStringLiteral("MMMM dd, yyyy hh:mm AP"));
            showReminderNotification(i, content, timeString);
        } else
            m_dueDate->addTaskDueDate(i, dueDate);
    }
}

void SystemTrayManager::checkReminder(int taskIndex)
{
    if (!m_todoModel) {
        return;
    }

    auto dueDateStr = m_todoModel->data(m_todoModel->index(taskIndex, 0), TodoModel::DueDateRole).toString();
    auto isCompleted = m_todoModel->data(m_todoModel->index(taskIndex, 0), TodoModel::IsCompletedRole).toBool();
    auto dueDate = QDateTime::fromString(dueDateStr, Qt::ISODateWithMs);
    auto content = m_todoModel->data(m_todoModel->index(taskIndex, 0), TodoModel::ContentRole).toString();

    if (!dueDate.isValid() || isCompleted) {
        return;
    }
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "Checking reminder for task index" << taskIndex << "dueDate" << dueDate << "isCompleted" << isCompleted << "now" << now << (now >= dueDate);

    if (now >= dueDate) {
        QString timeString = dueDate.toString(QStringLiteral("MMMM dd, yyyy hh:mm AP"));
        showReminderNotification(taskIndex, content, timeString);
    } else
        m_dueDate->addTaskDueDate(taskIndex, dueDate);
}

void SystemTrayManager::showReminder(int taskIndex)
{
    auto dueDateStr = m_todoModel->data(m_todoModel->index(taskIndex, 0), TodoModel::DueDateRole).toString();

    auto content = m_todoModel->data(m_todoModel->index(taskIndex, 0), TodoModel::ContentRole).toString();
    this->showReminderNotification(taskIndex, content, dueDateStr);
}

void SystemTrayManager::onSnoozeTriggered()
{
    if (!m_notification || !m_todoModel) {
        return;
    }

    // Close the current notification
    m_notification->close();

    // Get the task ID from the notification
    int taskIndex = m_notification->property("taskIndex").toInt();

    if (taskIndex >= 0 && taskIndex < m_todoModel->rowCount()) {
        // Update the task's due date to be 10 minutes from now
        const QModelIndex index = m_todoModel->index(taskIndex, 0);
        const QDateTime now = QDateTime::currentDateTime();
        const QDateTime dueDate = now.addSecs(600);

        // Update the due date in the model
        m_todoModel->setData(index, dueDate.toString(Qt::ISODate), TodoModel::DueDateRole);

        // Re-add the reminder
        m_dueDate->addTaskDueDate(taskIndex, dueDate);
        qDebug() << "Snoozed task" << taskIndex << "until" << dueDate.toString();
    }
}

void SystemTrayManager::onCompleteTriggered()
{
    if (!m_notification || !m_todoModel) {
        return;
    }

    // Close the current notification
    m_notification->close();

    // Get the task ID from the notification
    int taskIndex = m_notification->property("taskIndex").toInt();

    if (taskIndex >= 0 && taskIndex < m_todoModel->rowCount()) {
        // Mark the task as complete in the model
        const QModelIndex index = m_todoModel->index(taskIndex, 0);
        m_todoModel->setData(index, true, TodoModel::IsCompletedRole);

        // Note: The DueDate class doesn't have a removeTaskDueDate method,
        // but the task will be filtered out in the model when marked as complete
        qDebug() << "Marked task" << taskIndex << "as complete";
    }
}