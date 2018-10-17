#include "IPC.h"
#include <QUuid>
#include <algorithm>
#include <QJsonDocument>
#include <QJsonObject>
IPC::IPC(QObject *parent) : QObject(parent)
{
}

IPC::~IPC()
{
    m_server.close();
    m_socketMap.clear();
}

void IPC::Init()
{
    connect(&m_server, &QLocalServer::newConnection, this, &IPC::onNewConnection);
    m_server.listen(QUuid::createUuid().toString());
}

void IPC::sendData(const QString &socketName, const QByteArray &data)
{
    auto itor = m_socketMap.find(socketName);
    if (itor != m_socketMap.end())
    {
        itor->second->write(data);
    }
}

void IPC::onNewConnection()
{
    while (m_server.hasPendingConnections())
    {
        QLocalSocket *socket = m_server.nextPendingConnection();
        connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &IPC::onError);
        connect(socket, &QLocalSocket::readyRead, this, &IPC::onReadyRead);
        connect(socket, &QLocalSocket::disconnected, this, &IPC::onDisconnected);
        //这里不直接存储socket指针，收到第一次数据时，才保存
    }
}

void IPC::onError(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError)
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);
    qWarning() << "socket error " << socket->errorString();
}

void IPC::onReadyRead()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);
    QByteArray data = socket->readAll();
    if (socket->objectName().isEmpty())
    {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        if (obj.contains("processName"))
        {
            QString socketName = obj["processName"].toString();
            socket->setObjectName(socketName);
            m_socketMap.insert({socketName, socket});
        }
    }
    else
    {
        emit readyRead(socket->objectName(), data);
    }
}

void IPC::onDisconnected()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);
    qInfo() << "socket disconnected";
    auto itor = m_socketMap.find(socket->objectName());
    if (itor != m_socketMap.end())
    {
        m_socketMap.erase(itor);
    }
//    socket->deleteLater();
}
