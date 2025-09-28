#include "appsettings.h"

#include <QCoreApplication>
#include <QVariant>

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
    load();
}

void AppSettings::setLastSelectedProjectId(int id)
{
    if (m_lastSelectedProjectId == id)
        return;
    m_lastSelectedProjectId = id;
    saveLastSelectedProjectId();
    Q_EMIT lastSelectedProjectIdChanged();
}

void AppSettings::setWindowSize(const QSize &size)
{
    if (m_windowSize == size)
        return;
    m_windowSize = size;
    saveWindowSize();
    Q_EMIT windowSizeChanged();
}

void AppSettings::load()
{
    m_settings.beginGroup(QStringLiteral("ui"));
    m_lastSelectedProjectId = m_settings.value(QStringLiteral("lastSelectedProjectId"), m_lastSelectedProjectId).toInt();
    const int w = m_settings.value(QStringLiteral("windowWidth"), m_windowSize.width()).toInt();
    const int h = m_settings.value(QStringLiteral("windowHeight"), m_windowSize.height()).toInt();
    m_windowSize = QSize(w, h);
    m_settings.endGroup();
}

void AppSettings::saveLastSelectedProjectId()
{
    m_settings.beginGroup(QStringLiteral("ui"));
    m_settings.setValue(QStringLiteral("lastSelectedProjectId"), m_lastSelectedProjectId);
    m_settings.endGroup();
}

void AppSettings::saveWindowSize()
{
    m_settings.beginGroup(QStringLiteral("ui"));
    m_settings.setValue(QStringLiteral("windowWidth"), m_windowSize.width());
    m_settings.setValue(QStringLiteral("windowHeight"), m_windowSize.height());
    m_settings.endGroup();
}
