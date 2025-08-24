#include "todomodel.h"
#include <QDebug>
#include <QObject>
#include <qstringliteral.h>
#include <qstringview.h>

TodoModel::TodoModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TodoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_filteredTasks.size();
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredTasks.size())
        return QVariant();

    Task *task = m_filteredTasks.at(index.row());

    switch (role) {
    case IdRole:
        return task->id();
    case ProjectIdRole:
        return task->projectId();
    case ContentRole:
        return task->content();
    case DescriptionRole:
        return task->description();
    case DueDateRole:
        return task->dueDate();
    case PriorityRole:
        return task->priority();
    case IsCompletedRole:
        return task->isCompleted();
    case CreatedAtRole:
        return task->createdAt();
    case UpdatedAtRole:
        return task->updatedAt();
    case LabelsRole:
        return task->labels();
    case ParentTaskIdRole:
        return task->parentTaskId();
    case HasSubtasksRole:
        return task->hasSubtasks();
    case SubtaskCountRole:
        return task->subtaskCount();
    case CompletedSubtaskCountRole:
        return task->completedSubtaskCount();
    case IsRecurringRole:
        return task->isRecurring();
    case RepeatIntervalRole:
        return task->repeatInterval();
    case RepeatFrequencyRole:
        return task->repeatFrequency();
    case RepeatStartDateRole:
        return task->repeatStartDate();
    case RepeatEndTypeRole:
        return task->repeatEndType();
    case RepeatEndDateRole:
        return task->repeatEndDate();
    case RepeatEndCountRole:
        return task->repeatEndCount();
    case CurrentRepeatCountRole:
        return task->currentRepeatCount();
    default:
        return QVariant();
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
    return roles;
}

void TodoModel::setCurrentProjectId(int projectId)
{
    if (m_currentProjectId != projectId) {
        m_currentProjectId = projectId;
        Q_EMIT currentProjectIdChanged();
        loadTasks();
    }
}

void TodoModel::setCurrentFilter(const QString &filter)
{
    if (m_currentFilter != filter) {
        m_currentFilter = filter;
        Q_EMIT currentFilterChanged();
        loadTasks();
    }
}

void TodoModel::setSearchText(const QString &searchText)
{
    if (m_searchText != searchText) {
        m_searchText = searchText;
        Q_EMIT searchTextChanged();
        filterTasks();
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
    if (!m_dbManager || content.isEmpty())
        return false;

    Task *task = new Task(this);
    task->setProjectId(m_currentProjectId > 0 ? m_currentProjectId : 1); // Default to first project
    task->setContent(content);
    task->setDescription(description);
    task->setDueDate(dueDate);
    task->setPriority(priority);
    task->setIsRecurring(isRecurring);
    task->setRepeatInterval(repeatInterval);
    task->setRepeatFrequency(repeatFrequency);
    task->setRepeatStartDate(repeatStartDate);
    task->setRepeatEndType(repeatEndType);
    task->setRepeatEndDate(repeatEndDate);
    task->setRepeatEndCount(repeatEndCount);
    task->setCurrentRepeatCount(0);

    if (m_dbManager->addTask(task)) {
        loadTasks();
        return true;
    }

    delete task;
    return false;
}

bool TodoModel::updateTask(int taskId,
                           const QString &content,
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
    if (!m_dbManager)
        return false;

    Task *task = m_dbManager->getTask(taskId);
    if (!task)
        return false;

    task->setContent(content);
    task->setDescription(description);
    task->setDueDate(dueDate);
    task->setPriority(priority);
    task->setIsRecurring(isRecurring);
    task->setRepeatInterval(repeatInterval);
    task->setRepeatFrequency(repeatFrequency);
    task->setRepeatStartDate(repeatStartDate);
    task->setRepeatEndType(repeatEndType);
    task->setRepeatEndDate(repeatEndDate);
    task->setRepeatEndCount(repeatEndCount);

    bool success = m_dbManager->updateTask(task);
    if (success) {
        loadTasks();
    }

    delete task;
    return success;
}

bool TodoModel::deleteTask(int taskId)
{
    if (!m_dbManager)
        return false;

    if (m_dbManager->deleteTask(taskId)) {
        loadTasks();
        return true;
    }

    return false;
}

bool TodoModel::toggleTaskCompleted(int taskId)
{
    if (!m_dbManager)
        return false;

    Task *task = m_dbManager->getTask(taskId);
    if (!task)
        return false;

    task->setIsCompleted(!task->isCompleted());
    bool success = m_dbManager->updateTask(task);

    if (success) {
        loadTasks();
    }

    delete task;
    return success;
}

void TodoModel::refresh()
{
    loadTasks();
}

Q_INVOKABLE QVariantMap TodoModel::get(int index) const
{
    if (index < 0 || index >= rowCount())
        return QVariantMap();

    QVariantMap map;
    for (const QByteArray &roleName : roleNames().values()) {
        map[QString::fromLatin1(roleName)] = data(this->index(index, 0), roleNames().key(roleName));
    }
    return map;
}

bool TodoModel::addSubtask(int parentTaskId, const QString &content, const QString &description, const QDateTime &dueDate, int priority)
{
    if (!m_dbManager) {
        return false;
    }

    bool success = m_dbManager->addSubtask(parentTaskId, content, description, dueDate, priority);
    if (success) {
        loadTasks(); // Refresh to update subtask counts
    }
    return success;
}

QVariantList TodoModel::getSubtasks(int parentTaskId)
{
    QVariantList result;
    if (!m_dbManager) {
        return result;
    }

    QList<Task *> subtasks = m_dbManager->getSubtasks(parentTaskId);
    for (Task *subtask : subtasks) {
        QVariantMap subtaskData;
        subtaskData[QStringLiteral("id")] = subtask->id();
        subtaskData[QStringLiteral("content")] = subtask->content();
        subtaskData[QStringLiteral("description")] = subtask->description();
        subtaskData[QStringLiteral("isCompleted")] = subtask->isCompleted();
        subtaskData[QStringLiteral("priority")] = subtask->priority();
        subtaskData[QStringLiteral("dueDate")] = subtask->dueDate();
        subtaskData[QStringLiteral("createdAt")] = subtask->createdAt();
        subtaskData[QStringLiteral("updatedAt")] = subtask->updatedAt();
        result.append(subtaskData);
    }

    // Clean up
    qDeleteAll(subtasks);
    return result;
}

void TodoModel::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;
    loadTasks();
}

void TodoModel::loadTasks()
{
    if (!m_dbManager)
        return;

    beginResetModel();

    // Clear existing tasks
    qDeleteAll(m_tasks);
    m_tasks.clear();

    // Load new tasks based on current filter/project
    if (!m_currentFilter.isEmpty()) {
        m_tasks = m_dbManager->getTasksByFilter(m_currentFilter);
    } else {
        m_tasks = m_dbManager->getTasks(m_currentProjectId);
    }

    filterTasks();
    endResetModel();
}

void TodoModel::filterTasks()
{
    m_filteredTasks.clear();

    if (m_searchText.isEmpty()) {
        m_filteredTasks = m_tasks;
    } else {
        for (Task *task : m_tasks) {
            if (task->content().contains(m_searchText, Qt::CaseInsensitive) || task->description().contains(m_searchText, Qt::CaseInsensitive)) {
                m_filteredTasks.append(task);
            }
        }
    }
}
