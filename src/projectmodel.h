#pragma once

#include "databasemanager.h"
#include "project.h"
#include <QAbstractListModel>
#include <QQmlEngine>

class ProjectModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        UserIdRole,
        NameRole,
        ColorRole,
        IsFavoriteRole
    };

    explicit ProjectModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Invokable methods
    Q_INVOKABLE bool addProject(const QString &name, const QString &color = QString(), bool isFavorite = false);
    Q_INVOKABLE bool updateProject(int projectId, const QString &name, const QString &color, bool isFavorite);
    Q_INVOKABLE bool deleteProject(int projectId);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE Project *getProject(int projectId);

    void setDatabaseManager(DatabaseManager *dbManager);

private:
    void loadProjects();

    QList<Project *> m_projects;
    DatabaseManager *m_dbManager = nullptr;
};