#ifndef ZIMGCAPTHREAD_H
#define ZIMGCAPTHREAD_H

#include <QThread>
#include <QtGui/QImage>
#include "zcamdevice.h"
#include "zgblpara.h"
#include <QTimer>
class ZImgCapThread : public QThread
{
    Q_OBJECT
public:
    explicit ZImgCapThread(QString devNodeName,qint32 nPreWidth,qint32 nPreHeight,qint32 nPreFps);
    ~ZImgCapThread();

    qint32 ZStartThread();
    qint32 ZStopThread();

    qint32 ZGetCAMImgWidth();
    qint32 ZGetCAMImgHeight();
    qint32 ZGetCAMImgFps();

    QString ZGetDevName();
    bool ZIsRunning();

    QString ZGetCAMID();
signals:
    void ZSigCapImg(const QImage &img);
    void ZSigMsg(const QString &msg,const qint32 &type);
    void ZSigThreadFinished();
    void ZSigCAMIDFind(QString camID);
public slots:
    void ZSlotDoCapture();
    void ZSlotCheckGblStartFlag();
protected:
    void run();
private:
    qint32 convYUV2RGBPixel(qint32 y,qint32 u,qint32 v);
    qint32 convYUV2RGBBuffer(unsigned char *yuv,unsigned char *rgb,unsigned int width,unsigned int height);
private:
    QString m_devName;
    qint32 m_nPreWidth,m_nPreHeight,m_nPreFps;
    ZCAMDevice *m_cam;
    unsigned char *m_pImgData;
    unsigned char *m_pImgTemp;
private:
    QTimer *m_timerCap;
    bool m_bRunning;
private:
    QTimer *m_timerCtl;
};
#endif // ZIMGCAPTHREAD_H
