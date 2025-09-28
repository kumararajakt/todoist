// #include "notificationmanager.h"
// #include "todomodel.h"
// #include <KLocalizedString>
// #include <KNotification>
// #include <QApplication>
// #include <QDateTime>
// #include <QDebug>
// #include <knotification.h>
// #include <qcontainerfwd.h>

// NotificationManager::NotificationManager(QObject *parent)
//     : QObject(parent)
//     , m_todoModel(nullptr)
//     , m_reminderTimer(new QTimer(this))
// {
//     setupNotificationActions();

//     // Check for reminders every minute
//     m_reminderTimer->setInterval(60000); // 60 seconds
//     m_reminderTimer->setSingleShot(false);
//     connect(m_reminderTimer, &QTimer::timeout, this, &NotificationManager::checkTaskReminders);
//     m_reminderTimer->start();
// }

// NotificationManager::~NotificationManager()
// {
//     // Clean up any active notifications
//     for (auto it = m_notificationTaskMap.begin(); it != m_notificationTaskMap.end(); ++it) {
//         it.key()->close();
//     }
// }

// void NotificationManager::setTodoModel(TodoModel *model)
// {
//     if (m_todoModel == model)
//         return;

//     m_todoModel = model;
//     Q_EMIT todoModelChanged(model);
// }

// TodoModel *NotificationManager::todoModel() const
// {
//     return m_todoModel;
// }

// void NotificationManager::setupNotificationActions()
// {
//     // These actions will be available for task reminder notifications
//     // KNotification will automatically handle the action buttons
// }

// void NotificationManager::showTaskReminder(int taskId, const QString &taskContent, const QString &dueTime)
// {
//     KNotification *notification = createTaskReminderNotification(taskId, taskContent, dueTime);

//     // Store the mapping between notification and task ID
//     m_notificationTaskMap[notification] = taskId;

//     // Connect to notification actions
//     // connect(notification, &KNotification::action1Activated, this, [this, taskId]() {
//     //     onTaskCompleted(taskId);
//     // });

//     // connect(notification, &KNotification::action2Activated, this, [this, taskId]() {
//     //     onTaskSnoozed(taskId, 10); // Snooze for 10 minutes
//     // });

//     // connect(notification, &KNotification::action3Activated, this, [this, taskId]() {
//     //     onTaskSnoozed(taskId, 60); // Snooze for 1 hour
//     // });

//     // Clean up when notification is closed
//     connect(notification, &KNotification::closed, this, [this, notification]() {
//         m_notificationTaskMap.remove(notification);
//     });

//     notification->sendEvent();
// }

// void NotificationManager::showNotification(const QString &title, const QString &message, const QString &type)
// {
//     KNotification *notification = new KNotification(QStringLiteral("general"));
//     notification->setTitle(title);
//     notification->setText(message);

//     // Set icon based on type
//     if (type == QStringLiteral("warning")) {
//         notification->setIconName(QStringLiteral("dialog-warning"));
//     } else if (type == QStringLiteral("error")) {
//         notification->setIconName(QStringLiteral("dialog-error"));
//     } else if (type == QStringLiteral("success")) {
//         notification->setIconName(QStringLiteral("dialog-ok-apply"));
//     } else {
//         notification->setIconName(QStringLiteral("dialog-information"));
//     }

//     notification->sendEvent();
// }

// KNotification *NotificationManager::createTaskReminderNotification(int taskId, const QString &taskContent, const QString &dueTime)
// {
//     KNotification *notification = new KNotification(QStringLiteral("general"));

//     notification->setTitle(QStringLiteral("Task Reminder"));
//     // notification->setText(QString("%1\nDue: %2").arg(taskContent, dueTime));
//     notification->setIconName(QStringLiteral("view-task"));

//     // Add action buttons
//     // notification->setActions(QStringList() << QStringLiteral("Complete Task")
//     //                                        << QStringLiteral("Snooze 10min")
//     //                                        << QStringLiteral("Snooze 1hour"));

//     // Set notification to stay persistent until user interacts
//     notification->setFlags(KNotification::Persistent);

//     return notification;
// }

// void NotificationManager::onTaskCompleted(int taskId)
// {
//     qDebug() << "Task completed from notification:" << taskId;

//     if (m_todoModel) {
//         // Find the task index and mark it as completed
//         for (int i = 0; i < m_todoModel->rowCount(); ++i) {
//             QModelIndex index = m_todoModel->index(i, 0);
//             int id = m_todoModel->data(index, TodoModel::IdRole).toInt();
//             if (id == taskId) {
//                 // Mark task as completed
//                 m_todoModel->setData(index, true, TodoModel::IsCompletedRole);
//                 break;
//             }
//         }
//     }

//     Q_EMIT taskCompletedFromNotification(taskId);

//     // Close any related notifications
//     for (auto it = m_notificationTaskMap.begin(); it != m_notificationTaskMap.end(); ++it) {
//         if (it.value() == taskId) {
//             it.key()->close();
//             m_notificationTaskMap.erase(it);
//             break;
//         }
//     }
// }

// void NotificationManager::onTaskSnoozed(int taskId, int minutes)
// {
//     qDebug() << "Task snoozed from notification:" << taskId << "for" << minutes << "minutes";

//     if (m_todoModel) {
//         // Update the due date to snooze the task
//         for (int i = 0; i < m_todoModel->rowCount(); ++i) {
//             QModelIndex index = m_todoModel->index(i, 0);
//             int id = m_todoModel->data(index, TodoModel::IdRole).toInt();
//             if (id == taskId) {
//                 // Get current due date and add snooze time
//                 QString dueDateStr = m_todoModel->data(index, TodoModel::DueDateRole).toString();
//                 QDateTime dueDate = QDateTime::fromString(dueDateStr, Qt::ISODateWithMs);

//                 if (dueDate.isValid()) {
//                     QDateTime newDueDate = dueDate.addSecs(minutes * 60);
//                     m_todoModel->setData(index, newDueDate.toString(Qt::ISODateWithMs), TodoModel::DueDateRole);
//                 } else {
//                     // If no due date, set it to current time + snooze
//                     QDateTime newDueDate = QDateTime::currentDateTime().addSecs(minutes * 60);
//                     m_todoModel->setData(index, newDueDate.toString(Qt::ISODateWithMs), TodoModel::DueDateRole);
//                 }
//                 break;
//             }
//         }
//     }

//     Q_EMIT taskSnoozedFromNotification(taskId, minutes);

//     // Close any related notifications
//     for (auto it = m_notificationTaskMap.begin(); it != m_notificationTaskMap.end(); ++it) {
//         if (it.value() == taskId) {
//             it.key()->close();
//             m_notificationTaskMap.erase(it);
//             break;
//         }
//     }

//     // Show confirmation
//     // showNotification("Task Snoozed", QString("Task snoozed for %1 minutes").arg(minutes), "info");
// }

// void NotificationManager::checkTaskReminders()
// {
//     if (!m_todoModel) {
//         return;
//     }

//     QDateTime now = QDateTime::currentDateTime();

//     // Check all tasks for due reminders
//     for (int i = 0; i < m_todoModel->rowCount(); ++i) {
//         QModelIndex index = m_todoModel->index(i, 0);

//         QString dueDateStr = m_todoModel->data(index, TodoModel::DueDateRole).toString();
//         bool isCompleted = m_todoModel->data(index, TodoModel::IsCompletedRole).toBool();
//         QString content = m_todoModel->data(index, TodoModel::ContentRole).toString();
//         int taskId = m_todoModel->data(index, TodoModel::IdRole).toInt();

//         if (dueDateStr.trimmed().isEmpty() || isCompleted) {
//             continue;
//         }

//         QDateTime dueDate = QDateTime::fromString(dueDateStr, Qt::ISODateWithMs);
//         if (!dueDate.isValid()) {
//             continue;
//         }

//         // Check if task is due (current time >= due time)
//         if (now >= dueDate) {
//             // Check if we already have a notification for this task
//             bool hasActiveNotification = false;
//             for (auto it = m_notificationTaskMap.begin(); it != m_notificationTaskMap.end(); ++it) {
//                 if (it.value() == taskId) {
//                     hasActiveNotification = true;
//                     break;
//                 }
//             }

//             if (!hasActiveNotification) {
//                 // QString timeString = dueDate.toString("MMMM dd, yyyy hh:mm AP");
//                 // showTaskReminder(taskId, content, timeString);
//             }
//         }
//     }
// }

// void NotificationManager::onNotificationAction(const QString &actionName)
// {
//     // This method can be used for additional action handling if needed
//     qDebug() << "Notification action triggered:" << actionName;
// }