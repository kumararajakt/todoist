#include "databasemanager.h"
#include "label.h"
#include "project.h"
#include "task.h"
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <qcontainerfwd.h>
#include <qobject.h>
#include <qstandardpaths.h>
#include <qstringliteral.h>

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
    if (result) {
        // Check if we need to migrate existing database
        migrateDatabaseSchema();
    }
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
                                   "priority INTEGER DEFAULT 1,"
                                   "is_completed BOOLEAN DEFAULT 0,"
                                   "is_recurring BOOLEAN DEFAULT 0,"
                                   "repeat_interval TEXT DEFAULT 'days',"
                                   "repeat_frequency INTEGER DEFAULT 1,"
                                   "repeat_start_date DATETIME,"
                                   "repeat_end_type TEXT DEFAULT 'never',"
                                   "repeat_end_date DATETIME,"
                                   "repeat_end_count INTEGER DEFAULT 0,"
                                   "current_repeat_count INTEGER DEFAULT 0,"
                                   "pomodoro_count INTEGER DEFAULT 0,"
                                   "pomodoro_target INTEGER DEFAULT 4,"
                                   "pomodoro_length INTEGER DEFAULT 25,"
                                   "short_break_length INTEGER DEFAULT 5,"
                                   "long_break_length INTEGER DEFAULT 15,"
                                   "pomodoro_enabled BOOLEAN DEFAULT 0,"
                                   "pomodoro_active BOOLEAN DEFAULT 0,"
                                   "total_time_spent INTEGER DEFAULT 0,"
                                   "time_tracking_active BOOLEAN DEFAULT 0,"
                                   "time_tracking_started DATETIME,"
                                   "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                   "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                   "FOREIGN KEY (project_id) REFERENCES projects(id),"
                                   "FOREIGN KEY (parent_task_id) REFERENCES tasks(id) ON DELETE CASCADE)"))) {
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

bool DatabaseManager::addTask(Task *task)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO tasks (project_id, parent_task_id, content, description, due_date, priority, is_completed, "
        "is_recurring, repeat_interval, repeat_frequency, repeat_start_date, repeat_end_type, repeat_end_date, repeat_end_count, current_repeat_count, "
        "pomodoro_count, pomodoro_target, pomodoro_length, short_break_length, long_break_length, pomodoro_enabled, pomodoro_active, "
        "total_time_spent, time_tracking_active, time_tracking_started) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
    query.addBindValue(task->projectId());
    query.addBindValue(task->parentTaskId() > 0 ? task->parentTaskId() : QVariant());
    query.addBindValue(task->content());
    query.addBindValue(task->description());
    query.addBindValue(task->dueDate());
    query.addBindValue(task->priority());
    query.addBindValue(task->isCompleted());
    query.addBindValue(task->isRecurring());
    query.addBindValue(task->repeatInterval());
    query.addBindValue(task->repeatFrequency());
    query.addBindValue(task->repeatStartDate());
    query.addBindValue(task->repeatEndType());
    query.addBindValue(task->repeatEndDate());
    query.addBindValue(task->repeatEndCount());
    query.addBindValue(task->currentRepeatCount());
    query.addBindValue(task->pomodoroCount());
    query.addBindValue(task->pomodoroTarget());
    query.addBindValue(task->pomodoroLength());
    query.addBindValue(task->shortBreakLength());
    query.addBindValue(task->longBreakLength());
    query.addBindValue(task->pomodoroEnabled());
    query.addBindValue(task->pomodoroActive());
    query.addBindValue(task->totalTimeSpent());
    query.addBindValue(task->timeTrackingActive());
    query.addBindValue(task->timeTrackingStarted());

    if (query.exec()) {
        task->setId(query.lastInsertId().toInt());
        return true;
    }

    qDebug() << "Error adding task:" << query.lastError().text();
    return false;
}

bool DatabaseManager::updateTask(Task *task)
{
    QSqlQuery query(m_database);
    query.prepare(
        QStringLiteral("UPDATE tasks SET project_id=?, content=?, description=?, due_date=?, "
                       "priority=?, is_completed=?, is_recurring=?, repeat_interval=?, repeat_frequency=?, "
                       "repeat_start_date=?, repeat_end_type=?, repeat_end_date=?, repeat_end_count=?, "
                       "current_repeat_count=?, pomodoro_count=?, pomodoro_target=?, pomodoro_length=?, "
                       "short_break_length=?, long_break_length=?, pomodoro_enabled=?, pomodoro_active=?, "
                       "total_time_spent=?, time_tracking_active=?, time_tracking_started=?, updated_at=CURRENT_TIMESTAMP WHERE id=?"));
    query.addBindValue(task->projectId());
    query.addBindValue(task->content());
    query.addBindValue(task->description());
    query.addBindValue(task->dueDate());
    query.addBindValue(task->priority());
    query.addBindValue(task->isCompleted());
    query.addBindValue(task->isRecurring());
    query.addBindValue(task->repeatInterval());
    query.addBindValue(task->repeatFrequency());
    query.addBindValue(task->repeatStartDate());
    query.addBindValue(task->repeatEndType());
    query.addBindValue(task->repeatEndDate());
    query.addBindValue(task->repeatEndCount());
    query.addBindValue(task->currentRepeatCount());
    query.addBindValue(task->pomodoroCount());
    query.addBindValue(task->pomodoroTarget());
    query.addBindValue(task->pomodoroLength());
    query.addBindValue(task->shortBreakLength());
    query.addBindValue(task->longBreakLength());
    query.addBindValue(task->pomodoroEnabled());
    query.addBindValue(task->pomodoroActive());
    query.addBindValue(task->totalTimeSpent());
    query.addBindValue(task->timeTrackingActive());
    query.addBindValue(task->timeTrackingStarted());
    query.addBindValue(task->id());

    return query.exec();
}

bool DatabaseManager::deleteTask(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("DELETE FROM tasks WHERE id=?"));
    query.addBindValue(taskId);
    return query.exec();
}

QList<Task *> DatabaseManager::getTasks(int projectId)
{
    QList<Task *> tasks;
    QSqlQuery query(m_database);

    if (projectId == -1) {
        query.exec(QStringLiteral("SELECT * FROM tasks ORDER BY created_at DESC"));
    } else {
        query.prepare(QStringLiteral("SELECT * FROM tasks WHERE project_id=? ORDER BY created_at DESC"));
        query.addBindValue(projectId);
        query.exec();
    }

    while (query.next()) {
        Task *task = new Task(this);
        task->setId(query.value("id").toInt());
        task->setProjectId(query.value("project_id").toInt());
        task->setParentTaskId(query.value("parent_task_id").toInt());
        task->setContent(query.value("content").toString());
        task->setDescription(query.value("description").toString());
        task->setDueDate(query.value("due_date").toDateTime());
        task->setPriority(query.value("priority").toInt());
        task->setIsCompleted(query.value("is_completed").toBool());
        task->setIsRecurring(query.value("is_recurring").toBool());
        task->setRepeatInterval(query.value("repeat_interval").toString());
        task->setRepeatFrequency(query.value("repeat_frequency").toInt());
        task->setRepeatStartDate(query.value("repeat_start_date").toDateTime());
        task->setRepeatEndType(query.value("repeat_end_type").toString());
        task->setRepeatEndDate(query.value("repeat_end_date").toDateTime());
        task->setRepeatEndCount(query.value("repeat_end_count").toInt());
        task->setCurrentRepeatCount(query.value("current_repeat_count").toInt());
        task->setPomodoroCount(query.value("pomodoro_count").toInt());
        task->setPomodoroTarget(query.value("pomodoro_target").toInt());
        task->setPomodoroLength(query.value("pomodoro_length").toInt());
        task->setShortBreakLength(query.value("short_break_length").toInt());
        task->setLongBreakLength(query.value("long_break_length").toInt());
        task->setPomodoroEnabled(query.value("pomodoro_enabled").toBool());
        task->setPomodoroActive(query.value("pomodoro_active").toBool());
        task->setTotalTimeSpent(query.value("total_time_spent").toInt());
        task->setTimeTrackingActive(query.value("time_tracking_active").toBool());
        task->setTimeTrackingStarted(query.value("time_tracking_started").toDateTime());
        task->setCreatedAt(query.value("created_at").toDateTime());
        task->setUpdatedAt(query.value("updated_at").toDateTime());
        task->setLabels(getTaskLabels(task->id()));

        // Load subtask counts
        loadSubtaskCounts(task);

        tasks.append(task);
    }

    return tasks;
}

QList<Task *> DatabaseManager::getTasksByFilter(const QString &filter)
{
    QList<Task *> tasks;
    QSqlQuery query(m_database);

    if (filter == QStringLiteral("today")) {
        query.exec(QStringLiteral("SELECT * FROM tasks WHERE DATE(due_date) = DATE('now') ORDER BY priority DESC"));
    } else if (filter == QStringLiteral("upcoming")) {
        query.exec(QStringLiteral("SELECT * FROM tasks WHERE due_date > datetime('now') ORDER BY due_date ASC"));
    } else if (filter == QStringLiteral("completed")) {
        query.exec(QStringLiteral("SELECT * FROM tasks WHERE is_completed = 1 ORDER BY updated_at DESC"));
    } else if (filter == QStringLiteral("priority")) {
        query.exec(QStringLiteral("SELECT * FROM tasks WHERE priority >= 3 AND is_completed = 0 ORDER BY priority DESC"));
    } else {
        return getTasks();
    }

    while (query.next()) {
        Task *task = new Task(this);
        task->setId(query.value("id").toInt());
        task->setProjectId(query.value("project_id").toInt());
        task->setContent(query.value("content").toString());
        task->setDescription(query.value("description").toString());
        task->setDueDate(query.value("due_date").toDateTime());
        task->setPriority(query.value("priority").toInt());
        task->setIsCompleted(query.value("is_completed").toBool());
        task->setPomodoroCount(query.value("pomodoro_count").toInt());
        task->setPomodoroTarget(query.value("pomodoro_target").toInt());
        task->setPomodoroLength(query.value("pomodoro_length").toInt());
        task->setShortBreakLength(query.value("short_break_length").toInt());
        task->setLongBreakLength(query.value("long_break_length").toInt());
        task->setPomodoroEnabled(query.value("pomodoro_enabled").toBool());
        task->setPomodoroActive(query.value("pomodoro_active").toBool());
        task->setTotalTimeSpent(query.value("total_time_spent").toInt());
        task->setTimeTrackingActive(query.value("time_tracking_active").toBool());
        task->setTimeTrackingStarted(query.value("time_tracking_started").toDateTime());
        task->setCreatedAt(query.value("created_at").toDateTime());
        task->setUpdatedAt(query.value("updated_at").toDateTime());
        task->setLabels(getTaskLabels(task->id()));
        tasks.append(task);
    }

    return tasks;
}

Task *DatabaseManager::getTask(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT * FROM tasks WHERE id=?"));
    query.addBindValue(taskId);

    if (query.exec() && query.next()) {
        Task *task = new Task(this);
        task->setId(query.value("id").toInt());
        task->setProjectId(query.value("project_id").toInt());
        task->setContent(query.value("content").toString());
        task->setDescription(query.value("description").toString());
        task->setDueDate(query.value("due_date").toDateTime());
        task->setPriority(query.value("priority").toInt());
        task->setIsCompleted(query.value("is_completed").toBool());
        task->setPomodoroCount(query.value("pomodoro_count").toInt());
        task->setPomodoroTarget(query.value("pomodoro_target").toInt());
        task->setPomodoroLength(query.value("pomodoro_length").toInt());
        task->setShortBreakLength(query.value("short_break_length").toInt());
        task->setLongBreakLength(query.value("long_break_length").toInt());
        task->setPomodoroEnabled(query.value("pomodoro_enabled").toBool());
        task->setPomodoroActive(query.value("pomodoro_active").toBool());
        task->setTotalTimeSpent(query.value("total_time_spent").toInt());
        task->setTimeTrackingActive(query.value("time_tracking_active").toBool());
        task->setTimeTrackingStarted(query.value("time_tracking_started").toDateTime());
        task->setCreatedAt(query.value("created_at").toDateTime());
        task->setUpdatedAt(query.value("updated_at").toDateTime());
        task->setLabels(getTaskLabels(task->id()));
        return task;
    }

    return nullptr;
}

bool DatabaseManager::addProject(Project *project)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("INSERT INTO projects (user_id, name, color, is_favorite) VALUES (?, ?, ?, ?)"));
    query.addBindValue(project->userId());
    query.addBindValue(project->name());
    query.addBindValue(project->color());
    query.addBindValue(project->isFavorite());

    if (query.exec()) {
        project->setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool DatabaseManager::updateProject(Project *project)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE projects SET name=?, color=?, is_favorite=? WHERE id=?"));
    query.addBindValue(project->name());
    query.addBindValue(project->color());
    query.addBindValue(project->isFavorite());
    query.addBindValue(project->id());

    return query.exec();
}

bool DatabaseManager::deleteProject(int projectId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("DELETE FROM projects WHERE id=?"));
    query.addBindValue(projectId);
    return query.exec();
}

QList<Project *> DatabaseManager::getProjects()
{
    QList<Project *> projects;
    QSqlQuery query(m_database);
    query.exec(QStringLiteral("SELECT * FROM projects ORDER BY is_favorite DESC, name ASC"));

    while (query.next()) {
        Project *project = new Project(this);
        project->setId(query.value("id").toInt());
        project->setUserId(query.value("user_id").toInt());
        project->setName(query.value("name").toString());
        project->setColor(query.value("color").toString());
        project->setIsFavorite(query.value("is_favorite").toBool());
        projects.append(project);
    }

    return projects;
}

Project *DatabaseManager::getProject(int projectId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT * FROM projects WHERE id=?"));
    query.addBindValue(projectId);

    if (query.exec() && query.next()) {
        Project *project = new Project(this);
        project->setId(query.value("id").toInt());
        project->setUserId(query.value("user_id").toInt());
        project->setName(query.value("name").toString());
        project->setColor(query.value("color").toString());
        project->setIsFavorite(query.value("is_favorite").toBool());
        return project;
    }

    return nullptr;
}

bool DatabaseManager::addLabel(Label *label)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("INSERT INTO labels (name, color) VALUES (?, ?)"));
    query.addBindValue(label->name());
    query.addBindValue(label->color());

    if (query.exec()) {
        label->setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool DatabaseManager::updateLabel(Label *label)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE labels SET name=?, color=? WHERE id=?"));
    query.addBindValue(label->name());
    query.addBindValue(label->color());
    query.addBindValue(label->id());

    return query.exec();
}

bool DatabaseManager::deleteLabel(int labelId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("DELETE FROM labels WHERE id=?"));
    query.addBindValue(labelId);
    return query.exec();
}

QList<Label *> DatabaseManager::getLabels()
{
    QList<Label *> labels;
    QSqlQuery query(m_database);
    query.exec(QStringLiteral("SELECT * FROM labels ORDER BY name ASC"));

    while (query.next()) {
        Label *label = new Label(this);
        label->setId(query.value("id").toInt());
        label->setName(query.value("name").toString());
        label->setColor(query.value("color").toString());
        labels.append(label);
    }

    return labels;
}

Label *DatabaseManager::getLabel(int labelId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT * FROM labels WHERE id=?"));
    query.addBindValue(labelId);

    if (query.exec() && query.next()) {
        Label *label = new Label(this);
        label->setId(query.value("id").toInt());
        label->setName(query.value("name").toString());
        label->setColor(query.value("color").toString());
        return label;
    }

    return nullptr;
}

bool DatabaseManager::addTaskLabel(int taskId, int labelId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("INSERT OR IGNORE INTO task_labels (task_id, label_id) VALUES (?, ?)"));
    query.addBindValue(taskId);
    query.addBindValue(labelId);
    return query.exec();
}

bool DatabaseManager::removeTaskLabel(int taskId, int labelId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("DELETE FROM task_labels WHERE task_id=? AND label_id=?"));
    query.addBindValue(taskId);
    query.addBindValue(labelId);
    return query.exec();
}

QStringList DatabaseManager::getTaskLabels(int taskId)
{
    QStringList labels;
    QSqlQuery query(m_database);
    query.prepare(
        QStringLiteral("SELECT l.name FROM labels l "
                       "JOIN task_labels tl ON l.id = tl.label_id "
                       "WHERE tl.task_id = ?"));
    query.addBindValue(taskId);
    query.exec();

    while (query.next()) {
        labels.append(query.value("name").toString());
    }

    return labels;
}

QList<Task *> DatabaseManager::getSubtasks(int parentTaskId)
{
    QList<Task *> subtasks;
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT * FROM tasks WHERE parent_task_id=? ORDER BY created_at ASC"));
    query.addBindValue(parentTaskId);
    query.exec();

    while (query.next()) {
        Task *task = new Task(this);
        task->setId(query.value("id").toInt());
        task->setProjectId(query.value("project_id").toInt());
        task->setParentTaskId(query.value("parent_task_id").toInt());
        task->setContent(query.value("content").toString());
        task->setDescription(query.value("description").toString());
        task->setDueDate(query.value("due_date").toDateTime());
        task->setPriority(query.value("priority").toInt());
        task->setIsCompleted(query.value("is_completed").toBool());
        task->setCreatedAt(query.value("created_at").toDateTime());
        task->setUpdatedAt(query.value("updated_at").toDateTime());
        task->setLabels(getTaskLabels(task->id()));
        subtasks.append(task);
    }

    return subtasks;
}

bool DatabaseManager::addSubtask(int parentTaskId, const QString &content, const QString &description, const QDateTime &dueDate, int priority)
{
    // Get parent task to inherit project_id
    Task *parentTask = getTask(parentTaskId);
    if (!parentTask) {
        return false;
    }

    Task subtask;
    subtask.setProjectId(parentTask->projectId());
    subtask.setParentTaskId(parentTaskId);
    subtask.setContent(content);
    subtask.setDescription(description);
    subtask.setDueDate(dueDate);
    subtask.setPriority(priority);

    bool result = addTask(&subtask);

    // Update parent task subtask counts
    if (result) {
        loadSubtaskCounts(parentTask);
    }

    delete parentTask;
    return result;
}

void DatabaseManager::loadSubtaskCounts(Task *task)
{
    QSqlQuery query(m_database);

    // Count total subtasks
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM tasks WHERE parent_task_id=?"));
    query.addBindValue(task->id());
    query.exec();

    int totalSubtasks = 0;
    if (query.next()) {
        totalSubtasks = query.value(0).toInt();
    }

    // Count completed subtasks
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM tasks WHERE parent_task_id=? AND is_completed=1"));
    query.addBindValue(task->id());
    query.exec();

    int completedSubtasks = 0;
    if (query.next()) {
        completedSubtasks = query.value(0).toInt();
    }

    task->setSubtaskCount(totalSubtasks);
    task->setCompletedSubtaskCount(completedSubtasks);
}

void DatabaseManager::migrateDatabaseSchema()
{
    QSqlQuery query(m_database);

    // Check if pomodoro columns exist, if not add them
    query.exec(QStringLiteral("PRAGMA table_info(tasks)"));
    QStringList columnNames;
    while (query.next()) {
        columnNames << query.value(1).toString(); // column name is at index 1
    }

    if (!columnNames.contains(QStringLiteral("pomodoro_count"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN pomodoro_count INTEGER DEFAULT 0"));
    }
    if (!columnNames.contains(QStringLiteral("pomodoro_target"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN pomodoro_target INTEGER DEFAULT 4"));
    }
    if (!columnNames.contains(QStringLiteral("pomodoro_length"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN pomodoro_length INTEGER DEFAULT 25"));
    }
    if (!columnNames.contains(QStringLiteral("short_break_length"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN short_break_length INTEGER DEFAULT 5"));
    }
    if (!columnNames.contains(QStringLiteral("long_break_length"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN long_break_length INTEGER DEFAULT 15"));
    }
    if (!columnNames.contains(QStringLiteral("pomodoro_enabled"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN pomodoro_enabled BOOLEAN DEFAULT 0"));
    }
    if (!columnNames.contains(QStringLiteral("pomodoro_active"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN pomodoro_active BOOLEAN DEFAULT 0"));
    }
    if (!columnNames.contains(QStringLiteral("total_time_spent"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN total_time_spent INTEGER DEFAULT 0"));
    }
    if (!columnNames.contains(QStringLiteral("time_tracking_active"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN time_tracking_active BOOLEAN DEFAULT 0"));
    }
    if (!columnNames.contains(QStringLiteral("time_tracking_started"))) {
        query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN time_tracking_started DATETIME"));
    }
}
