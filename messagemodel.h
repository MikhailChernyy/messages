#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H


#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QVariant>
#include <QModelIndex>
#include <QtSql/QSqlDatabase>

class Database;

class MessageModel : public QSqlQueryModel
{
    Q_OBJECT

public:

    explicit MessageModel(QObject *parent = nullptr);

    void setDatabase(Database *db);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void loadMoreMessages();
    void refresh();

private:
    Database *m_database = nullptr;
    QSqlDatabase m_dbConnection;
    int m_currentOffset = 0;
    const int m_pageSize = 20;
    bool m_hasMore = true;
    int m_totalCount;

    void updateQuery();
};

#endif // MESSAGEMODEL_H
