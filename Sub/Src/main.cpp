#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " serverSocketName processName gemotry" << std::endl;
        return -1;
    }
    QString serverSocketName = argv[1];
    QString processName = argv[2];
    QJsonDocument doc = QJsonDocument::fromJson(argv[3]);
    QJsonObject argObj = doc.object();
    int x = argObj["x"].toInt();
    int y = argObj["y"].toInt();
    int w = argObj["w"].toInt();
    int h = argObj["h"].toInt();


    std::cout << argv[1] << "started" << std::endl;
    QApplication app(argc, argv);
    QQuickView view;

    QLocalSocket socket;
    socket.connectToServer(serverSocketName);
    if (!socket.waitForConnected())
    {
        std::cout << "Connect Failed" << std::endl;
        return -2;
    }

    QObject::connect(&socket, &QLocalSocket::readyRead, [&](){
        QJsonDocument doc = QJsonDocument::fromJson(socket.readAll());
        QJsonObject obj = doc.object();
        //收到{ "operator": "raise" } 时弹起窗口
        if (obj.contains("operator") && obj["operator"].toString() == QStringLiteral("raise"))
        {
            std::cout << "received raise " << std::endl;

            view.raise();
        }
    });
    //启动时发送 {"processName": "$processName"}, 自报名称给服务器。服务器用来做索引。
    QJsonObject processNameObj {
        {"processName", QString(processName)},
        {"winid", (int)view.winId()}
    };
    socket.write(QJsonDocument(processNameObj).toJson());

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFlags(Qt::FramelessWindowHint);
    view.setGeometry(x, y, w, h);
    view.setSource(QUrl("qrc:/Qml/Main.qml"));
    view.show();
    app.exec();
}
