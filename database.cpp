#include <QRandomGenerator>
#include "database.h"

Database::Database(QObject *parent) : QObject(parent)
{

}

Database::~Database()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("localhost");
    m_db.setDatabaseName("message_db");
    m_db.setUserName("mcher");
    m_db.setPassword("1234");
    m_db.setPort(5432);

    if (!m_db.open()) {
        qWarning() << "Database does not exists";
        createAndConnectDatabase();
    }

    createTables();
    return true;
}

void Database::createAndConnectDatabase()
{
    QSqlDatabase adminDb = QSqlDatabase::addDatabase("QPSQL", "admin_connection");
            adminDb.setHostName("localhost");
            adminDb.setDatabaseName("postgres");
            adminDb.setUserName("postgres");
            adminDb.setPassword("1234");
            adminDb.setPort(5432);

            if (adminDb.open()) {
                QSqlQuery query(adminDb);
                if (!query.exec("CREATE DATABASE message_db")) {
                    qWarning() << "Failed to create database:" << query.lastError().text();
                    return;
                }
                adminDb.close();
                initDatabase();
            }
}

void Database::createTables()
{
    QSqlQuery query;

    if (!query.exec("CREATE TABLE IF NOT EXISTS messages ("
                    "id SERIAL PRIMARY KEY,"
                    "sender VARCHAR(100) NOT NULL,"
                    "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                    "text TEXT NOT NULL)")) {
        qWarning() << "Failed to create message table:" << query.lastError().text();
        return;
    }

    query.exec("SELECT COUNT(*) FROM messages");
    if (query.next() && query.value(0).toInt() == 0) {
        populateTestData();
    }
}

void Database::populateTestData()
{
    QSqlQuery query;

    QStringList senders = {"Алиса", "Боб", "Карл", "Дина", "Ева", "Франк", "Грейс"};
    QStringList messageTemplates = {
        "Привет! Как дела?",
        "Сегодня отличная погода",
        "Напомни мне о встрече",
        "Отправил документы на проверку",
        "Спасибо за помощь с проектом",
        "Когда будет готов отчет?",
        "Обсудим это на следующей неделе",
        "Получил твое сообщение, сбасибо",
        "Нужна твоя консультация по вопросу",
        "Поздравляю с успешным завершением"
    };

    query.prepare("INSERT INTO messages (sender, timestamp, text)"
                  "VALUES (:sender, :timestamp, :text)");

    QDateTime baseTime = QDateTime::currentDateTime();

    for (int i = 0; i < 150; ++i) {
        QString sender = senders.at(QRandomGenerator::global()->bounded(senders.size()));
        QString text = messageTemplates.at(QRandomGenerator::global()->bounded(messageTemplates.size()));
        QDateTime timestamp = baseTime.addSecs(-i * 3600);

        query.bindValue(":sender", sender);
        query.bindValue(":timestamp", timestamp);
        query.bindValue(":text", text + " (#" +QString::number(i + 1) + ")");

        if (!query.exec()) {
            qWarning() << "Failed to insert test data:" << query.lastError().text();
        }
    }

    qDebug() << "Test data populated successfuly";
}

QStringList Database::getMessages(int offset, int limit)
{
    QStringList messages;

    QSqlQuery query;
    query.prepare("SELECT sender, timestamp, text FROM messages "
                  "ORDER BY timestamp DESC "
                  "LIMIT :limit OFFSET :offset");
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        qWarning() <<"Failed to get messages: " << query.lastError().text();
        return messages;
    }

    while (query.next()) {
            QString message = query.value(0).toString() + "|" +
                             query.value(1).toDateTime().toString("dd.MM.yyyy hh:mm") + "|" +
                             query.value(2).toString();
            messages.append(message);
        }

        return messages;
    }

int Database::getTotalMessageCount()
{
    QSqlQuery query("SELECT COUNT(*) FROM messages");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
