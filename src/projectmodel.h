#pragma once

#include <QDateTime>
#include <QObject>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <qstringliteral.h>

class ProjectModel : public QSqlTableModel
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

    // QSqlTableModel interface
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Invokable methods
    Q_INVOKABLE bool addProject(const QString &name, const QString &color = QString(), bool isFavorite = false);
    Q_INVOKABLE bool updateProject(int projectId, const QString &name, const QString &color, bool isFavorite);
    Q_INVOKABLE bool deleteProject(int projectId);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE QVariantMap getProject(int projectId);

    void initializeDatabase();

private:
    int getColumnIndex(const QString &columnName) const;
};