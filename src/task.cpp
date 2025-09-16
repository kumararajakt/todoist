#include "task.h"
#include <QObject>

Task::Task(QObject *parent)
    : QObject(parent)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

void Task::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        Q_EMIT idChanged();
    }
}

void Task::setProjectId(int projectId)
{
    if (m_projectId != projectId) {
        m_projectId = projectId;
        Q_EMIT projectIdChanged();
    }
}

void Task::setContent(const QString &content)
{
    if (m_content != content) {
        m_content = content;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT contentChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT descriptionChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setDueDate(const QDateTime &dueDate)
{
    if (m_dueDate != dueDate) {
        m_dueDate = dueDate;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT dueDateChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setPriority(int priority)
{
    if (m_priority != priority) {
        m_priority = priority;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT priorityChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setIsCompleted(bool isCompleted)
{
    if (m_isCompleted != isCompleted) {
        m_isCompleted = isCompleted;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT isCompletedChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt != createdAt) {
        m_createdAt = createdAt;
        Q_EMIT createdAtChanged();
    }
}

void Task::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt != updatedAt) {
        m_updatedAt = updatedAt;
        Q_EMIT updatedAtChanged();
    }
}

void Task::setLabels(const QStringList &labels)
{
    if (m_labels != labels) {
        m_labels = labels;
        Q_EMIT labelsChanged();
    }
}

void Task::setParentTaskId(int parentTaskId)
{
    if (m_parentTaskId != parentTaskId) {
        m_parentTaskId = parentTaskId;
        Q_EMIT parentTaskIdChanged();
    }
}

void Task::setSubtaskCount(int count)
{
    if (m_subtaskCount != count) {
        bool hadSubtasks = hasSubtasks();
        m_subtaskCount = count;
        Q_EMIT subtaskCountChanged();
        if (hadSubtasks != hasSubtasks()) {
            Q_EMIT hasSubtasksChanged();
        }
    }
}

void Task::setCompletedSubtaskCount(int count)
{
    if (m_completedSubtaskCount != count) {
        m_completedSubtaskCount = count;
        Q_EMIT completedSubtaskCountChanged();
    }
}

void Task::setIsRecurring(bool isRecurring)
{
    if (m_isRecurring != isRecurring) {
        m_isRecurring = isRecurring;
        Q_EMIT isRecurringChanged();
    }
}

void Task::setRepeatInterval(const QString &interval)
{
    if (m_repeatInterval != interval) {
        m_repeatInterval = interval;
        Q_EMIT repeatIntervalChanged();
    }
}

void Task::setRepeatFrequency(int frequency)
{
    if (m_repeatFrequency != frequency) {
        m_repeatFrequency = frequency;
        Q_EMIT repeatFrequencyChanged();
    }
}

void Task::setRepeatStartDate(const QDateTime &startDate)
{
    if (m_repeatStartDate != startDate) {
        m_repeatStartDate = startDate;
        Q_EMIT repeatStartDateChanged();
    }
}

void Task::setRepeatEndType(const QString &endType)
{
    if (m_repeatEndType != endType) {
        m_repeatEndType = endType;
        Q_EMIT repeatEndTypeChanged();
    }
}

void Task::setRepeatEndDate(const QDateTime &endDate)
{
    if (m_repeatEndDate != endDate) {
        m_repeatEndDate = endDate;
        Q_EMIT repeatEndDateChanged();
    }
}

void Task::setRepeatEndCount(int count)
{
    if (m_repeatEndCount != count) {
        m_repeatEndCount = count;
        Q_EMIT repeatEndCountChanged();
    }
}

void Task::setCurrentRepeatCount(int count)
{
    if (m_currentRepeatCount != count) {
        m_currentRepeatCount = count;
        Q_EMIT currentRepeatCountChanged();
    }
}

void Task::setPomodoroCount(int count)
{
    if (m_pomodoroCount != count) {
        m_pomodoroCount = count;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT pomodoroCountChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setPomodoroTarget(int target)
{
    if (m_pomodoroTarget != target) {
        m_pomodoroTarget = target;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT pomodoroTargetChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setPomodoroLength(int length)
{
    if (m_pomodoroLength != length) {
        m_pomodoroLength = length;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT pomodoroLengthChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setShortBreakLength(int length)
{
    if (m_shortBreakLength != length) {
        m_shortBreakLength = length;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT shortBreakLengthChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setLongBreakLength(int length)
{
    if (m_longBreakLength != length) {
        m_longBreakLength = length;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT longBreakLengthChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setPomodoroEnabled(bool enabled)
{
    if (m_pomodoroEnabled != enabled) {
        m_pomodoroEnabled = enabled;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT pomodoroEnabledChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setPomodoroActive(bool active)
{
    if (m_pomodoroActive != active) {
        m_pomodoroActive = active;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT pomodoroActiveChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setTotalTimeSpent(int timeSpent)
{
    if (m_totalTimeSpent != timeSpent) {
        m_totalTimeSpent = timeSpent;
        m_updatedAt = QDateTime::currentDateTime();
        Q_EMIT totalTimeSpentChanged();
        Q_EMIT updatedAtChanged();
    }
}

void Task::setTimeTrackingActive(bool active)
{
    if (m_timeTrackingActive != active) {
        m_timeTrackingActive = active;
        Q_EMIT timeTrackingActiveChanged();
    }
}

void Task::setTimeTrackingStarted(const QDateTime &startTime)
{
    if (m_timeTrackingStarted != startTime) {
        m_timeTrackingStarted = startTime;
        Q_EMIT timeTrackingStartedChanged();
    }
}

void Task::startTimeTracking()
{
    if (!m_timeTrackingActive) {
        setTimeTrackingStarted(QDateTime::currentDateTime());
        setTimeTrackingActive(true);
    }
}

void Task::stopTimeTracking()
{
    if (m_timeTrackingActive) {
        int sessionTime = getCurrentSessionTime();
        setTotalTimeSpent(m_totalTimeSpent + sessionTime);
        setTimeTrackingActive(false);
        setTimeTrackingStarted(QDateTime());
    }
}

void Task::pauseTimeTracking()
{
    if (m_timeTrackingActive) {
        int sessionTime = getCurrentSessionTime();
        setTotalTimeSpent(m_totalTimeSpent + sessionTime);
        setTimeTrackingActive(false);
        setTimeTrackingStarted(QDateTime());
    }
}

int Task::getCurrentSessionTime() const
{
    if (m_timeTrackingActive && m_timeTrackingStarted.isValid()) {
        return m_timeTrackingStarted.secsTo(QDateTime::currentDateTime());
    }
    return 0;
}

QString Task::formatTimeSpent(int seconds) const
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    if (hours > 0) {
        return QStringLiteral("%1h %2m %3s").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
    } else if (minutes > 0) {
        return QStringLiteral("%1m %2s").arg(minutes).arg(secs, 2, 10, QLatin1Char('0'));
    } else {
        return QStringLiteral("%1s").arg(secs);
    }
}