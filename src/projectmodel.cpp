#include "projectmodel.h"

ProjectModel::ProjectModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_projects.size();
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_projects.size())
        return QVariant();

    Project *project = m_projects.at(index.row());

    switch (role) {
    case IdRole:
        return project->id();
    case UserIdRole:
        return project->userId();
    case NameRole:
        return project->name();
    case ColorRole:
        return project->color();
    case IsFavoriteRole:
        return project->isFavorite();
    default:
        return QVariant();
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

bool ProjectModel::addProject(const QString &name, const QString &color, bool isFavorite)
{
    if (!m_dbManager || name.isEmpty())
        return false;

    Project *project = new Project(this);
    project->setName(name);
    project->setColor(color.isEmpty() ? QStringLiteral("#3498db") : color);
    project->setIsFavorite(isFavorite);

    if (m_dbManager->addProject(project)) {
        loadProjects();
        return true;
    }

    delete project;
    return false;
}

bool ProjectModel::updateProject(int projectId, const QString &name, const QString &color, bool isFavorite)
{
    if (!m_dbManager)
        return false;

    Project *project = m_dbManager->getProject(projectId);
    if (!project)
        return false;

    project->setName(name);
    project->setColor(color);
    project->setIsFavorite(isFavorite);

    bool success = m_dbManager->updateProject(project);
    if (success) {
        loadProjects();
    }

    delete project;
    return success;
}

bool ProjectModel::deleteProject(int projectId)
{
    if (!m_dbManager)
        return false;

    if (m_dbManager->deleteProject(projectId)) {
        loadProjects();
        return true;
    }

    return false;
}

void ProjectModel::refresh()
{
    loadProjects();
}

Project *ProjectModel::getProject(int projectId)
{
    if (!m_dbManager)
        return nullptr;

    return m_dbManager->getProject(projectId);
}

void ProjectModel::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;
    loadProjects();
}

void ProjectModel::loadProjects()
{
    if (!m_dbManager)
        return;

    beginResetModel();

    qDeleteAll(m_projects);
    m_projects.clear();

    m_projects = m_dbManager->getProjects();

    endResetModel();
}