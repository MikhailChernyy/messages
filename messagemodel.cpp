#include "messagemodel.h"
#include "database.h"

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void MessageModel::setDatabase(Database *db)
{
    m_database = db;
    refresh();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_messages.size();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_messages.size())
        return QVariant();

    QString message = m_messages.at(index.row());
    QStringList parts = message.split("|");

    if (parts.size() < 3)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return parts[0] + ": " + parts[2];
    case Qt::UserRole:
        return parts[0];
    case Qt::UserRole + 1:
        return parts[1];
    case Qt::UserRole + 2:
        return parts[2];
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole] = "sender";
    roles[Qt::UserRole + 1] = "timestamp";
    roles[Qt::UserRole + 2] = "text";
    return roles;
}

void MessageModel::loadMoreMessages()
{
    if (!m_database || !m_hasMore)
        return;

    QStringList newMessages = m_database->getMessages(m_currentOffset, m_pageSize);

    if (newMessages.isEmpty()) {
        m_hasMore = false;
        return;
    }

    beginInsertRows(QModelIndex(), m_messages.count(), m_messages.count() + newMessages.count() - 1);
    m_messages.append(newMessages);
    endInsertRows();

    m_currentOffset += newMessages.size();

    if (newMessages.size() < m_pageSize) {
        m_hasMore = false;
    }
}

void MessageModel::refresh()
{
    beginResetModel();
    m_messages.clear();
    m_currentOffset = 0;
    m_hasMore = true;
    endResetModel();

    loadMoreMessages();
}
