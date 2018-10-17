#pragma once

#include <QObject>
#include <QProcess>
class ProcessMgr : public QObject
{
    Q_OBJECT
public:
    explicit ProcessMgr(QObject *parent = nullptr);
    ~ProcessMgr();
    void createProcess(const QString &program, const QStringList &arguments);
signals:

public slots:

private slots:
    void onFinished(int exitCode);
    void onErrorOccurred(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
private:
    void connectProcess(QProcess *process);

};
