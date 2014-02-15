#include "executabletester.h"

#include <stdexcept>

ExecutableBase::ExecutableBase(const QFileInfo &file, const QStringList &args, QObject *parent) :
    QObject(parent), _proc(this), _file(file), _args(args), _stdin(), _stderr(),
    _stdinStream(&_stdin), _stderrStream(&_stderr), _status(NotRunYet)
{
    setupSignals();
}

ExecutableBase::~ExecutableBase()
{
}

ExecutableBase::ExecutableStatus ExecutableBase::getStatus()
{
    return _status;
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
    _status = Terminated;
    _proc.terminate();
}

void ExecutableBase::kill()
{
    _status = Killed;
    _proc.kill();
}

void ExecutableBase::procError(QProcess::ProcessError error)
{
    if (_status == Terminated || _status == Killed)
        return;

    bool emitFinished = _status != Running;

    switch (error) {
    case QProcess::FailedToStart:
        _status = FailedToStart;
        break;

    case QProcess::Crashed:
        _status = Crashed;
        break;

    case QProcess::Timedout:
        _status = Timedout;
        break;

    case QProcess::ReadError:
        _status = ReadError;
        break;

    case QProcess::WriteError:
        _status = WriteError;
        break;

    default:
        _status = UnknownError;
    }

    if (emitFinished)
        emit finished();
}

void ExecutableBase::procFinished(int, QProcess::ExitStatus exitStatus)
{
    if (_status == Running) {
        if (exitStatus == QProcess::NormalExit)
            _status = FinishedSuccessfully;
        else
            _status = FinishedWithError;
    }

    parseOutput();

    emit finished();
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
    _status = Running;
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
