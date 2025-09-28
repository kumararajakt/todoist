#include "databasemanager.h"
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::initialize()
{
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataLocation);
    QString dbPath = dataLocation + QStringLiteral("/todoapp.db");

    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qDebug() << "Error opening database:" << m_database.lastError().text();
        return false;
    }

    bool result = createTables();

    return result;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // Create projects table
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS projects ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "user_id INTEGER DEFAULT 1,"
                                   "name TEXT NOT NULL,"
                                   "color TEXT DEFAULT '#3498db',"
                                   "is_favorite BOOLEAN DEFAULT 0)"))) {
        qDebug() << "Error creating projects table:" << query.lastError().text();
        return false;
    }

    // Create tasks table
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS tasks ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "project_id INTEGER,"
                                   "parent_task_id INTEGER,"
                                   "content TEXT NOT NULL,"
                                   "description TEXT,"
                                   "due_date DATETIME,"
                                   "is_completed BOOLEAN DEFAULT 0,"
                                   "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                   "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                   "repeat_interval TEXT DEFAULT '{}',"
                                   "time_tracker TEXT DEFAULT '{\"estimate\": 1800000, \"time_spent\": 0}',"
                                   "FOREIGN KEY (project_id) REFERENCES projects (id),"
                                   "FOREIGN KEY (parent_task_id) REFERENCES tasks (id) ON DELETE CASCADE)"))) {
        qDebug() << "Error creating tasks table:" << query.lastError().text();
        return false;
    }

    // Create labels table
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS labels ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL UNIQUE,"
                                   "color TEXT DEFAULT '#e74c3c')"))) {
        qDebug() << "Error creating labels table:" << query.lastError().text();
        return false;
    }

    // Create task_labels junction table
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS task_labels ("
                                   "task_id INTEGER,"
                                   "label_id INTEGER,"
                                   "PRIMARY KEY (task_id, label_id),"
                                   "FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,"
                                   "FOREIGN KEY (label_id) REFERENCES labels(id) ON DELETE CASCADE)"))) {
        qDebug() << "Error creating task_labels table:" << query.lastError().text();
        return false;
    }

    // Insert default project if none exists
    query.exec(QStringLiteral("SELECT COUNT(*) FROM projects"));
    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare(QStringLiteral("INSERT INTO projects (name, color, is_favorite) VALUES (?, ?, ?)"));
        query.addBindValue(QStringLiteral("Inbox"));
        query.addBindValue(QStringLiteral("#3498db"));
        query.addBindValue(true);
        query.exec();
    }

    return true;
}