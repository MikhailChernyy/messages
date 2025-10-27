#include "messagemodel.h"
#include "database.h"
#include <QSqlRecord>
#include <QDebug>

MessageModel::MessageModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

void MessageModel::setDatabase(Database *db)
{
    m_database = db;
    if (db) {
        m_dbConnection = QSqlDatabase::database();
        m_totalCount = db->getTotalMessageCount();
    }
    refresh();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    int row = index.row();

    switch (role) {
    case Qt::DisplayRole: {
        QVariant sender = QSqlQueryModel::data(createIndex(row, 0), Qt::DisplayRole);
        QVariant text = QSqlQueryModel::data(createIndex(row, 2), Qt::DisplayRole);
        return sender.toString() + ": " + text.toString();
    }
    case Qt::UserRole: // sender
        return QSqlQueryModel::data(createIndex(row, 0), Qt::DisplayRole);
    case Qt::UserRole + 1: { // timestamp
        QVariant timestamp = QSqlQueryModel::data(createIndex(row, 1), Qt::DisplayRole);
        if (timestamp.userType() == QMetaType::QDateTime) {
            return timestamp.toDateTime().toString("dd.MM.yyyy hh:mm");
        }
        return timestamp.toString();
    }
    case Qt::UserRole + 2: // text
        return QSqlQueryModel::data(createIndex(row, 2), Qt::DisplayRole);
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

void MessageModel::updateQuery()
{
    if (!m_database) {
        qWarning() << "Database not set!";
        return;
    }

    // Загружаем ВСЕ данные до текущего offset + pageSize
    QString sql = QString(
        "SELECT sender, timestamp, text FROM messages "
        "ORDER BY timestamp DESC "
        "LIMIT %1"
    ).arg(m_currentOffset + m_pageSize);

    this->setQuery(sql, m_dbConnection);

    if (this->lastError().isValid()) {
        qWarning() << "Query error:" << this->lastError().text();
    } else {
        qDebug() << "Model updated, total rows:" << rowCount()
                 << "current offset:" << m_currentOffset;
    }
}

void MessageModel::loadMoreMessages()
{
    if (!m_database || !m_hasMore)
        return;

    if (m_currentOffset + m_pageSize >= m_totalCount) {
        m_hasMore = false;
        qDebug() << "No more messages to load";
        return;
    }

    m_currentOffset += m_pageSize;
    qDebug() << "Loading more messages, new offset:" << m_currentOffset;

    updateQuery();

    if (m_currentOffset + m_pageSize >= m_totalCount) {
        m_hasMore = false;
        qDebug() << "All messages loaded";
    }
}

void MessageModel::refresh()
{
    m_currentOffset = 0;
    m_hasMore = true;
    if (m_database) {
        m_totalCount = m_database->getTotalMessageCount();
    }
    qDebug() << "Refreshing model, total messages:" << m_totalCount;
    updateQuery();
}
