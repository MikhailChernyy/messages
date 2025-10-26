#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "database.h"
#include "messagemodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    Database db;
    if (!db.initDatabase()) {
        qWarning() << "Failed to initialize database";
        return -1;
    }

    MessageModel messageModel;
    messageModel.setDatabase(&db);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("messageModel", &messageModel);


    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
