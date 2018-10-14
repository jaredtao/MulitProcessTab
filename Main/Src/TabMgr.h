#pragma once

#include <QObject>
#include <QProcess>
#include <QLocalServer>
#include <QLocalSocket>
#include <unordered_map>
#include <vector>
#include <QQuickView>

namespace std {
template<>
struct hash<QString>
{
    std::size_t operator ()(const QString &str) const noexcept
    {
        return qHash(str);
    }
};
}
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
    void connectProcess(QProcess *pro);
    void onNewConnection();
    void onReadyReay();
private:
    void processMessage(const QJsonObject &obj);
private:
    std::unordered_map<QString, QProcess*> m_processMap;
    std::vector<QLocalSocket *> m_socketList;
    QStringList m_tabList;
    QString m_currentTab = QStringLiteral("main");
    QLocalServer m_server;
    QQuickView m_view;
};

