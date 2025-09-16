#include "todomodel.h"

#include <QDebug>
#include <QObject>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringLiteral>
#include <QStringView>
#include <qlogging.h>
#include <qstringview.h>

TodoModel::TodoModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    setTable(QStringLiteral("tasks"));
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    this->sort(getColumnIndex(QStringLiteral("created_at")), Qt::DescendingOrder);
    this->setFilter(QStringLiteral("is_completed = 0"));
    select();
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case IdRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole);
    case ProjectIdRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("project_id"))), Qt::DisplayRole);
    case ContentRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("content"))), Qt::DisplayRole);
    case DescriptionRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("description"))), Qt::DisplayRole);
    case DueDateRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("due_date"))), Qt::DisplayRole);
    case PriorityRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("priority"))), Qt::DisplayRole);
    case IsCompletedRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("is_completed"))), Qt::DisplayRole);
    case CreatedAtRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("created_at"))), Qt::DisplayRole);
    case UpdatedAtRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("updated_at"))), Qt::DisplayRole);
    case ParentTaskIdRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("parent_task_id"))), Qt::DisplayRole);
    case IsRecurringRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("is_recurring"))), Qt::DisplayRole);
    case RepeatIntervalRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_interval"))), Qt::DisplayRole);
    case RepeatFrequencyRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_frequency"))), Qt::DisplayRole);
    case RepeatStartDateRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_start_date"))), Qt::DisplayRole);
    case RepeatEndTypeRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_type"))), Qt::DisplayRole);
    case RepeatEndDateRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_date"))), Qt::DisplayRole);
    case RepeatEndCountRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_count"))), Qt::DisplayRole);
    case CurrentRepeatCountRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("current_repeat_count"))), Qt::DisplayRole);
    case PomodoroCountRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_count"))), Qt::DisplayRole);
    case PomodoroTargetRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_target"))), Qt::DisplayRole);
    case PomodoroLengthRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_length"))), Qt::DisplayRole);
    case ShortBreakLengthRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("short_break_length"))), Qt::DisplayRole);
    case LongBreakLengthRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("long_break_length"))), Qt::DisplayRole);
    case PomodoroEnabledRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_enabled"))), Qt::DisplayRole);
    case PomodoroActiveRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_active"))), Qt::DisplayRole);
    case TotalTimeSpentRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("total_time_spent"))), Qt::DisplayRole);
    case TimeTrackingActiveRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("time_tracking_active"))), Qt::DisplayRole);
    case TimeTrackingStartedRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("time_tracking_started"))), Qt::DisplayRole);
    case LabelsRole:
        // For labels, we need a separate query since it's in a different table
        {
            int taskId = QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
            QSqlQuery query(database());
            query.prepare(QStringLiteral("SELECT l.name FROM labels l JOIN task_labels tl ON l.id = tl.label_id WHERE tl.task_id = ?"));
            query.addBindValue(taskId);
            QStringList labels;
            if (query.exec()) {
                while (query.next()) {
                    labels << query.value(0).toString();
                }
            }
            return labels;
        }
    case HasSubtasksRole: {
        int taskId = QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
        QSqlQuery query(database());
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM tasks WHERE parent_task_id = ?"));
        query.addBindValue(taskId);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() > 0;
        }
        return false;
    }
    case SubtaskCountRole: {
        int taskId = QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
        QSqlQuery query(database());
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM tasks WHERE parent_task_id = ?"));
        query.addBindValue(taskId);
        if (query.exec() && query.next()) {
            return query.value(0).toInt();
        }
        return 0;
    }
    case CompletedSubtaskCountRole: {
        int taskId = QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
        QSqlQuery query(database());
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM tasks WHERE parent_task_id = ? AND is_completed = 1"));
        query.addBindValue(taskId);
        if (query.exec() && query.next()) {
            return query.value(0).toInt();
        }
        return 0;
    }
    default:
        return QSqlTableModel::data(index, role);
    }
}

QHash<int, QByteArray> TodoModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[ProjectIdRole] = "projectId";
    roles[ContentRole] = "content";
    roles[DescriptionRole] = "description";
    roles[DueDateRole] = "dueDate";
    roles[PriorityRole] = "priority";
    roles[IsCompletedRole] = "isCompleted";
    roles[CreatedAtRole] = "createdAt";
    roles[UpdatedAtRole] = "updatedAt";
    roles[LabelsRole] = "labels";
    roles[ParentTaskIdRole] = "parentTaskId";
    roles[HasSubtasksRole] = "hasSubtasks";
    roles[SubtaskCountRole] = "subtaskCount";
    roles[CompletedSubtaskCountRole] = "completedSubtaskCount";
    roles[IsRecurringRole] = "isRecurring";
    roles[RepeatIntervalRole] = "repeatInterval";
    roles[RepeatFrequencyRole] = "repeatFrequency";
    roles[RepeatStartDateRole] = "repeatStartDate";
    roles[RepeatEndTypeRole] = "repeatEndType";
    roles[RepeatEndDateRole] = "repeatEndDate";
    roles[RepeatEndCountRole] = "repeatEndCount";
    roles[CurrentRepeatCountRole] = "currentRepeatCount";
    roles[PomodoroCountRole] = "pomodoroCount";
    roles[PomodoroTargetRole] = "pomodoroTarget";
    roles[PomodoroLengthRole] = "pomodoroLength";
    roles[ShortBreakLengthRole] = "shortBreakLength";
    roles[LongBreakLengthRole] = "longBreakLength";
    roles[PomodoroEnabledRole] = "pomodoroEnabled";
    roles[PomodoroActiveRole] = "pomodoroActive";
    roles[TotalTimeSpentRole] = "totalTimeSpent";
    roles[TimeTrackingActiveRole] = "timeTrackingActive";
    roles[TimeTrackingStartedRole] = "timeTrackingStarted";
    return roles;
}

bool TodoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    bool success = false;

    switch (role) {
    case ContentRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("content"))), value, Qt::EditRole);
    case DescriptionRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("description"))), value, Qt::EditRole);
    case DueDateRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("due_date"))), value, Qt::EditRole);
    case PriorityRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("priority"))), value, Qt::EditRole);
    case IsCompletedRole: {
        bool result = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("is_completed"))), value, Qt::EditRole);
        if (result) {
            QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
        }
        success = result;
    }
    case ProjectIdRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("project_id"))), value, Qt::EditRole);
    case ParentTaskIdRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("parent_task_id"))), value, Qt::EditRole);
    case IsRecurringRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("is_recurring"))), value, Qt::EditRole);
    case RepeatIntervalRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_interval"))), value, Qt::EditRole);
    case RepeatFrequencyRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_frequency"))), value, Qt::EditRole);
    case RepeatStartDateRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_start_date"))), value, Qt::EditRole);
    case RepeatEndTypeRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_type"))), value, Qt::EditRole);
    case RepeatEndDateRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_date"))), value, Qt::EditRole);
    case RepeatEndCountRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_end_count"))), value, Qt::EditRole);
    case CurrentRepeatCountRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("current_repeat_count"))), value, Qt::EditRole);
    case PomodoroCountRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_count"))), value, Qt::EditRole);
    case PomodoroTargetRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_target"))), value, Qt::EditRole);
    case PomodoroLengthRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_length"))), value, Qt::EditRole);
    case ShortBreakLengthRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("short_break_length"))), value, Qt::EditRole);
    case LongBreakLengthRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("long_break_length"))), value, Qt::EditRole);
    case PomodoroEnabledRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_enabled"))), value, Qt::EditRole);
    case PomodoroActiveRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("pomodoro_active"))), value, Qt::EditRole);
    case TotalTimeSpentRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("total_time_spent"))), value, Qt::EditRole);
    case TimeTrackingActiveRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("time_tracking_active"))), value, Qt::EditRole);
    case TimeTrackingStartedRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("time_tracking_started"))), value, Qt::EditRole);
    default:
        success = QSqlTableModel::setData(index, value, role);
    }

    if (success) {
        submitAll(); // Commit changes immediately
    }
    return success;
}

void TodoModel::setCurrentProjectId(int projectId)
{
    if (m_currentProjectId != projectId) {
        m_currentProjectId = projectId;
        Q_EMIT currentProjectIdChanged();
        applyFilters();
    }
}

void TodoModel::setCurrentFilter(const QString &filter)
{
    if (m_currentFilter != filter) {
        m_currentFilter = filter;
        Q_EMIT currentFilterChanged();
        applyFilters();
    }
}

void TodoModel::setSearchText(const QString &searchText)
{
    if (m_searchText != searchText) {
        m_searchText = searchText;
        Q_EMIT searchTextChanged();
        applyFilters();
    }
}

bool TodoModel::addTask(const QString &content,
                        const QString &description,
                        const QDateTime &dueDate,
                        int priority,
                        bool isRecurring,
                        const QString &repeatInterval,
                        int repeatFrequency,
                        const QDateTime &repeatStartDate,
                        const QString &repeatEndType,
                        const QDateTime &repeatEndDate,
                        int repeatEndCount)
{
    QSqlRecord record = this->record();
    record.setValue(QStringLiteral("project_id"), m_currentProjectId > 0 ? m_currentProjectId : QVariant());
    record.setValue(QStringLiteral("content"), content);
    record.setValue(QStringLiteral("description"), description);
    record.setValue(QStringLiteral("due_date"), dueDate.isValid() ? dueDate : QVariant());
    record.setValue(QStringLiteral("priority"), priority);
    record.setValue(QStringLiteral("is_completed"), false);
    record.setValue(QStringLiteral("is_recurring"), isRecurring);
    record.setValue(QStringLiteral("repeat_interval"), repeatInterval);
    record.setValue(QStringLiteral("repeat_frequency"), repeatFrequency);
    record.setValue(QStringLiteral("repeat_start_date"), repeatStartDate.isValid() ? repeatStartDate : QVariant());
    record.setValue(QStringLiteral("repeat_end_type"), repeatEndType);
    record.setValue(QStringLiteral("repeat_end_date"), repeatEndDate.isValid() ? repeatEndDate : QVariant());
    record.setValue(QStringLiteral("repeat_end_count"), repeatEndCount);
    record.setValue(QStringLiteral("current_repeat_count"), 0);
    record.setValue(QStringLiteral("created_at"), QDateTime::currentDateTime());
    record.setValue(QStringLiteral("updated_at"), QDateTime::currentDateTime());

    bool success = insertRecord(-1, record);
    if (success) {
        submitAll();
    }
    return success;
}

// Helper function to get property to column name mapping
static QMap<QString, QString> getPropertyToColumnMap()
{
    static const QMap<QString, QString> mapping = {{QStringLiteral("content"), QStringLiteral("content")},
                                                   {QStringLiteral("description"), QStringLiteral("description")},
                                                   {QStringLiteral("dueDate"), QStringLiteral("due_date")},
                                                   {QStringLiteral("priority"), QStringLiteral("priority")},
                                                   {QStringLiteral("isRecurring"), QStringLiteral("is_recurring")},
                                                   {QStringLiteral("repeatInterval"), QStringLiteral("repeat_interval")},
                                                   {QStringLiteral("repeatFrequency"), QStringLiteral("repeat_frequency")},
                                                   {QStringLiteral("repeatStartDate"), QStringLiteral("repeat_start_date")},
                                                   {QStringLiteral("repeatEndType"), QStringLiteral("repeat_end_type")},
                                                   {QStringLiteral("repeatEndDate"), QStringLiteral("repeat_end_date")},
                                                   {QStringLiteral("repeatEndCount"), QStringLiteral("repeat_end_count")}};
    return mapping;
}

bool TodoModel::updateTask(int taskId, const QVariantMap &updates)
{
    if (updates.isEmpty()) {
        qWarning() << "No updates provided for task" << taskId;
        return false;
    }

    qDebug() << "Updating task" << taskId << "with values:" << updates;

    // Get the property to column mapping
    static const QMap<QString, QString> propertyToColumn = getPropertyToColumnMap();

    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            bool hasUpdates = false;

            // Process each update
            for (auto it = updates.constBegin(); it != updates.constEnd(); ++it) {
                const QString &propertyName = it.key();
                const QVariant &value = it.value();

                // Skip null or invalid values
                if (value.isNull() || !value.isValid()) {
                    continue;
                }

                // Special handling for dates
                if ((propertyName == QLatin1String("dueDate") || propertyName == QLatin1String("repeatStartDate")
                     || propertyName == QLatin1String("repeatEndDate"))
                    && !value.toDateTime().isValid()) {
                    continue;
                }

                // Special handling for numbers
                if ((propertyName == QLatin1String("priority") || propertyName == QLatin1String("repeatFrequency")
                     || propertyName == QLatin1String("repeatEndCount"))
                    && value.toInt() < 0) {
                    continue;
                }

                // Get the corresponding column name
                QString columnName = propertyToColumn.value(propertyName, propertyName);

                // Update the field
                setData(index(row, getColumnIndex(columnName)), value, Qt::EditRole);
                hasUpdates = true;
            }

            if (hasUpdates) {
                // Always update the updated_at timestamp when anything changes
                setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
                return submitAll();
            }

            return true; // No actual updates, but task exists
        }
    }

    qWarning() << "Task with id" << taskId << "not found";
    return false;
}

bool TodoModel::deleteTask(int taskId)
{
    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            removeRow(row);
            return submitAll();
        }
    }
    return false;
}

bool TodoModel::toggleTaskCompleted(int taskId)
{
    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            bool currentCompleted = data(index(row, getColumnIndex(QStringLiteral("is_completed"))), Qt::DisplayRole).toBool();
            setData(index(row, getColumnIndex(QStringLiteral("is_completed"))), !currentCompleted, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

void TodoModel::refresh()
{
    select();
}

QVariantMap TodoModel::get(int index) const
{
    QVariantMap map;
    if (index >= 0 && index < rowCount()) {
        QSqlRecord rec = record(index);
        for (int i = 0; i < rec.count(); ++i) {
            map[rec.fieldName(i)] = rec.value(i);
        }
    }
    return map;
}

bool TodoModel::addSubtask(int parentTaskId, const QString &content, const QString &description, const QDateTime &dueDate, int priority)
{
    QSqlRecord record = this->record();
    record.setValue(QStringLiteral("parent_task_id"), parentTaskId);
    record.setValue(QStringLiteral("project_id"), m_currentProjectId > 0 ? m_currentProjectId : QVariant());
    record.setValue(QStringLiteral("content"), content);
    record.setValue(QStringLiteral("description"), description);
    record.setValue(QStringLiteral("due_date"), dueDate.isValid() ? dueDate : QVariant());
    record.setValue(QStringLiteral("priority"), priority);
    record.setValue(QStringLiteral("is_completed"), false);
    record.setValue(QStringLiteral("created_at"), QDateTime::currentDateTime());
    record.setValue(QStringLiteral("updated_at"), QDateTime::currentDateTime());

    bool success = insertRecord(-1, record);
    if (success) {
        submitAll();
    }
    return success;
}

QVariantList TodoModel::getSubtasks(int parentTaskId)
{
    QVariantList subtasks;
    QSqlQuery query(database());
    query.prepare(QStringLiteral("SELECT * FROM tasks WHERE parent_task_id = ? ORDER BY created_at"));
    query.addBindValue(parentTaskId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap task;
            QSqlRecord rec = query.record();
            for (int i = 0; i < rec.count(); ++i) {
                task[rec.fieldName(i)] = rec.value(i);
            }
            subtasks.append(task);
        }
    }
    return subtasks;
}

bool TodoModel::updatePomodoroCount(int taskId, int count)
{
    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_count"))), count, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

bool TodoModel::updatePomodoroSettings(int taskId, int target, int length, int shortBreak, int longBreak)
{
    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_target"))), target, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_length"))), length, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("short_break_length"))), shortBreak, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("long_break_length"))), longBreak, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

bool TodoModel::enablePomodoro(int taskId, bool enabled)
{
    // Find the row with the given taskId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_enabled"))), enabled, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

bool TodoModel::setActivePomodoroTask(int taskId)
{
    // First, deactivate all other pomodoro tasks
    for (int row = 0; row < rowCount(); ++row) {
        int currentId = data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
        if (currentId != taskId) {
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_active"))), false, Qt::EditRole);
        }
    }

    // Then activate the specified task
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            setData(index(row, getColumnIndex(QStringLiteral("pomodoro_active"))), true, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

int TodoModel::getActivePomodoroTaskId()
{
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("pomodoro_active"))), Qt::DisplayRole).toBool()) {
            return data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt();
        }
    }
    return -1;
}

void TodoModel::initializeDatabase()
{
    // Database initialization is handled by the database connection
    // This method can be used for any additional setup if needed
    select();
}

void TodoModel::applyFilters()
{
    QStringList conditions;

    // Project filter
    if (m_currentProjectId > 0) {
        conditions << QStringLiteral("project_id = %1").arg(m_currentProjectId);
    }

    // Status filter
    if (m_currentFilter == QStringLiteral("active") || m_currentFilter != QStringLiteral("completed")) {
        conditions << QStringLiteral("is_completed = 0");
    } else if (m_currentFilter == QStringLiteral("completed")) {
        conditions << QStringLiteral("is_completed = 1");
    }

    // Search filter
    if (!m_searchText.isEmpty()) {
        conditions << QStringLiteral("(content LIKE '%%1%' OR description LIKE '%%1%')").arg(m_searchText);
    }

    qWarning() << "Conditions: " << conditions << m_currentFilter;

    QString filterString = conditions.join(QStringLiteral(" AND "));
    setFilter(filterString);
    select();
}

int TodoModel::getColumnIndex(const QString &columnName) const
{
    for (int i = 0; i < columnCount(); ++i) {
        if (headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() == columnName) {
            return i;
        }
    }
    return -1;
}
int TodoModel::currentProjectId() const
{
    return m_currentProjectId;
}
QString TodoModel::currentFilter() const
{
    return m_currentFilter;
}
QString TodoModel::searchText() const
{
    return m_searchText;
}
