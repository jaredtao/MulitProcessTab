#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <iostream>
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " serverSocketName processName geometry" << std::endl;
        return -1;
    }
    QString serverSocketName = argv[1];
    QString processName = argv[2];
    QString geometryStr = argv[3];
    QJsonDocument geometrydoc = QJsonDocument::fromJson(geometryStr.toUtf8());
    QJsonObject geometryObj = geometrydoc.object();
    int x = geometryObj["x"].toInt();
    int y = geometryObj["y"].toInt();
    int w = geometryObj["w"].toInt();
    int h = geometryObj["h"].toInt();

    std::cout << argv[1] << "started" << std::endl;
    QApplication app(argc, argv);
    QQuickView view;
    view.setGeometry(x, y, w, h);
    QLocalSocket socket;
    socket.connectToServer(serverSocketName);
    if (!socket.waitForConnected())
    {
        std::cout << "Connect Failed" << std::endl;
        return -2;
    }

    //启动时发送 {"processName": "$processName"}, 自报名称给服务器。服务器用来做索引。
    QJsonObject processNameObj{
        { "processName", QString(processName) },
    };
    socket.write(QJsonDocument(processNameObj).toJson());

    //发送句柄给服务器，之后view的坐标、宽高及隐藏显示 都由server进程控制
    QJsonObject winIdObj{ { "winid", QString::number(static_cast<quint64>(view.winId())) } };
    std::cout << "send winid" << static_cast<quint64>(view.winId()) << std::endl;
    socket.write(QJsonDocument(winIdObj).toJson());
    QObject::connect(&socket, &QLocalSocket::readyRead, [&]() {
        QJsonDocument doc = QJsonDocument::fromJson(socket.readAll());
        QJsonObject obj = doc.object();
        for (auto key : obj.keys())
        {
            if (key == QStringLiteral("operator"))
            {
                if (obj[key].toString() == QStringLiteral("quit"))
                {
                    std::cout << "quit" << std::endl;
                    app.quit();
                }
                else if (obj[key].toString() == QStringLiteral("show"))
                {
                    std::cout << "show" << std::endl;
                    view.show();
                }
                else if (obj[key].toString() == QStringLiteral("hide"))
                {
                    std::cout << "hide" << std::endl;
                    view.hide();
                }
                else if (obj[key].toString() == QStringLiteral("resize"))
                {
                    int x = obj["x"].toInt();
                    int y = obj["y"].toInt();
                    int w = obj["w"].toInt();
                    int h = obj["h"].toInt();
                    view.setGeometry(x, y, w, h);
                    std::cout << "resize " << x << " " << y << " " << w << " " << h << std::endl;
                }
            }
        }
    });
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFlags(Qt::FramelessWindowHint | Qt::ForeignWindow);
    view.setSource(QUrl("qrc:/Qml/Main.qml"));
    view.show();
    app.exec();
}
