#include "projectmodel.h"
#include <QDebug>
#include <QObject>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <qstringliteral.h>

ProjectModel::ProjectModel(QObject *parent)
    : QSqlTableModel(parent)
{
    setTable(QStringLiteral("projects"));
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case IdRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole);
    case UserIdRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("user_id"))), Qt::DisplayRole);
    case NameRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("name"))), Qt::DisplayRole);
    case ColorRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("color"))), Qt::DisplayRole);
    case IsFavoriteRole:
        return QSqlTableModel::data(this->index(index.row(), getColumnIndex(QStringLiteral("is_favorite"))), Qt::DisplayRole);
    default:
        return QSqlTableModel::data(index, role);
    }
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[UserIdRole] = "userId";
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[IsFavoriteRole] = "isFavorite";
    return roles;
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    switch (role) {
    case NameRole:
        return QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("name"))), value, Qt::EditRole);
    case ColorRole:
        return QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("color"))), value, Qt::EditRole);
    case IsFavoriteRole:
        return QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("is_favorite"))), value, Qt::EditRole);
    case UserIdRole:
        return QSqlTableModel::setData(this->index(index.row(), getColumnIndex(QStringLiteral("user_id"))), value, Qt::EditRole);
    default:
        return QSqlTableModel::setData(index, value, role);
    }
}

bool ProjectModel::addProject(const QString &name, const QString &color, bool isFavorite)
{
    if (name.isEmpty())
        return false;

    QSqlRecord record = this->record();
    record.setValue(QStringLiteral("name"), name);
    record.setValue(QStringLiteral("color"), color.isEmpty() ? QStringLiteral("#3498db") : color);
    record.setValue(QStringLiteral("is_favorite"), isFavorite);
    record.setValue(QStringLiteral("user_id"), 1); // Default user_id

    bool success = insertRecord(-1, record);
    if (success) {
        submitAll();
    }
    return success;
}

bool ProjectModel::updateProject(int projectId, const QString &name, const QString &color, bool isFavorite)
{
    if (name.isEmpty())
        return false;

    // Find the row with the given projectId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == projectId) {
            setData(index(row, getColumnIndex(QStringLiteral("name"))), name, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("color"))), color, Qt::EditRole);
            setData(index(row, getColumnIndex(QStringLiteral("is_favorite"))), isFavorite, Qt::EditRole);
            return submitAll();
        }
    }
    return false;
}

bool ProjectModel::deleteProject(int projectId)
{
    // Find the row with the given projectId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == projectId) {
            removeRow(row);
            return submitAll();
        }
    }
    return false;
}

void ProjectModel::refresh()
{
    select();
}

QVariantMap ProjectModel::getProject(int projectId)
{
    QVariantMap project;

    // Find the row with the given projectId
    for (int row = 0; row < rowCount(); ++row) {
        if (data(index(row, getColumnIndex(QStringLiteral("id"))), Qt::DisplayRole).toInt() == projectId) {
            QSqlRecord rec = record(row);
            for (int i = 0; i < rec.count(); ++i) {
                project[rec.fieldName(i)] = rec.value(i);
            }
            break;
        }
    }
    return project;
}

void ProjectModel::initializeDatabase()
{
    // Database initialization is handled by the database connection
    // This method can be used for any additional setup if needed
    select();
}

int ProjectModel::getColumnIndex(const QString &columnName) const
{
    for (int i = 0; i < columnCount(); ++i) {
        if (headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() == columnName) {
            return i;
        }
    }
    return -1;
}
