#pragma once

#include "databasemanager.h"
#include "label.h"
#include <QAbstractListModel>
#include <QQmlEngine>

class LabelModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        ColorRole
    };

    explicit LabelModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Invokable methods
    Q_INVOKABLE bool addLabel(const QString &name, const QString &color = QString());
    Q_INVOKABLE bool updateLabel(int labelId, const QString &name, const QString &color);
    Q_INVOKABLE bool deleteLabel(int labelId);
    Q_INVOKABLE void refresh();

    void setDatabaseManager(DatabaseManager *dbManager);

private:
    void loadLabels();

    QList<Label *> m_labels;
    DatabaseManager *m_dbManager = nullptr;
};