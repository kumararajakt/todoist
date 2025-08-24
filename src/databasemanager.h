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

    // Task operations
    bool addTask(Task *task);
    bool updateTask(Task *task);
    bool deleteTask(int taskId);
    QList<Task *> getTasks(int projectId = -1);
    QList<Task *> getTasksByFilter(const QString &filter);
    Task *getTask(int taskId);

    // Project operations
    bool addProject(Project *project);
    bool updateProject(Project *project);
    bool deleteProject(int projectId);
    QList<Project *> getProjects();
    Project *getProject(int projectId);

    // Label operations
    bool addLabel(Label *label);
    bool updateLabel(Label *label);
    bool deleteLabel(int labelId);
    QList<Label *> getLabels();
    Label *getLabel(int labelId);

    // Task-Label operations
    bool addTaskLabel(int taskId, int labelId);
    bool removeTaskLabel(int taskId, int labelId);
    QStringList getTaskLabels(int taskId);

    // Subtask operations
    Q_INVOKABLE QList<Task *> getSubtasks(int parentTaskId);
    Q_INVOKABLE bool
    addSubtask(int parentTaskId, const QString &content, const QString &description = QString(), const QDateTime &dueDate = QDateTime(), int priority = 1);
    void loadSubtaskCounts(Task *task);

private:
    bool createTables();
    QSqlDatabase m_database;
};