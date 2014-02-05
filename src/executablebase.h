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
    explicit ExecutableBase(QObject *parent = 0);
    explicit ExecutableBase(const QFileInfo &file, QObject *parent = 0);

signals:
    void started();
    void finished();
    void terminated();

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
    QByteArray _stdin;
    QByteArray _stderr;
    QTextStream _stdinStream;
    QTextStream _stderrStream;

    virtual void setupSignals();
    virtual void parseOutput() =0;

};

#endif // EXECUTABLEBASE_H