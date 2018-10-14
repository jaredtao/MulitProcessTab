#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        qWarning() << "Usage: " << argv[0] << " serverSocketName processName gemotry";
        return -1;
    }
    QString serverSocketName = argv[1];
    QString processName = argv[2];
    QJsonDocument doc = QJsonDocument::fromJson(argv[3]);
    QJsonObject obj = doc.object();
    int x = obj["x"].toInt();
    int y = obj["y"].toInt();
    int w = obj["w"].toInt();
    int h = obj["h"].toInt();

    QLocalSocket socket;
    socket.connectToServer(serverSocketName);
    if (!socket.waitForConnected())
    {
        qWarning() << "Connect Failed";
        return -2;
    }

    QApplication app(argc, argv);
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFlags(Qt::FramelessWindowHint);
    view.setGeometry(x, y, w, h);
    view.setSource(QUrl("qrc:/Qml/Main.qml"));
    view.show();
    app.exec();
}
