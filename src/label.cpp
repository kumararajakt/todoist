#include "label.h"
#include <QObject>

Label::Label(QObject *parent)
    : QObject(parent)
{
}

void Label::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        Q_EMIT idChanged();
    }
}

void Label::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged();
    }
}

void Label::setColor(const QString &color)
{
    if (m_color != color) {
        m_color = color;
        Q_EMIT colorChanged();
    }
}