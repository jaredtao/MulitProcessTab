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
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " serverSocketName processName" << std::endl;
        return -1;
    }
    QString serverSocketName = argv[1];
    QString processName = argv[2];

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

    //启动时发送 {"processName": "$processName"}, 自报名称给服务器。服务器用来做索引。
    QJsonObject processNameObj {
        {"processName", QString(processName)},
    };
    socket.write(QJsonDocument(processNameObj).toJson());

    //发送句柄给服务器，之后view的坐标、宽高及隐藏显示 都由server进程控制
    QJsonObject winIdObj {
        {"winid", QString::number((int)view.winId())}
    };
	std::cout << "send winid" << (int)view.winId() << std::endl;
    socket.write(QJsonDocument(winIdObj).toJson());
    QObject::connect(&socket, &QLocalSocket::readyRead, [&](){
        QJsonDocument doc = QJsonDocument::fromJson(socket.readAll());
        qWarning() << "received " << doc.toJson();
        QJsonObject obj = doc.object();
        for (auto key : obj.keys())
        {
            if (key == QStringLiteral("operator") && obj[key].toString() == QStringLiteral("quit"))
            {
                std::cout << "quit" << std::endl;
                app.quit();
            }
        }
    });
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFlags(Qt::FramelessWindowHint);
    view.setSource(QUrl("qrc:/Qml/Main.qml"));
    view.show();
    app.exec();
}
