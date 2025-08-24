#pragma once

#include <QObject>
#include <QString>

class Project : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool isFavorite READ isFavorite WRITE setIsFavorite NOTIFY isFavoriteChanged)

public:
    explicit Project(QObject *parent = nullptr);

    int id() const
    {
        return m_id;
    }
    void setId(int id);

    int userId() const
    {
        return m_userId;
    }
    void setUserId(int userId);

    QString name() const
    {
        return m_name;
    }
    void setName(const QString &name);

    QString color() const
    {
        return m_color;
    }
    void setColor(const QString &color);

    bool isFavorite() const
    {
        return m_isFavorite;
    }
    void setIsFavorite(bool isFavorite);

Q_SIGNALS:
    void idChanged();
    void userIdChanged();
    void nameChanged();
    void colorChanged();
    void isFavoriteChanged();

private:
    int m_id = -1;
    int m_userId = 1; // Default user
    QString m_name;
    QString m_color = QStringLiteral("#3498db"); // Default blue
    bool m_isFavorite = false;
};