#pragma once

#include <QObject>
#include <QString>

class Label : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit Label(QObject *parent = nullptr);

    int id() const
    {
        return m_id;
    }
    void setId(int id);

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

Q_SIGNALS:
    void idChanged();
    void nameChanged();
    void colorChanged();

private:
    int m_id = -1;
    QString m_name;
    QString m_color = QStringLiteral("#e74c3c"); // Default red
};