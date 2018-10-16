#ifndef IPC_H
#define IPC_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <unordered_map>
#include "Common.h"
class IPC : public QObject
{
    Q_OBJECT
public:
    explicit IPC(QObject *parent = nullptr);
    ~IPC();
    //初始化
    void Init();
    QString GetServerName() const
    {
        return m_server.serverName();
    }
    void sendData(const QString &socketName, const QByteArray &data);
signals:
    void readyRead(const QString &socketName, const QByteArray &data);
private slots:
    void onNewConnection();
    void onError(QLocalSocket::LocalSocketError socketError);
    void onReadyRead();
    void onDisconnected();
private:
    QLocalServer m_server;
    std::unordered_map<QString, QLocalSocket *> m_socketMap;
};

#endif // IPC_H
