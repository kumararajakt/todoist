// #pragma once

// #include <QObject>
// #include <QQmlEngine>
// #include <KNotification>
// #include <QTimer>
// #include <QDateTime>

// class TodoModel;

// class NotificationManager : public QObject
// {
//     Q_OBJECT
//     QML_ELEMENT

//     Q_PROPERTY(TodoModel *todoModel READ todoModel WRITE setTodoModel NOTIFY todoModelChanged)

// public:
//     explicit NotificationManager(QObject *parent = nullptr);
//     ~NotificationManager();

//     void setTodoModel(TodoModel *model);
//     TodoModel *todoModel() const;

//     // Invokable methods for QML
//     Q_INVOKABLE void showTaskReminder(int taskId, const QString &taskContent, const QString &dueTime);
//     Q_INVOKABLE void showNotification(const QString &title, const QString &message, const QString &type = QStringLiteral("info"));

// public Q_SLOTS:
//     void onTaskCompleted(int taskId);
//     void onTaskSnoozed(int taskId, int minutes = 10);
//     void checkTaskReminders();

// Q_SIGNALS:
//     void todoModelChanged(TodoModel *model);
//     void taskCompletedFromNotification(int taskId);
//     void taskSnoozedFromNotification(int taskId, int minutes);

// private Q_SLOTS:
//     void onNotificationAction(const QString &actionName);

// private:
//     void setupNotificationActions();
//     KNotification* createTaskReminderNotification(int taskId, const QString &taskContent, const QString &dueTime);

//     TodoModel *m_todoModel;
//     QHash<KNotification*, int> m_notificationTaskMap;
//     QTimer *m_reminderTimer;
// };