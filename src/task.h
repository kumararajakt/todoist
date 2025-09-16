#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QStringList>
#include <qobject.h>
#include <qstringliteral.h>

class Task : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int projectId READ projectId WRITE setProjectId NOTIFY projectIdChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime dueDate READ dueDate WRITE setDueDate NOTIFY dueDateChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(bool isCompleted READ isCompleted WRITE setIsCompleted NOTIFY isCompletedChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(QStringList labels READ labels WRITE setLabels NOTIFY labelsChanged)
    Q_PROPERTY(int parentTaskId READ parentTaskId WRITE setParentTaskId NOTIFY parentTaskIdChanged)
    Q_PROPERTY(bool hasSubtasks READ hasSubtasks NOTIFY hasSubtasksChanged)
    Q_PROPERTY(int subtaskCount READ subtaskCount NOTIFY subtaskCountChanged)
    Q_PROPERTY(int completedSubtaskCount READ completedSubtaskCount NOTIFY completedSubtaskCountChanged)
    Q_PROPERTY(bool isRecurring READ isRecurring WRITE setIsRecurring NOTIFY isRecurringChanged)
    Q_PROPERTY(QString repeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY repeatIntervalChanged)
    Q_PROPERTY(int repeatFrequency READ repeatFrequency WRITE setRepeatFrequency NOTIFY repeatFrequencyChanged)
    Q_PROPERTY(QDateTime repeatStartDate READ repeatStartDate WRITE setRepeatStartDate NOTIFY repeatStartDateChanged)
    Q_PROPERTY(QString repeatEndType READ repeatEndType WRITE setRepeatEndType NOTIFY repeatEndTypeChanged)
    Q_PROPERTY(QDateTime repeatEndDate READ repeatEndDate WRITE setRepeatEndDate NOTIFY repeatEndDateChanged)
    Q_PROPERTY(int repeatEndCount READ repeatEndCount WRITE setRepeatEndCount NOTIFY repeatEndCountChanged)
    Q_PROPERTY(int currentRepeatCount READ currentRepeatCount WRITE setCurrentRepeatCount NOTIFY currentRepeatCountChanged)
    Q_PROPERTY(int pomodoroCount READ pomodoroCount WRITE setPomodoroCount NOTIFY pomodoroCountChanged)
    Q_PROPERTY(int pomodoroTarget READ pomodoroTarget WRITE setPomodoroTarget NOTIFY pomodoroTargetChanged)
    Q_PROPERTY(int pomodoroLength READ pomodoroLength WRITE setPomodoroLength NOTIFY pomodoroLengthChanged)
    Q_PROPERTY(int shortBreakLength READ shortBreakLength WRITE setShortBreakLength NOTIFY shortBreakLengthChanged)
    Q_PROPERTY(int longBreakLength READ longBreakLength WRITE setLongBreakLength NOTIFY longBreakLengthChanged)
    Q_PROPERTY(bool pomodoroEnabled READ pomodoroEnabled WRITE setPomodoroEnabled NOTIFY pomodoroEnabledChanged)
    Q_PROPERTY(bool pomodoroActive READ pomodoroActive WRITE setPomodoroActive NOTIFY pomodoroActiveChanged)

    // Time tracking properties
    Q_PROPERTY(int totalTimeSpent READ totalTimeSpent WRITE setTotalTimeSpent NOTIFY totalTimeSpentChanged)
    Q_PROPERTY(bool timeTrackingActive READ timeTrackingActive WRITE setTimeTrackingActive NOTIFY timeTrackingActiveChanged)
    Q_PROPERTY(QDateTime timeTrackingStarted READ timeTrackingStarted WRITE setTimeTrackingStarted NOTIFY timeTrackingStartedChanged)

public:
    explicit Task(QObject *parent = nullptr);

    int id() const
    {
        return m_id;
    }
    void setId(int id);

    int projectId() const
    {
        return m_projectId;
    }
    void setProjectId(int projectId);

    QString content() const
    {
        return m_content;
    }
    void setContent(const QString &content);

    QString description() const
    {
        return m_description;
    }
    void setDescription(const QString &description);

    QDateTime dueDate() const
    {
        return m_dueDate;
    }
    void setDueDate(const QDateTime &dueDate);

    int priority() const
    {
        return m_priority;
    }
    void setPriority(int priority);

    bool isCompleted() const
    {
        return m_isCompleted;
    }
    void setIsCompleted(bool isCompleted);

    QDateTime createdAt() const
    {
        return m_createdAt;
    }
    void setCreatedAt(const QDateTime &createdAt);

    QDateTime updatedAt() const
    {
        return m_updatedAt;
    }
    void setUpdatedAt(const QDateTime &updatedAt);

    QStringList labels() const
    {
        return m_labels;
    }
    void setLabels(const QStringList &labels);

    int parentTaskId() const
    {
        return m_parentTaskId;
    }
    void setParentTaskId(int parentTaskId);

    bool hasSubtasks() const
    {
        return m_subtaskCount > 0;
    }
    int subtaskCount() const
    {
        return m_subtaskCount;
    }
    void setSubtaskCount(int count);

    int completedSubtaskCount() const
    {
        return m_completedSubtaskCount;
    }
    void setCompletedSubtaskCount(int count);

    bool isRecurring() const
    {
        return m_isRecurring;
    }
    void setIsRecurring(bool isRecurring);

    QString repeatInterval() const
    {
        return m_repeatInterval;
    }
    void setRepeatInterval(const QString &interval);

    int repeatFrequency() const
    {
        return m_repeatFrequency;
    }
    void setRepeatFrequency(int frequency);

    QDateTime repeatStartDate() const
    {
        return m_repeatStartDate;
    }
    void setRepeatStartDate(const QDateTime &startDate);

    QString repeatEndType() const
    {
        return m_repeatEndType;
    }
    void setRepeatEndType(const QString &endType);

    QDateTime repeatEndDate() const
    {
        return m_repeatEndDate;
    }
    void setRepeatEndDate(const QDateTime &endDate);

    int repeatEndCount() const
    {
        return m_repeatEndCount;
    }
    void setRepeatEndCount(int count);

    int currentRepeatCount() const
    {
        return m_currentRepeatCount;
    }
    void setCurrentRepeatCount(int count);

    int pomodoroCount() const
    {
        return m_pomodoroCount;
    }
    void setPomodoroCount(int count);

    int pomodoroTarget() const
    {
        return m_pomodoroTarget;
    }
    void setPomodoroTarget(int target);

    int pomodoroLength() const
    {
        return m_pomodoroLength;
    }
    void setPomodoroLength(int length);

    int shortBreakLength() const
    {
        return m_shortBreakLength;
    }
    void setShortBreakLength(int length);

    int longBreakLength() const
    {
        return m_longBreakLength;
    }
    void setLongBreakLength(int length);

    bool pomodoroEnabled() const
    {
        return m_pomodoroEnabled;
    }
    void setPomodoroEnabled(bool enabled);

    bool pomodoroActive() const
    {
        return m_pomodoroActive;
    }
    void setPomodoroActive(bool active);

    // Time tracking methods
    int totalTimeSpent() const
    {
        return m_totalTimeSpent;
    }
    void setTotalTimeSpent(int timeSpent);

    bool timeTrackingActive() const
    {
        return m_timeTrackingActive;
    }
    void setTimeTrackingActive(bool active);

    QDateTime timeTrackingStarted() const
    {
        return m_timeTrackingStarted;
    }
    void setTimeTrackingStarted(const QDateTime &startTime);

    // Time tracking utility methods
    Q_INVOKABLE void startTimeTracking();
    Q_INVOKABLE void stopTimeTracking();
    Q_INVOKABLE void pauseTimeTracking();
    Q_INVOKABLE int getCurrentSessionTime() const;
    Q_INVOKABLE QString formatTimeSpent(int seconds) const;

Q_SIGNALS:
    void idChanged();
    void projectIdChanged();
    void contentChanged();
    void descriptionChanged();
    void dueDateChanged();
    void priorityChanged();
    void isCompletedChanged();
    void createdAtChanged();
    void updatedAtChanged();
    void labelsChanged();
    void parentTaskIdChanged();
    void hasSubtasksChanged();
    void subtaskCountChanged();
    void completedSubtaskCountChanged();
    void isRecurringChanged();
    void repeatIntervalChanged();
    void repeatFrequencyChanged();
    void repeatStartDateChanged();
    void repeatEndTypeChanged();
    void repeatEndDateChanged();
    void repeatEndCountChanged();
    void currentRepeatCountChanged();
    void pomodoroCountChanged();
    void pomodoroTargetChanged();
    void pomodoroLengthChanged();
    void shortBreakLengthChanged();
    void longBreakLengthChanged();
    void pomodoroEnabledChanged();
    void pomodoroActiveChanged();
    void totalTimeSpentChanged();
    void timeTrackingActiveChanged();
    void timeTrackingStartedChanged();

private:
    int m_id = -1;
    int m_projectId = -1;
    QString m_content;
    QString m_description;
    QDateTime m_dueDate;
    int m_priority = 1;
    bool m_isCompleted = false;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QStringList m_labels;
    int m_parentTaskId = -1;
    int m_subtaskCount = 0;
    int m_completedSubtaskCount = 0;
    bool m_isRecurring = false;
    QString m_repeatInterval = QStringLiteral("days"); // days, weeks, months, years
    int m_repeatFrequency = 1; // every X intervals
    QDateTime m_repeatStartDate;
    QString m_repeatEndType = QStringLiteral("never"); // never, after, on
    QDateTime m_repeatEndDate;
    int m_repeatEndCount = 0;
    int m_currentRepeatCount = 0;
    int m_pomodoroCount = 0;
    int m_pomodoroTarget = 4; // Default: 4 pomodoros per task
    int m_pomodoroLength = 25; // Default: 25 minutes
    int m_shortBreakLength = 5; // Default: 5 minutes
    int m_longBreakLength = 15; // Default: 15 minutes
    bool m_pomodoroEnabled = false;
    bool m_pomodoroActive = false;

    // Time tracking member variables
    int m_totalTimeSpent = 0; // Total time spent in seconds
    bool m_timeTrackingActive = false;
    QDateTime m_timeTrackingStarted;
};
