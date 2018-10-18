#include "ProcessMgr.h"
#include <QCoreApplication>
#include <QDebug>

ProcessMgr::ProcessMgr(QObject *parent) : QObject(parent) {}

ProcessMgr::~ProcessMgr() {}

void ProcessMgr::createProcess(const QString &program, const QStringList &arguments)
{
    //创建新的QProcess，注意设置parent为this（可以让指针自动释放；可以在当前进程结束时，让子进程自动关闭）。
    QProcess *pro = new QProcess(this);
    pro->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    pro->setWorkingDirectory(qApp->applicationDirPath());
    connectProcess(pro);
    pro->start(program, arguments);
    pro->waitForStarted();
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
    pro->deleteLater();
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
