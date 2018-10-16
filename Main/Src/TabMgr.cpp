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
    m_ipc.Init();
    connect(&m_ipc, &IPC::readyRead, this, &TabMgr::onReadyReay);
}

TabMgr::~TabMgr()
{
    m_processMap.clear();
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
        m_processMgr.createProcess(qApp->applicationDirPath() + "/Sub.exe", args);

		m_processMap.insert({ name,0 });
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
			const QString windIdStr = obj[key].toString();
			uint winid = windIdStr.toInt();
			QString nameCpy = socketName;
			m_processMap[socketName] = winid;
            auto hw = ::SetParent(reinterpret_cast<HWND>(winid), reinterpret_cast<HWND>(m_view.winId()));
            if (nullptr == hw)
            {
                auto d = ::GetLastError();
                std::cout << d <<std::endl;
            }
            QQuickItem *contentItem = m_view.rootObject()->findChild<QQuickItem*>("contentRect");
            Q_ASSERT(contentItem);
            QQuickItem *titleItem = m_view.rootObject()->findChild<QQuickItem*>("titleRect");
            Q_ASSERT(titleItem);

            ::MoveWindow((HWND)(winid), 0, titleItem->height(), contentItem->width(), contentItem->height(), false);
        }
    }
}


void TabMgr::raiseSubProcess(const QString &subProcessName)
{
    auto itor = m_processMap.find(subProcessName);
    if (itor != m_processMap.end())
    {
		HWND hwnd = (HWND)(itor->second);
        bool ret = ::ShowWindow(hwnd, SW_SHOW);
    }
}

void TabMgr::lowerSubProcess(const QString &subProcessName)
{
    auto itor = m_processMap.find(subProcessName);
    if (itor != m_processMap.end())
    {
		HWND hwnd = (HWND)(itor->second);
		bool ret = ::ShowWindow(hwnd, SW_HIDE);
		if (!ret)
		{
			std::cout << ::GetLastError() << std::endl;
		}
    }
}
