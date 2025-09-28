#pragma once

#include <QDateTime>
#include <QObject>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStringLiteral>

class SystemTrayManager;

class TodoModel : public QSqlTableModel
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
        RepeatConfigRole,
    };

    explicit TodoModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    int currentProjectId() const;
    void setCurrentProjectId(int projectId);

    QString currentFilter() const;
    void setCurrentFilter(const QString &filter);

    QString searchText() const;
    void setSearchText(const QString &searchText);

    Q_INVOKABLE bool addTask(const QVariantMap &taskData);
    Q_INVOKABLE bool updateTask(int taskId, const QVariantMap &updates);
    Q_INVOKABLE bool deleteTask(int taskId);
    Q_INVOKABLE bool toggleTaskCompleted(int taskId);
    Q_INVOKABLE void refresh();

    Q_INVOKABLE QVariantMap get(int index) const;

    Q_INVOKABLE bool
    addSubtask(int parentTaskId, const QString &content, const QString &description = QString(), const QDateTime &dueDate = QDateTime(), int priority = 1);
    Q_INVOKABLE QVariantList getSubtasks(int parentTaskId);

    void initializeDatabase();

Q_SIGNALS:
    void currentProjectIdChanged();
    void currentFilterChanged();
    void searchTextChanged();
    void taskUpdated(int index);

private:
    void applyFilters();
    int getColumnIndex(const QString &columnName) const;

    int m_currentProjectId = -1;
    QString m_currentFilter;
    QString m_searchText;
};
