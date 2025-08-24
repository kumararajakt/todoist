#pragma once

#include "databasemanager.h"
#include "task.h"
#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>
#include <qcontainerfwd.h>
#include <qstringliteral.h>

class TodoModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int currentProjectId READ currentProjectId WRITE setCurrentProjectId NOTIFY currentProjectIdChanged)
    Q_PROPERTY(QString currentFilter READ currentFilter WRITE setCurrentFilter NOTIFY currentFilterChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        ProjectIdRole,
        ContentRole,
        DescriptionRole,
        DueDateRole,
        PriorityRole,
        IsCompletedRole,
        CreatedAtRole,
        UpdatedAtRole,
        LabelsRole,
        ParentTaskIdRole,
        HasSubtasksRole,
        SubtaskCountRole,
        CompletedSubtaskCountRole,
        IsRecurringRole,
        RepeatIntervalRole,
        RepeatFrequencyRole,
        RepeatStartDateRole,
        RepeatEndTypeRole,
        RepeatEndDateRole,
        RepeatEndCountRole,
        CurrentRepeatCountRole
    };

    explicit TodoModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Properties
    int currentProjectId() const
    {
        return m_currentProjectId;
    }
    void setCurrentProjectId(int projectId);

    QString currentFilter() const
    {
        return m_currentFilter;
    }
    void setCurrentFilter(const QString &filter);

    QString searchText() const
    {
        return m_searchText;
    }
    void setSearchText(const QString &searchText);

    // Invokable methods
    Q_INVOKABLE bool addTask(const QString &content,
                             const QString &description = QString(),
                             const QDateTime &dueDate = QDateTime(),
                             int priority = 1,
                             bool isRecurring = false,
                             const QString &repeatInterval = QString(QStringLiteral("days")),
                             int repeatFrequency = 1,
                             const QDateTime &repeatStartDate = QDateTime(),
                             const QString &repeatEndType = QString(QStringLiteral("never")),
                             const QDateTime &repeatEndDate = QDateTime(),
                             int repeatEndCount = 0);
    Q_INVOKABLE bool updateTask(int taskId,
                                const QString &content,
                                const QString &description = QString(),
                                const QDateTime &dueDate = QDateTime(),
                                int priority = 1,
                                bool isRecurring = false,
                                const QString &repeatInterval = QString(QStringLiteral("days")),
                                int repeatFrequency = 1,
                                const QDateTime &repeatStartDate = QDateTime(),
                                const QString &repeatEndType = QString(QStringLiteral("never")),
                                const QDateTime &repeatEndDate = QDateTime(),
                                int repeatEndCount = 0);
    Q_INVOKABLE bool deleteTask(int taskId);
    Q_INVOKABLE bool toggleTaskCompleted(int taskId);
    Q_INVOKABLE void refresh();

    Q_INVOKABLE QVariantMap get(int index) const;

    // Subtask methods
    Q_INVOKABLE bool
    addSubtask(int parentTaskId, const QString &content, const QString &description = QString(), const QDateTime &dueDate = QDateTime(), int priority = 1);
    Q_INVOKABLE QVariantList getSubtasks(int parentTaskId);

    void setDatabaseManager(DatabaseManager *dbManager);

Q_SIGNALS:
    void currentProjectIdChanged();
    void currentFilterChanged();
    void searchTextChanged();

private:
    void loadTasks();
    void filterTasks();

    QList<Task *> m_tasks;
    QList<Task *> m_filteredTasks;
    DatabaseManager *m_dbManager = nullptr;
    int m_currentProjectId = -1;
    QString m_currentFilter;
    QString m_searchText;
};
