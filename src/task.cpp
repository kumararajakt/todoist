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