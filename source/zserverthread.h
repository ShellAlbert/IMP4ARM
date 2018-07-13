#ifndef ZSERVERTHREAD_H
#define ZSERVERTHREAD_H
#include <QThread>
#include <udt4/src/udt.h>
#include <zservicethread.h>
#include <zimgprocessthread.h>
#include <zimgcapthread.h>
#include <unistd.h>
#include <string.h>
#include <QDebug>
#include <QVector>
#include <QTimer>
class ZServerThread : public QThread
{
    Q_OBJECT
public:
    ZServerThread(ZImgProcessThread *processThread);

    qint32 ZStartThread();
    qint32 ZStopThread();
    bool ZIsRunning();
signals:
    void ZSigThreadFinished();
private slots:
    void ZSlotTimeoutSchedule();
protected:
    void run();
private:
    ZImgProcessThread *m_processThread;
    QTimer *m_timer;
    UDTSOCKET m_serv;
    bool m_bRunning;
};

#endif // ZSERVERTHREAD_H
