#include "TabMgr.h"
#include <QDebug>
#include <QCoreApplication>
#include <QUuid>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <iostream>
TabMgr::TabMgr(QObject *parent) : QObject(parent)
{
    m_view.setResizeMode(QQuickView::SizeViewToRootObject);
    m_view.setFlags(Qt::FramelessWindowHint | Qt::Dialog);
    m_view.rootContext()->setContextProperty("rootView", &m_view);
    m_view.rootContext()->setContextProperty("tabMgr", this);
    m_view.setSource(QUrl("qrc:/Qml/Main.qml"));

    QObject::connect(m_view.engine(), &QQmlEngine::quit, qApp, &QApplication::quit);
    m_view.show();

    m_server.listen(QUuid::createUuid().toString());
    connect(&m_server, &QLocalServer::newConnection, this, &TabMgr::onNewConnection);
}

TabMgr::~TabMgr()
{
    for (auto it : m_processMap)
    {
        if (it.second->state() != QProcess::NotRunning)
        {
            it.second->kill();
        }
        it.second->deleteLater();
    }
    m_processMap.clear();
}

void TabMgr::activeTab(const QString &name)
{
    if (name == QStringLiteral("main"))
    {
        setCurrentTab(name);
        return;
    }
    auto it = m_processMap.find(name);
    if (it == m_processMap.end())
    {
        auto pro = new QProcess();
        pro->setObjectName(name);
        connectProcess(pro);
        auto env = QProcessEnvironment::systemEnvironment();
//        qWarning() << env.toStringList();
        pro->setProcessEnvironment(env);

        pro->setWorkingDirectory(qApp->applicationDirPath());
        QStringList args;
        args << m_server.serverName() << name;
        {
            QQuickItem *contentRect = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
            QPointF pos = m_view.mapToGlobal(contentRect->position().toPoint());
            QJsonObject obj
            {
                {"x", pos.x()},
                {"y", pos.y()},
                {"w", contentRect->width()},
                {"h", contentRect->height()}
            };
            args << QString(QJsonDocument(obj).toJson());
        }

        pro->start(qApp->applicationDirPath() + "/Sub.exe", args);
        m_processMap.insert({name, pro});
        m_tabList.append(name);
        emit tabListChanged();
    }
    setCurrentTab(name);
}

const QStringList &TabMgr::tabList() const
{
    return m_tabList;
}

const QString &TabMgr::currentTab() const
{
    return m_currentTab;
}

void TabMgr::setCurrentTab(const QString &currentTab)
{
    if (m_currentTab == currentTab)
    {
        return;
    }
    if (currentTab == "main")
    {
        m_view.raise();
    }
    else
    {
        raiseSubProcess(currentTab);
    }

    m_currentTab = currentTab;


    emit currentTabChanged();
}

void TabMgr::connectProcess(QProcess *pro)
{
    connect(pro, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error){
        qWarning() << "process [" << pro->objectName() << "] errorOccurred:" << error;
    });
    connect(pro, QOverload<int>::of(&QProcess::finished), this, [&](int exitCode){
        auto name = pro->objectName();
        qWarning() << "process [" << pro->objectName() << "] finished with exitCode" << exitCode;
        delete m_processMap.at(name);
        m_processMap.erase(name);
        m_tabList.removeOne(name);
        emit tabListChanged();
    });
    connect(pro, &QProcess::readyReadStandardOutput, this, [&, pro](){
       qWarning() << pro->objectName() << pro->readAllStandardOutput();
    });
    connect(pro, &QProcess::readyReadStandardError, this, [&, pro](){
       qWarning() << pro->objectName() << pro->readAllStandardError();
    });
}

void TabMgr::onNewConnection()
{
    while(m_server.hasPendingConnections())
    {
        QLocalSocket * socket = m_server.nextPendingConnection();
        connect(socket, &QLocalSocket::readyRead, this, &TabMgr::onReadyReay);
        m_socketList.push_back(socket);
    }
}

void TabMgr::onReadyReay()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket)
    {
       auto data = socket->readAll();
       QJsonParseError error;
       QJsonDocument doc = QJsonDocument::fromJson(data, &error);
       if (doc.isNull())
       {
           qWarning() << "parseError " << error.errorString();
           return;
       }
       QJsonObject obj = doc.object();
       if (obj.contains("processName"))
       {
           qWarning() << "received " << obj["processName"].toString();
            socket->setObjectName(obj["processName"].toString());

            auto hw = ::SetParent((HWND)obj["winid"].toInt(), (HWND)m_view.winId());
            if (hw == NULL)
            {
                auto d = ::GetLastError();
                std::cout << d <<std::endl;
            }
            ::MoveWindow((HWND)obj["winid"].toInt(), 0, 40, 1200, 760, false);
       }
    }
}

void TabMgr::processMessage(const QJsonObject &obj)
{

}

void TabMgr::raiseSubProcess(const QString &subProcessName)
{
    QJsonObject obj {
        {"operator", "raise"}
    };
    for (auto it : m_socketList)
    {
        qWarning() << it->objectName();
        if (it->objectName() == subProcessName)
        {
            qWarning() << "send";
            it->write(QJsonDocument(obj).toJson());
            break;
        }
    }
}
