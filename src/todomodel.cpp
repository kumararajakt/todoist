#include "todomodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringLiteral>
#include <QStringView>

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
    case RepeatConfigRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_config"))), Qt::DisplayRole);
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
    roles[RepeatConfigRole] = "repeatConfig";
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
        break;
    case DescriptionRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("description"))), value, Qt::EditRole);
        break;
    case DueDateRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("due_date"))), value, Qt::EditRole);
        break;
    case PriorityRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("priority"))), value, Qt::EditRole);
        break;
    case IsCompletedRole: {
        bool result = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("is_completed"))), value, Qt::EditRole);
        if (result) {
            QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("updated_at"))), QDateTime::currentDateTime(), Qt::EditRole);
        }
        success = result;
        break;
    }
    case ProjectIdRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("project_id"))), value, Qt::EditRole);
        break;
    case RepeatConfigRole:
        success = QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("repeat_config"))), value, Qt::EditRole);
        break;
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

// Helper function to get property to column name mapping
static QMap<QString, QString> getPropertyToColumnMap()
{
    static const QMap<QString, QString> mapping = {{QStringLiteral("content"), QStringLiteral("content")},
                                                   {QStringLiteral("description"), QStringLiteral("description")},
                                                   {QStringLiteral("dueDate"), QStringLiteral("due_date")},
                                                   {QStringLiteral("priority"), QStringLiteral("priority")},
                                                   {QStringLiteral("repeatConfig"), QStringLiteral("repeat_config")}};
    return mapping;
}

bool TodoModel::addTask(const QVariantMap &taskData)
{
    if (taskData.isEmpty()) {
        qWarning() << "No task data provided";
        return false;
    }

    QSqlRecord record = this->record();

    // Set default values
    record.setValue(QStringLiteral("project_id"), m_currentProjectId > 0 ? m_currentProjectId : QVariant());
    record.setValue(QStringLiteral("is_completed"), false);
    record.setValue(QStringLiteral("created_at"), QDateTime::currentDateTime());
    record.setValue(QStringLiteral("updated_at"), QDateTime::currentDateTime());

    // Get the property to column mapping
    static const QMap<QString, QString> propertyToColumn = getPropertyToColumnMap();

    // Process each field in taskData
    for (auto it = taskData.constBegin(); it != taskData.constEnd(); ++it) {
        const QString &propertyName = it.key();
        const QVariant &value = it.value();

        // Skip null or invalid values
        if (value.isNull() || !value.isValid()) {
            continue;
        }

        // Special handling for dates
        if ((propertyName == QLatin1String("dueDate")) && !value.toDateTime().isValid()) {
            continue;
        }

        // Get the corresponding column name
        const QString columnName = propertyToColumn.value(propertyName);
        if (!columnName.isEmpty()) {
            record.setValue(columnName, value);
        }
    }

    bool success = insertRecord(-1, record);
    if (success) {
        submitAll();
    }
    return success;
}

bool TodoModel::updateTask(int taskId, const QVariantMap &updates)
{
    if (updates.isEmpty()) {
        qWarning() << "No updates provided for task" << taskId;
        return false;
    }

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
                if ((propertyName == QLatin1String("dueDate")) && !value.toDateTime().isValid()) {
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
                Q_EMIT taskUpdated(row);

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
        qDebug() << data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() << taskId;
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == taskId) {
            removeRows(row, 1);
            auto result = submitAll();
            return result;
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

void TodoModel::initializeDatabase()
{
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
    if (m_currentFilter == QStringLiteral("active")) {
        conditions << QStringLiteral("is_completed = 0");
    } else if (m_currentFilter == QStringLiteral("completed")) {
        conditions << QStringLiteral("is_completed = 1");
    }

    // Search filter
    if (!m_searchText.isEmpty()) {
        conditions << QStringLiteral("(content LIKE '%%1%' OR description LIKE '%%1%')").arg(m_searchText);
    }

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
