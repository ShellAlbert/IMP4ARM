#include "zservicethread.h"
#include <QDebug>
#include <QImage>
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDateTime>
#include <udt4/src/udt.h>
#include "zgblpara.h"
#include "jpeglib.h"
void int2char4(const int intVal,char *pChar)
{
    char *pIntVal=(char*)&intVal;
    *(pChar+0)=*(pIntVal+3);
    *(pChar+1)=*(pIntVal+2);
    *(pChar+2)=*(pIntVal+1);
    *(pChar+3)=*(pIntVal+0);
}
ZServiceThread::ZServiceThread(UDPSOCKET socket)
{
    this->m_socket=socket;
}
ZServiceThread::~ZServiceThread()
{
    qDebug()<<"service thread destroyed.";
}
qint32 ZServiceThread::ZStartThread()
{
    this->start();
    return 0;
}
qint32 ZServiceThread::ZStopThread()
{
    this->exit(0);
    return 0;
}
void ZServiceThread::run()
{
    this->m_timer=new QTimer;
    connect(this->m_timer,SIGNAL(timeout()),this,SLOT(ZSlotSchedule()),Qt::DirectConnection);
    this->m_timer->start(1000);
    //enter event-loop until exit() was called.
    this->exec();
    UDT::close(this->m_socket);

    this->m_timer->stop();
    delete this->m_timer;
    qDebug()<<"thread service exit okay.";
}
void ZServiceThread::ZSlotSchedule()
{
    if(gGblPara.m_bGblRst2Exit)
    {
        this->exit(0);
    }
}
void ZServiceThread::ZSlotDiffXYT(QRect rectTemplate,QRect rectMatched,qint32 nDiffX,qint32 nDiffY,qint32 nCostMs)
{
    this->m_rectTemp=rectTemplate;
    this->m_rectMatched=rectMatched;
    this->m_nDiffX=nDiffX;
    this->m_nDiffY=nDiffY;
    this->m_nCostMs=nCostMs;
}
qint32 ZServiceThread::ZEncodeQImage2JPEG(const QImage &img,unsigned char **outBuffer,unsigned long *outSize)
{
    //tx image1 data body with libjpeg compressed.
    JSAMPLE *pTxRGBData=(JSAMPLE*)img.constBits();
    JSAMPROW row_pointer[1];
    qint32 row_stride;

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err=jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_mem_dest(&cinfo,outBuffer,outSize);
    cinfo.image_width=img.width();
    cinfo.image_height=img.height();
    cinfo.input_components=3;//RGB is 3,gray is 1.
    cinfo.in_color_space=JCS_RGB;
    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo,50,true);
    jpeg_start_compress(&cinfo,true);

    row_stride=img.width()*3;//RGB.
    while(cinfo.next_scanline<cinfo.image_height)
    {
        row_pointer[0]=&pTxRGBData[cinfo.next_scanline*row_stride];
        (void)jpeg_write_scanlines(&cinfo,row_pointer,1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return 0;
}
#if 0
void ZServiceThread::ZSlotTxImg2PC(const QImage &img1,const QImage &img2)
{
    char *pTxData;
    qint32 nTxTotal1,nTxTotal2;
    qint32 nTxBytes;
    ////////////////////////////////////////////////
    nTxTotal1=img1.byteCount();
    nTxTotal2=img2.byteCount();
    //send total bytes and width*height.
    //nDiffx,nDiffY,nCostMs.
    char txTotalBuffer[84];
    int2char4(this->m_nDiffX,&txTotalBuffer[0]);//4bytes.
    int2char4(this->m_nDiffY,&txTotalBuffer[4]);//4bytes.
    int2char4(this->m_nCostMs,&txTotalBuffer[8]);//4bytes.
    int2char4(img1.width(),&txTotalBuffer[12]);//4 bytes.
    int2char4(img1.height(),&txTotalBuffer[16]);//4bytes.
    int2char4(nTxTotal1,&txTotalBuffer[20]);//4 bytes.
    int2char4(img2.width(),&txTotalBuffer[24]);//4 bytes.
    int2char4(img2.height(),&txTotalBuffer[28]);//4bytes.
    int2char4(nTxTotal2,&txTotalBuffer[32]);//4 bytes.
    //calibrate (x1,y1).
    int2char4(gGblPara.m_calibrateX1,&txTotalBuffer[36]);//4 bytes.
    int2char4(gGblPara.m_calibrateY1,&txTotalBuffer[40]);//4 bytes.
    //calibrate (x2,y2).
    int2char4(gGblPara.m_calibrateX2,&txTotalBuffer[44]);//4 bytes.
    int2char4(gGblPara.m_calibrateY2,&txTotalBuffer[48]);//4 bytes.

    //rect template.(x,y,width,height).
    int2char4(this->m_rectTemp.x(),&txTotalBuffer[52]);//4 bytes.
    int2char4(this->m_rectTemp.y(),&txTotalBuffer[56]);//4 bytes.
    int2char4(this->m_rectTemp.width(),&txTotalBuffer[60]);//4 bytes.
    int2char4(this->m_rectTemp.height(),&txTotalBuffer[64]);//4 bytes.
    //rect matched.
    int2char4(this->m_rectMatched.x(),&txTotalBuffer[68]);//4 bytes.
    int2char4(this->m_rectMatched.y(),&txTotalBuffer[72]);//4 bytes.
    int2char4(this->m_rectMatched.width(),&txTotalBuffer[76]);//4 bytes.
    int2char4(this->m_rectMatched.height(),&txTotalBuffer[80]);//4 bytes.
#ifdef USE_TCP
    UDT::TRACEINFO trace;
    UDT::perfmon(this->m_socket,&trace);

    //tx header.
    qint32 nTxHeadSize=sizeof(txTotalBuffer);
    nTxBytes=0;
    while(nTxBytes<nTxHeadSize)
    {
        qint32 nTxSingle=UDT::send(this->m_socket,txTotalBuffer+nTxBytes,nTxHeadSize-nTxBytes,0);
        if(nTxSingle<0)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->exit(-1);
            return;
        }
        nTxBytes+=nTxSingle;
    }
    //qDebug("%d,%x\n",nTxTotal,nTxTotal);
    //qDebug("%02x %02x %02x %02x\n",txTotalBuffer[0],txTotalBuffer[1],txTotalBuffer[2],txTotalBuffer[3]);

    //tx image1 data body.
    nTxBytes=0;
    pTxData=(char*)img1.constBits();
    while(nTxBytes<nTxTotal1)
    {
        qint32 nTxSingle=UDT::send(this->m_socket,pTxData+nTxBytes,nTxTotal1-nTxBytes,0);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->exit(-1);
            return;
        }
        nTxBytes+=nTxSingle;
    }

    //tx image2 data body.
    nTxBytes=0;
    pTxData=(char*)img2.constBits();
    while(nTxBytes<nTxTotal2)
    {
        qint32 nTxSingle=UDT::send(this->m_socket,pTxData+nTxBytes,nTxTotal2-nTxBytes,0);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->exit(-1);
            return;
        }
        nTxBytes+=nTxSingle;
    }

    UDT::perfmon(this->m_socket,&trace);
    qDebug()<<"speed="<<trace.mbpsSendRate<<"Mb/s";
#else

    UDT::TRACEINFO trace;
    if(gGblPara.m_bTransferSpeedMonitor)
    {
        UDT::perfmon(this->m_socket,&trace);
    }

    //tx header.
    qint32 nTxHeadSize=sizeof(txTotalBuffer);
    nTxBytes=0;
    while(nTxBytes<nTxHeadSize)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,txTotalBuffer+nTxBytes,nTxHeadSize-nTxBytes,-1,true);
        if(nTxSingle<0)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    //qDebug("%d,%x\n",nTxTotal,nTxTotal);
    //qDebug("%02x %02x %02x %02x\n",txTotalBuffer[0],txTotalBuffer[1],txTotalBuffer[2],txTotalBuffer[3]);

    //tx image1 data body.
    nTxBytes=0;
    pTxData=(char*)img1.constBits();
    while(nTxBytes<nTxTotal1)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,pTxData+nTxBytes,nTxTotal1-nTxBytes,-1,true);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    //tx image2 data body.
    nTxBytes=0;
    pTxData=(char*)img2.constBits();
    while(nTxBytes<nTxTotal2)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,pTxData+nTxBytes,nTxTotal2-nTxBytes,-1,true);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    if(gGblPara.m_bTransferSpeedMonitor)
    {
        UDT::perfmon(this->m_socket,&trace);
        qDebug()<<"speed="<<trace.mbpsSendRate<<"Mb/s";
    }
#endif
    return;
}
#endif
void ZServiceThread::ZSlotTxImg2PC(const QImage &img1,const QImage &img2)
{
    unsigned char *outBuffer1,*outBuffer2;
    unsigned long outSize1,outSize2;
    this->ZEncodeQImage2JPEG(img1,&outBuffer1,&outSize1);
    this->ZEncodeQImage2JPEG(img2,&outBuffer2,&outSize2);
    if(outSize1<=0 || outSize2<=0)
    {
        qDebug()<<"<error>:encode QImage to Jpeg";
        return;
    }

    //send total bytes and width*height.
    //nDiffx,nDiffY,nCostMs.
    char txTotalBuffer[84];
    int2char4(this->m_nDiffX,&txTotalBuffer[0]);//4bytes.
    int2char4(this->m_nDiffY,&txTotalBuffer[4]);//4bytes.
    int2char4(this->m_nCostMs,&txTotalBuffer[8]);//4bytes.
    int2char4(img1.width(),&txTotalBuffer[12]);//4 bytes.
    int2char4(img1.height(),&txTotalBuffer[16]);//4bytes.
    int2char4(outSize1,&txTotalBuffer[20]);//4 bytes.
    int2char4(img2.width(),&txTotalBuffer[24]);//4 bytes.
    int2char4(img2.height(),&txTotalBuffer[28]);//4bytes.
    int2char4(outSize2,&txTotalBuffer[32]);//4 bytes.
    //calibrate (x1,y1).
    int2char4(gGblPara.m_calibrateX1,&txTotalBuffer[36]);//4 bytes.
    int2char4(gGblPara.m_calibrateY1,&txTotalBuffer[40]);//4 bytes.
    //calibrate (x2,y2).
    int2char4(gGblPara.m_calibrateX2,&txTotalBuffer[44]);//4 bytes.
    int2char4(gGblPara.m_calibrateY2,&txTotalBuffer[48]);//4 bytes.

    //rect template.(x,y,width,height).
    int2char4(this->m_rectTemp.x(),&txTotalBuffer[52]);//4 bytes.
    int2char4(this->m_rectTemp.y(),&txTotalBuffer[56]);//4 bytes.
    int2char4(this->m_rectTemp.width(),&txTotalBuffer[60]);//4 bytes.
    int2char4(this->m_rectTemp.height(),&txTotalBuffer[64]);//4 bytes.
    //rect matched.
    int2char4(this->m_rectMatched.x(),&txTotalBuffer[68]);//4 bytes.
    int2char4(this->m_rectMatched.y(),&txTotalBuffer[72]);//4 bytes.
    int2char4(this->m_rectMatched.width(),&txTotalBuffer[76]);//4 bytes.
    int2char4(this->m_rectMatched.height(),&txTotalBuffer[80]);//4 bytes.

    UDT::TRACEINFO trace;
    if(gGblPara.m_bTransferSpeedMonitor)
    {
        UDT::perfmon(this->m_socket,&trace);
    }

    //tx header.
    qint32 nTxHeadSize=sizeof(txTotalBuffer);
    qint32 nTxBytes=0;
    while(nTxBytes<nTxHeadSize)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,txTotalBuffer+nTxBytes,nTxHeadSize-nTxBytes,-1,true);
        if(nTxSingle<0)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    //qDebug("%d,%x\n",nTxTotal,nTxTotal);
    //qDebug("%02x %02x %02x %02x\n",txTotalBuffer[0],txTotalBuffer[1],txTotalBuffer[2],txTotalBuffer[3]);

    //tx image1 data body.
    nTxBytes=0;
    while(nTxBytes<outSize1)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,(const char*)(outBuffer1+nTxBytes),outSize1-nTxBytes,-1,true);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    //tx image2 data body.
    nTxBytes=0;
    while(nTxBytes<outSize2)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,(const char*)(outBuffer2+nTxBytes),outSize2-nTxBytes,-1,true);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            return;
        }
        nTxBytes+=nTxSingle;
    }

    if(gGblPara.m_bTransferSpeedMonitor)
    {
        UDT::perfmon(this->m_socket,&trace);
        qDebug()<<"speed="<<trace.mbpsSendRate<<"Mb/s";
    }
    return;
}
