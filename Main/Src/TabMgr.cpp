#include "TabMgr.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QThread>
#include <QUuid>
#include <iostream>
#include <string>

std::string GetLastErrorAsString()
{
    // Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string("errorMessageID is 0"); // No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = ::FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                errorMessageID,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)(&messageBuffer),
                0,
                nullptr);
    std::string message(messageBuffer, size);

    // Free the buffer.
    ::LocalFree(messageBuffer);

    return message;
}

TabMgr::TabMgr(QObject *parent) : QObject(parent)
{
    m_view.resize(1200, 800);
    m_view.setResizeMode(QQuickView::SizeRootObjectToView);
    m_view.setFlags(Qt::FramelessWindowHint | Qt::Dialog);
    m_view.rootContext()->setContextProperty("rootView", &m_view);
    m_view.rootContext()->setContextProperty("tabMgr", this);
    m_view.setSource(QUrl("qrc:/Qml/Main.qml"));
    connect(&m_view, &QQuickView::widthChanged, this, &TabMgr::syncSize);
    connect(&m_view, &QQuickView::heightChanged, this, &TabMgr::syncSize);
    connect(m_view.engine(), &QQmlEngine::quit, qApp, &QApplication::quit);
    m_view.show();
    m_ipc.Init();
    connect(&m_ipc, &IPC::readyRead, this, &TabMgr::onReadyReay);
}

TabMgr::~TabMgr()
{
    for (auto name : m_tabList)
    {
        closeTab(name);
    }
}

void TabMgr::activeTab(const QString &name)
{
    if (name == s_mainStr)
    {
        setCurrentTab(name);
        return;
    }
    auto it = m_processMap.find(name);
    if (it == m_processMap.end())
    {
        //找不到则创建进程
        QStringList args;
        args << m_ipc.GetServerName() << name;
        {
            QQuickItem *titleItem = m_view.rootObject()->findChild<QQuickItem *>("titleRect");
            Q_ASSERT(titleItem);
            QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
            Q_ASSERT(contentItem);
            QJsonObject obj{ { "x", m_view.position().x() },
                             { "y", m_view.position().y() + titleItem->height() },
                             { "w", contentItem->width() },
                             { "h", contentItem->height() } };
            args << QString(QJsonDocument(obj).toJson());
        }

        m_processMgr.createProcess(qApp->applicationDirPath() + "/Sub.exe", args);

        m_processMap[name] = 0;
        m_tabList.append(name);
        emit tabListChanged();
    }
    setCurrentTab(name);
}

void TabMgr::closeTab(const QString &name)
{
    if (name == s_mainStr)
    {
        return;
    }
    if (name == currentTab())
    {
        setCurrentTab(s_mainStr);
    }
    m_tabList.removeOne(name);
    emit tabListChanged();
    m_processMap.erase(name);
    QJsonObject obj{ { "operator", "quit" } };
    m_ipc.sendData(name, QJsonDocument(obj).toJson());
}

const QStringList &TabMgr::tabList() const
{
    return m_tabList;
}

const QString &TabMgr::currentTab() const
{
    return m_currentTab;
}

void TabMgr::showMaximized()
{
    m_view.showMaximized();
}

void TabMgr::showMinimized()
{
    m_view.showMinimized();
}

void TabMgr::showNormal()
{
    m_view.showNormal();
}

void TabMgr::setCurrentTab(const QString &currentTab)
{
    if (m_currentTab == currentTab)
    {
        return;
    }
    //新的页面不是main则raise，否则不处理
    if (currentTab != s_mainStr)
    {
        raiseSubProcess(currentTab);
    }
    //旧的页面不是main则lower，否则不处理
    if (m_currentTab != s_mainStr)
    {
        lowerSubProcess(m_currentTab);
    }
    m_currentTab = currentTab;
    emit currentTabChanged();
}

void TabMgr::onReadyReay(const QString &socketName, const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull())
    {
        qWarning() << "parseError " << error.errorString();
        return;
    }
    QJsonObject obj = doc.object();
    for (auto key : obj.keys())
    {
        if (key == QStringLiteral("winid"))
        {
            quint64 winid = obj[key].toString().toULongLong();
            qWarning() << "received winid" << winid;
            m_processMap[socketName] = winid;
            auto hw = ::SetParent(reinterpret_cast<HWND>(winid), reinterpret_cast<HWND>(m_view.winId()));
            if (nullptr == hw)
            {
                std::cout << GetLastErrorAsString() << std::endl;
                continue;
            }
            QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
            Q_ASSERT(contentItem);
            QQuickItem *titleItem = m_view.rootObject()->findChild<QQuickItem *>("titleRect");
            Q_ASSERT(titleItem);
            ::MoveWindow(reinterpret_cast<HWND>(winid),
                         0,
                         static_cast<int>(titleItem->height()),
                         static_cast<int>(contentItem->width()),
                         static_cast<int>(contentItem->height()),
                         true);
        }
    }
}

void TabMgr::syncSize()
{
    QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
    Q_ASSERT(contentItem);
    QQuickItem *titleItem = m_view.rootObject()->findChild<QQuickItem *>("titleRect");
    Q_ASSERT(titleItem);
    QJsonObject obj{
        { "operator", "resize" },
        { "x", 0},
        { "y", titleItem->height()},
        { "w", contentItem->width()},
        { "h", contentItem->height()},
    };
    QByteArray json = QJsonDocument(obj).toJson();
    for (auto tab : m_tabList) {
        m_ipc.sendData(tab, json);
    }
}

void TabMgr::raiseSubProcess(const QString &subProcessName)
{
    //    auto itor = m_processMap.find(subProcessName);
    //    if (itor != m_processMap.end())
    //    {
    //        HWND hwnd = (HWND)(itor->second);
    //        ::ShowWindow(hwnd, SW_SHOW);

    //        QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
    //        Q_ASSERT(contentItem);
    //        QQuickItem *titleItem = m_view.rootObject()->findChild<QQuickItem *>("titleRect");
    //        Q_ASSERT(titleItem);
    //        ::MoveWindow(hwnd, 0, titleItem->height(), contentItem->width(), contentItem->height(), true);

    //        qWarning() << "raise" << itor->second;
    //    }
    QJsonObject obj{ { "operator", "show" } };
    m_ipc.sendData(subProcessName, QJsonDocument(obj).toJson());
}

void TabMgr::lowerSubProcess(const QString &subProcessName)
{
    //    auto itor = m_processMap.find(subProcessName);
    //    if (itor != m_processMap.end())
    //    {
    //        HWND hwnd = (HWND)(itor->second);

    //        QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem *>("contentRect");
    //        Q_ASSERT(contentItem);

    //        ::MoveWindow(hwnd, 2000, 2000, contentItem->width(), contentItem->height(), true);

    //        ::ShowWindow(hwnd, SW_HIDE);
    //        qWarning() << "lower" << itor->second;
    //    }
    QJsonObject obj{ { "operator", "hide" } };
    m_ipc.sendData(subProcessName, QJsonDocument(obj).toJson());
}
