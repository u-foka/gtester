#ifndef EXECUTETHREAD_H
#define EXECUTETHREAD_H

#include <QThread>

class ExecuteThread : public QThread
{
    Q_OBJECT
public:
    explicit ExecuteThread(QObject *parent = 0);

signals:

public slots:

protected:
    void run();

};

#endif // EXECUTETHREAD_H
