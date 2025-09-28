#pragma once

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QtCore>

// Forward declarations
class Task;
class Project;
class Label;

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize();

    QSqlDatabase database() const
    {
        return m_database;
    }

private:
    bool createTables();
    void migrateDatabaseSchema();
    QSqlDatabase m_database;
};