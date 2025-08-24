#include "labelmodel.h"

LabelModel::LabelModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LabelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_labels.size();
}

QVariant LabelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_labels.size())
        return QVariant();

    Label *label = m_labels.at(index.row());

    switch (role) {
    case IdRole:
        return label->id();
    case NameRole:
        return label->name();
    case ColorRole:
        return label->color();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LabelModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    return roles;
}

bool LabelModel::addLabel(const QString &name, const QString &color)
{
    if (!m_dbManager || name.isEmpty())
        return false;

    Label *label = new Label(this);
    label->setName(name);
    label->setColor(color.isEmpty() ? QStringLiteral("#e74c3c") : color);

    if (m_dbManager->addLabel(label)) {
        loadLabels();
        return true;
    }

    delete label;
    return false;
}

bool LabelModel::updateLabel(int labelId, const QString &name, const QString &color)
{
    if (!m_dbManager)
        return false;

    Label *label = m_dbManager->getLabel(labelId);
    if (!label)
        return false;

    label->setName(name);
    label->setColor(color);

    bool success = m_dbManager->updateLabel(label);
    if (success) {
        loadLabels();
    }

    delete label;
    return success;
}

bool LabelModel::deleteLabel(int labelId)
{
    if (!m_dbManager)
        return false;

    if (m_dbManager->deleteLabel(labelId)) {
        loadLabels();
        return true;
    }

    return false;
}

void LabelModel::refresh()
{
    loadLabels();
}

void LabelModel::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;
    loadLabels();
}

void LabelModel::loadLabels()
{
    if (!m_dbManager)
        return;

    beginResetModel();

    qDeleteAll(m_labels);
    m_labels.clear();

    m_labels = m_dbManager->getLabels();

    endResetModel();
}