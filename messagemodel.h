#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H


#include <QAbstractListModel>
#include <QStringList>
#include <QVariant>
#include <QModelIndex>

class Database;

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit MessageModel(QObject *parent = nullptr);

    void setDatabase(Database *db);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void loadMoreMessages();
    void refresh();

private:
    Database *m_database = nullptr;
    QStringList m_messages;
    int m_currentOffset = 0;
    const int m_pageSize = 20;
    bool m_hasMore = true;
};

#endif // MESSAGEMODEL_H
