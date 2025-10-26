#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool initDatabase();
    void createAndConnectDatabase();
    QStringList getMessages(int offset, int limit);
    int getTotalMessageCount();

private:
    QSqlDatabase m_db;
    void createTables();
    void populateTestData();
};

#endif // DATABASE_H
