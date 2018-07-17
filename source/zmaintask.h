#ifndef ZMAINTASK_H
#define ZMAINTASK_H

#include <QObject>
#include <zimgcapthread.h>
#include <zimgprocessthread.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QPaintEvent>
#include <QImage>
#include <QtWidgets/QWidget>
#include <QLabel>
#include "zimgdisplayer.h"
#include <QProgressBar>
#include "zgblpara.h"
#include "zserverthread.h"
#ifdef BUILD_GUI
class ZMainTask : public QFrame
{

    Q_OBJECT
public:
    explicit ZMainTask(QWidget *parent = nullptr);
    ~ZMainTask();
#else
class ZMainTask : public QObject
{

    Q_OBJECT
public:
    explicit ZMainTask(QObject *parent = nullptr);
    ~ZMainTask();
#endif
    qint32 ZDoInit();
signals:

public slots:
    void ZSlotMsg(const QString &msg,const qint32 &type);
    void ZSlotSubThreadFinished();
    void ZSlotDiffXYT(QRect rectTemp,QRect rectMatched,qint32 nDiffX,qint32 nDiffY,qint32 nCostMs);
    void ZSlotSSIMImgSimilarity(qint32 nVal);
    void ZSlot1sTimeout();
private:
    void ZUpdateMatchBar(QProgressBar *pBar,qint32 nVal);
private:
    ZImgCapThread *m_cap1;
    ZImgCapThread *m_cap2;
    ZImgProcessThread *m_process;
    ZServerThread *m_video2PC;
#ifdef BUILD_GUI
    QLabel *m_llDiffXY;
    QLabel *m_llRunSec;
    QTimer *m_timer;
    qint64 m_nTimeCnt;
    QLabel *m_llState;
    QHBoxLayout *m_hLayoutInfo;
    QProgressBar *m_SSIMMatchBar;
    ZImgDisplayer *m_disp[2];
    QHBoxLayout *m_hLayout;
    QVBoxLayout *m_vLayout;
#endif
};

#endif // ZMAINTASK_H
