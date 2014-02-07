#include "executabletester.h"

#include <stdexcept>

ExecutableBase::ExecutableBase(const QFileInfo &file, const QStringList &args, QObject *parent) :
    QObject(parent), _proc(this), _file(file), _args(args), _stdin(), _stderr(),
    _stdinStream(&_stdin), _stderrStream(&_stderr)
{
    setupSignals();
}

ExecutableBase::~ExecutableBase()
{
}

void ExecutableBase::execute()
{
    if (! _file.isFile() || ! _file.isExecutable()) {
        throw std::runtime_error("Unable to run file");
    }

    _proc.start(_file.absoluteFilePath(), _args);
}

void ExecutableBase::terminate()
{
    _proc.terminate();
}

void ExecutableBase::kill()
{
    _proc.kill();
}

void ExecutableBase::procError(QProcess::ProcessError)
{
    emit terminated();
}

void ExecutableBase::procFinished(int, QProcess::ExitStatus exitStatus)
{
    parseOutput();

    if (exitStatus == QProcess::NormalExit)
        emit finished();
    else
        emit terminated();
}

void ExecutableBase::procReadyReadStandardError()
{
    _stderr.append(_proc.readAllStandardError());
}

void ExecutableBase::procReadyReadStandardOutput()
{
    _stdin.append(_proc.readAllStandardOutput());
}

void ExecutableBase::procStarted()
{
    emit started();
}

void ExecutableBase::setupSignals()
{
    connect(&_proc, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(procError(QProcess::ProcessError)));
    connect(&_proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(procFinished(int,QProcess::ExitStatus)));
    connect(&_proc, SIGNAL(readyReadStandardError()),
            this, SLOT(procReadyReadStandardError()));
    connect(&_proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(procReadyReadStandardOutput()));
    connect(&_proc, SIGNAL(started()),
            this, SLOT(procStarted()));
}
