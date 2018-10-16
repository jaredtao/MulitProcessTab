#pragma once

#include <QObject>
#include <unordered_map>
#include <map>
#include <QQuickView>

#include "Common.h"
#include "IPC.h"
#include "ProcessMgr.h"

const static QString s_mainStr = QStringLiteral("main");
class TabMgr : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList tabList READ tabList NOTIFY tabListChanged)
    Q_PROPERTY(QString currentTab READ currentTab NOTIFY currentTabChanged)
public:
    explicit TabMgr(QObject *parent = nullptr);
    ~TabMgr();
    Q_INVOKABLE void activeTab(const QString &name);

    const QStringList &tabList() const;

    const QString &currentTab() const;

signals:

    void tabListChanged();

    void currentTabChanged();

public slots:

    void setCurrentTab(const QString &currentTab);
private slots:

    void onReadyReay(const QString &socketName, const QByteArray &data);
private:
    void raiseSubProcess(const QString &subProcessName);
    void lowerSubProcess(const QString &subProcessName);
private:

    QStringList m_tabList;
    QString m_currentTab = s_mainStr;
    QQuickView m_view;
    ProcessMgr m_processMgr;
    IPC m_ipc;

    //<processName, winid>
    std::unordered_map<QString, uint> m_processMap;
};

