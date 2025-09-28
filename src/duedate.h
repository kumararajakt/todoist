#pragma once

#include <QDateTime>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

class DueDate : public QObject
{
    Q_OBJECT

public:
    explicit DueDate(QObject *parent = nullptr);
    ~DueDate();

    void addTaskDueDate(int taskIndex, const QDateTime &dueDate);

Q_SIGNALS:
    void remindTrigger(int taskIndex);

private:
    QMap<int, QDateTime> m_snoozedTasks; // taskId -> snooze until time
    QMap<int, QDateTime> m_dueDates; // taskId -> due date

    QSet<int> m_notifiedTasks; // Track which tasks we've already notified about
};