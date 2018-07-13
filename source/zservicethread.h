#ifndef ZSERVICETHREAD_H
#define ZSERVICETHREAD_H

#include <QThread>
#include <udt4/src/udt.h>
#include <QImage>
#include <QTimer>
class ZServiceThread : public QThread
{
    Q_OBJECT
public:
    ZServiceThread(UDPSOCKET socket);
    ~ZServiceThread();

    qint32 ZStartThread();
    qint32 ZStopThread();
public slots:
    void ZSlotTxImg2PC(const QImage &img1,const QImage &img2);
    void ZSlotDiffXYT(QRect rectTemplate,QRect rectMatched,qint32 nDiffX,qint32 nDiffY,qint32 nCostMs);
private slots:
    void ZSlotSchedule();
protected:
    void run();
private:
    qint32 ZEncodeQImage2JPEG(const QImage &img,unsigned char **outBuffer,unsigned long *outSize);
private:
    UDPSOCKET m_socket;
    qint32 m_nDiffX,m_nDiffY,m_nCostMs;
    QRect m_rectTemp,m_rectMatched;
    QTimer *m_timer;
};

#endif // ZSERVICETHREAD_H
