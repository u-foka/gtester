#ifndef EXECUTABLEBASE_H
#define EXECUTABLEBASE_H

#include <QObject>
#include <QProcess>
#include <QFileInfo>
#include <QTextStream>

class ExecutableBase : public QObject
{
    Q_OBJECT
public:
    ExecutableBase(const QFileInfo &file, const QStringList &args, QObject *parent);
    virtual ~ExecutableBase();

    enum ExecutableStatus {
        NotRunYet,
        Running,
        FinishedSuccessfully,
        FinishedWithError,
        Terminated,
        Killed,
        FailedToStart,
        Crashed,
        Timedout,
        ReadError,
        WriteError,
        UnknownError
    };

    ExecutableStatus getStatus();

signals:
    void started();
    void finished();

public slots:
    void execute();
    void terminate();
    void kill();

private slots:
    void procError(QProcess::ProcessError error);
    void procFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void procReadyReadStandardError();
    void procReadyReadStandardOutput();
    void procStarted();

protected:
    QProcess _proc;
    QFileInfo _file;
    QStringList _args;
    QByteArray _stdin;
    QByteArray _stderr;
    QTextStream _stdinStream;
    QTextStream _stderrStream;
    ExecutableStatus _status;

    virtual void setupSignals();
    virtual void parseOutput() =0;

};

#endif // EXECUTABLEBASE_H
