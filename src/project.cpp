#include "project.h"
#include <QObject>

Project::Project(QObject *parent)
    : QObject(parent)
{
}

void Project::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        Q_EMIT idChanged();
    }
}

void Project::setUserId(int userId)
{
    if (m_userId != userId) {
        m_userId = userId;
        Q_EMIT userIdChanged();
    }
}

void Project::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged();
    }
}

void Project::setColor(const QString &color)
{
    if (m_color != color) {
        m_color = color;
        Q_EMIT colorChanged();
    }
}

void Project::setIsFavorite(bool isFavorite)
{
    if (m_isFavorite != isFavorite) {
        m_isFavorite = isFavorite;
        Q_EMIT isFavoriteChanged();
    }
}