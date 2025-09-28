#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QSize>

class AppSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int lastSelectedProjectId READ lastSelectedProjectId WRITE setLastSelectedProjectId NOTIFY lastSelectedProjectIdChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowSizeChanged)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight NOTIFY windowSizeChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    int lastSelectedProjectId() const
    {
        return m_lastSelectedProjectId;
    }
    void setLastSelectedProjectId(int id);

    QSize windowSize() const
    {
        return m_windowSize;
    }
    void setWindowSize(const QSize &size);

    int windowWidth() const
    {
        return m_windowSize.width();
    }
    void setWindowWidth(int w)
    {
        setWindowSize(QSize(w, m_windowSize.height()));
    }

    int windowHeight() const
    {
        return m_windowSize.height();
    }
    void setWindowHeight(int h)
    {
        setWindowSize(QSize(m_windowSize.width(), h));
    }

Q_SIGNALS:
    void lastSelectedProjectIdChanged();
    void windowSizeChanged();

private:
    void load();
    void saveLastSelectedProjectId();
    void saveWindowSize();

    QSettings m_settings;
    int m_lastSelectedProjectId = -1;
    QSize m_windowSize{1024, 700};
};
