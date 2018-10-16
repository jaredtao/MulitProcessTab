#include "ProcessMgr.h"
#include <QCoreApplication>
#include <QDebug>
#include <algorithm>
ProcessMgr::ProcessMgr(QObject *parent) : QObject(parent) {}

ProcessMgr::~ProcessMgr()
{
    for (auto pro : m_processList)
    {
        if (pro->state() != QProcess::NotRunning)
        {
            pro->kill();
        }
		pro->deleteLater();
    }
    m_processList.clear();
}

void ProcessMgr::createProcess(const QString &program, const QStringList &arguments)
{
    QProcess *pro = new QProcess;
    pro->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    pro->setWorkingDirectory(qApp->applicationDirPath());
    connectProcess(pro);
    pro->start(program, arguments);
    m_processList.push_back(pro);
}

void ProcessMgr::connectProcess(QProcess *process)
{
    connect(process, &QProcess::errorOccurred, this, &ProcessMgr::onErrorOccurred);
    connect(process, &QProcess::readyReadStandardOutput, this, &ProcessMgr::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &ProcessMgr::onReadyReadStandardError);
    connect(process, QOverload<int>::of(&QProcess::finished), this, &ProcessMgr::onFinished);
}

void ProcessMgr::onFinished(int exitCode)
{
    QProcess *pro = qobject_cast<QProcess *>(sender());
    Q_ASSERT(pro);
    qInfo() << pro->program() << pro->processId() << " finished with exitcode " << exitCode;
    m_processList.erase(std::find(m_processList.begin(), m_processList.end(), pro));
}

void ProcessMgr::onErrorOccurred(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    QProcess *pro = qobject_cast<QProcess *>(sender());
    Q_ASSERT(pro);
    qWarning() << pro->program() << pro->processId() << " error: " << pro->errorString();
}

void ProcessMgr::onReadyReadStandardOutput()
{
    QProcess *pro = qobject_cast<QProcess *>(sender());
    Q_ASSERT(pro);
    qInfo() << pro->program() << pro->processId() << pro->readAllStandardOutput();
}

void ProcessMgr::onReadyReadStandardError()
{
    QProcess *pro = qobject_cast<QProcess *>(sender());
    Q_ASSERT(pro);
    qWarning() << pro->program() << pro->processId() << pro->readAllStandardError();
}
