#include "zimgcapthread.h"
#include <QDateTime>
#include <QDebug>
short redAdjust[] = {
    -161,-160,-159,-158,-157,-156,-155,-153,
    -152,-151,-150,-149,-148,-147,-145,-144,
    -143,-142,-141,-140,-139,-137,-136,-135,
    -134,-133,-132,-131,-129,-128,-127,-126,
    -125,-124,-123,-122,-120,-119,-118,-117,
    -116,-115,-114,-112,-111,-110,-109,-108,
    -107,-106,-104,-103,-102,-101,-100, -99,
    -98, -96, -95, -94, -93, -92, -91, -90,
    -88, -87, -86, -85, -84, -83, -82, -80,
    -79, -78, -77, -76, -75, -74, -72, -71,
    -70, -69, -68, -67, -66, -65, -63, -62,
    -61, -60, -59, -58, -57, -55, -54, -53,
    -52, -51, -50, -49, -47, -46, -45, -44,
    -43, -42, -41, -39, -38, -37, -36, -35,
    -34, -33, -31, -30, -29, -28, -27, -26,
    -25, -23, -22, -21, -20, -19, -18, -17,
    -16, -14, -13, -12, -11, -10,  -9,  -8,
    -6,  -5,  -4,  -3,  -2,  -1,   0,   1,
    2,   3,   4,   5,   6,   7,   9,  10,
    11,  12,  13,  14,  15,  17,  18,  19,
    20,  21,  22,  23,  25,  26,  27,  28,
    29,  30,  31,  33,  34,  35,  36,  37,
    38,  39,  40,  42,  43,  44,  45,  46,
    47,  48,  50,  51,  52,  53,  54,  55,
    56,  58,  59,  60,  61,  62,  63,  64,
    66,  67,  68,  69,  70,  71,  72,  74,
    75,  76,  77,  78,  79,  80,  82,  83,
    84,  85,  86,  87,  88,  90,  91,  92,
    93,  94,  95,  96,  97,  99, 100, 101,
    102, 103, 104, 105, 107, 108, 109, 110,
    111, 112, 113, 115, 116, 117, 118, 119,
    120, 121, 123, 124, 125, 126, 127, 128,
};

short greenAdjust1[] = {
    34,  34,  33,  33,  32,  32,  32,  31,
    31,  30,  30,  30,  29,  29,  28,  28,
    28,  27,  27,  27,  26,  26,  25,  25,
    25,  24,  24,  23,  23,  23,  22,  22,
    21,  21,  21,  20,  20,  19,  19,  19,
    18,  18,  17,  17,  17,  16,  16,  15,
    15,  15,  14,  14,  13,  13,  13,  12,
    12,  12,  11,  11,  10,  10,  10,   9,
    9,   8,   8,   8,   7,   7,   6,   6,
    6,   5,   5,   4,   4,   4,   3,   3,
    2,   2,   2,   1,   1,   0,   0,   0,
    0,   0,  -1,  -1,  -1,  -2,  -2,  -2,
    -3,  -3,  -4,  -4,  -4,  -5,  -5,  -6,
    -6,  -6,  -7,  -7,  -8,  -8,  -8,  -9,
    -9, -10, -10, -10, -11, -11, -12, -12,
    -12, -13, -13, -14, -14, -14, -15, -15,
    -16, -16, -16, -17, -17, -17, -18, -18,
    -19, -19, -19, -20, -20, -21, -21, -21,
    -22, -22, -23, -23, -23, -24, -24, -25,
    -25, -25, -26, -26, -27, -27, -27, -28,
    -28, -29, -29, -29, -30, -30, -30, -31,
    -31, -32, -32, -32, -33, -33, -34, -34,
    -34, -35, -35, -36, -36, -36, -37, -37,
    -38, -38, -38, -39, -39, -40, -40, -40,
    -41, -41, -42, -42, -42, -43, -43, -44,
    -44, -44, -45, -45, -45, -46, -46, -47,
    -47, -47, -48, -48, -49, -49, -49, -50,
    -50, -51, -51, -51, -52, -52, -53, -53,
    -53, -54, -54, -55, -55, -55, -56, -56,
    -57, -57, -57, -58, -58, -59, -59, -59,
    -60, -60, -60, -61, -61, -62, -62, -62,
    -63, -63, -64, -64, -64, -65, -65, -66,
};

short greenAdjust2[] = {
    74,  73,  73,  72,  71,  71,  70,  70,
    69,  69,  68,  67,  67,  66,  66,  65,
    65,  64,  63,  63,  62,  62,  61,  60,
    60,  59,  59,  58,  58,  57,  56,  56,
    55,  55,  54,  53,  53,  52,  52,  51,
    51,  50,  49,  49,  48,  48,  47,  47,
    46,  45,  45,  44,  44,  43,  42,  42,
    41,  41,  40,  40,  39,  38,  38,  37,
    37,  36,  35,  35,  34,  34,  33,  33,
    32,  31,  31,  30,  30,  29,  29,  28,
    27,  27,  26,  26,  25,  24,  24,  23,
    23,  22,  22,  21,  20,  20,  19,  19,
    18,  17,  17,  16,  16,  15,  15,  14,
    13,  13,  12,  12,  11,  11,  10,   9,
    9,   8,   8,   7,   6,   6,   5,   5,
    4,   4,   3,   2,   2,   1,   1,   0,
    0,   0,  -1,  -1,  -2,  -2,  -3,  -4,
    -4,  -5,  -5,  -6,  -6,  -7,  -8,  -8,
    -9,  -9, -10, -11, -11, -12, -12, -13,
    -13, -14, -15, -15, -16, -16, -17, -17,
    -18, -19, -19, -20, -20, -21, -22, -22,
    -23, -23, -24, -24, -25, -26, -26, -27,
    -27, -28, -29, -29, -30, -30, -31, -31,
    -32, -33, -33, -34, -34, -35, -35, -36,
    -37, -37, -38, -38, -39, -40, -40, -41,
    -41, -42, -42, -43, -44, -44, -45, -45,
    -46, -47, -47, -48, -48, -49, -49, -50,
    -51, -51, -52, -52, -53, -53, -54, -55,
    -55, -56, -56, -57, -58, -58, -59, -59,
    -60, -60, -61, -62, -62, -63, -63, -64,
    -65, -65, -66, -66, -67, -67, -68, -69,
    -69, -70, -70, -71, -71, -72, -73, -73,
};

short blueAdjust[] = {
    -276,-274,-272,-270,-267,-265,-263,-261,
    -259,-257,-255,-253,-251,-249,-247,-245,
    -243,-241,-239,-237,-235,-233,-231,-229,
    -227,-225,-223,-221,-219,-217,-215,-213,
    -211,-209,-207,-204,-202,-200,-198,-196,
    -194,-192,-190,-188,-186,-184,-182,-180,
    -178,-176,-174,-172,-170,-168,-166,-164,
    -162,-160,-158,-156,-154,-152,-150,-148,
    -146,-144,-141,-139,-137,-135,-133,-131,
    -129,-127,-125,-123,-121,-119,-117,-115,
    -113,-111,-109,-107,-105,-103,-101, -99,
    -97, -95, -93, -91, -89, -87, -85, -83,
    -81, -78, -76, -74, -72, -70, -68, -66,
    -64, -62, -60, -58, -56, -54, -52, -50,
    -48, -46, -44, -42, -40, -38, -36, -34,
    -32, -30, -28, -26, -24, -22, -20, -18,
    -16, -13, -11,  -9,  -7,  -5,  -3,  -1,
    0,   2,   4,   6,   8,  10,  12,  14,
    16,  18,  20,  22,  24,  26,  28,  30,
    32,  34,  36,  38,  40,  42,  44,  46,
    49,  51,  53,  55,  57,  59,  61,  63,
    65,  67,  69,  71,  73,  75,  77,  79,
    81,  83,  85,  87,  89,  91,  93,  95,
    97,  99, 101, 103, 105, 107, 109, 112,
    114, 116, 118, 120, 122, 124, 126, 128,
    130, 132, 134, 136, 138, 140, 142, 144,
    146, 148, 150, 152, 154, 156, 158, 160,
    162, 164, 166, 168, 170, 172, 175, 177,
    179, 181, 183, 185, 187, 189, 191, 193,
    195, 197, 199, 201, 203, 205, 207, 209,
    211, 213, 215, 217, 219, 221, 223, 225,
    227, 229, 231, 233, 235, 238, 240, 242,
};
//判断范围
unsigned char clip(int val)
{
    if(val > 255)
    {
        return 255;
    }
    else if(val > 0)
    {
        return val;
    }
    else
    {
        return 0;
    }
}
//查表法YUV TO RGB
int YUYVToRGB_table(unsigned char *yuv, unsigned char *rgb, unsigned int width,unsigned int height)
{
    short y1=0, y2=0, u=0, v=0;
    unsigned char *pYUV = yuv;
    unsigned char *pGRB = rgb;
    int i=0;
    //int y=0,x=0,in=0,y0,out=0;
    int count =width * height /2;//width * height *2/4;

    for(i = 0; i < count; i++)
    {
        y1 = *pYUV++ ;
        u  = *pYUV++ ;
        y2 = *pYUV++ ;
        v  = *pYUV++ ;

        *pGRB++ = clip(y1 + redAdjust[v]);
        *pGRB++ = clip(y1 + greenAdjust1[u] + greenAdjust2[v]);
        *pGRB++ = clip(y1 + blueAdjust[u]);
        *pGRB++ = clip(y2 + redAdjust[v]);
        *pGRB++ = clip(y2 + greenAdjust1[u] + greenAdjust2[v]);
        *pGRB++ = clip(y2 + blueAdjust[u]);
    }
    return 0;
}
ZImgCapThread::ZImgCapThread(QString devNodeName,qint32 nPreWidth,qint32 nPreHeight,qint32 nPreFps)
{
    this->m_devName=devNodeName;
    this->m_nPreWidth=nPreWidth;
    this->m_nPreHeight=nPreHeight;
    this->m_nPreFps=nPreFps;

    this->m_pImgData=NULL;
    this->m_pImgTemp=NULL;
    this->m_bRunning=false;

    this->m_cam=new ZCAMDevice(this->m_devName,this->m_nPreWidth,this->m_nPreHeight,this->m_nPreFps);
    connect(this->m_cam,SIGNAL(ZSigMsg(QString,qint32)),this,SIGNAL(ZSigMsg(QString,qint32)),Qt::DirectConnection);
}
ZImgCapThread::~ZImgCapThread()
{
    disconnect(this->m_cam,SIGNAL(ZSigMsg(QString,qint32)),this,SIGNAL(ZSigMsg(QString,qint32)));
    delete this->m_cam;
    this->m_cam=NULL;
}
qint32 ZImgCapThread::ZStartThread()
{
    this->start();
    return 0;
}
qint32 ZImgCapThread::ZStopThread()
{
    this->exit(0);
    return 0;
}
qint32 ZImgCapThread::ZGetCAMImgWidth()
{
    return this->m_cam->ZGetImgWidth();
}
qint32 ZImgCapThread::ZGetCAMImgHeight()
{
    return this->m_cam->ZGetImgHeight();
}
qint32 ZImgCapThread::ZGetCAMImgFps()
{
    return this->m_cam->ZGetFrameRate();
}
QString ZImgCapThread::ZGetDevName()
{
    return this->m_devName;
}
bool ZImgCapThread::ZIsRunning()
{
    return this->m_bRunning;
}
QString ZImgCapThread::ZGetCAMID()
{
    return this->m_cam->ZGetCAMID();
}
void ZImgCapThread::ZSlotCheckGblStartFlag()
{
    if(gGblPara.m_bGblStartFlag)
    {
        if(!this->m_timerCap->isActive())
        {
            qDebug()<<"detected start flag,start cap thread:"<<this->m_devName;
            this->m_timerCap->start(10);
        }
    }else{
        if(this->m_timerCap->isActive())
        {
            qDebug()<<"detected stop flag,stop cap thread:"<<this->m_devName;
            this->m_timerCap->stop();
        }
    }
}
void ZImgCapThread::ZSlotDoCapture()
{
    //if the global request exit flag was set.
    //when we exit event-loop.
    if(gGblPara.m_bGblRst2Exit)
    {
        this->exit(0);
        return;
    }

    qint32 nLen;
    qint64 nStartTs,nEndTs;
    if(gGblPara.m_bCaptureLog)
    {
        nStartTs=QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug()<<this->m_devName<<" start at "<<nStartTs;
    }
    if(this->m_cam->ZGetFrame((void**)&this->m_pImgTemp,(size_t*)&nLen)<0)
    {
        emit this->ZSigMsg(tr("failed to get frame,len error:%1").arg(nLen),Log_Msg_Error);
        //here we set global request exit flag to help other thread to exit.
        gGblPara.m_bGblRst2Exit=true;
        return;
    }
    //memset(this->m_pImgData,0,nSingleImgSize);
    //this->convYUV2RGBBuffer(this->m_pImgTemp,this->m_pImgData,this->m_cam->ZGetImgWidth(),this->m_cam->ZGetImgHeight());

    YUYVToRGB_table(this->m_pImgTemp,this->m_pImgData,this->m_cam->ZGetImgWidth(),this->m_cam->ZGetImgHeight());
    //qint32 nSingleImgSize=this->m_cam->ZGetImgWidth()*this->m_cam->ZGetImgHeight()*3*sizeof(char);
    //this->m_QImg->loadFromData((uchar*)this->m_pImgData,nSingleImgSize);

    //build a RGB888 QImage object.
    QImage newImg((uchar*)this->m_pImgData,this->m_cam->ZGetImgWidth(),this->m_cam->ZGetImgHeight(),QImage::Format_RGB888);
    emit this->ZSigCapImg(newImg);
    this->m_cam->ZUnGetFrame();

    if(gGblPara.m_bCaptureLog)
    {
        nEndTs=QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug()<<this->m_devName<<" end at "<<nEndTs<<",cost "<<nEndTs-nStartTs<<"ms";
    }
}
void ZImgCapThread::run()
{
    do{
        if(this->m_cam->ZOpenCAM()<0)
        {
            emit this->ZSigMsg(tr("failed to open video device %1").arg(this->m_devName),Log_Msg_Error);
            break;
        }
        if(this->m_cam->ZInitCAM()<0)
        {
            emit this->ZSigMsg(tr("failed to init device %1").arg(this->m_devName),Log_Msg_Error);
            break;
        }
        if(this->m_cam->ZInitMAP()<0)
        {
            emit this->ZSigMsg(tr("failed to init map device:%1.").arg(this->m_devName),Log_Msg_Error);
            break;
        }
        //if user only wants to dump camera info to file.
        //then we donot start capture.
        if(gGblPara.m_bDumpCamInfo2File)
        {
            //we only dump camera info to file and then quit.
            break;
        }else{
            //malloc memory.
            qint32 nSingleImgSize=this->m_cam->ZGetImgWidth()*this->m_cam->ZGetImgHeight()*3*sizeof(char);
            if(NULL==(this->m_pImgData=(unsigned char*)malloc(nSingleImgSize)))
            {
                emit this->ZSigMsg(tr("failed to malloc memory!"),Log_Msg_Error);
                break;
            }
            //start capture.
            if(this->m_cam->ZStartCapture()<0)
            {
                emit this->ZSigMsg(tr("failed to start capture!"),Log_Msg_Error);
                break;
            }

            //start schedule timer.
            this->m_timerCap=new QTimer;
            QObject::connect(this->m_timerCap,SIGNAL(timeout()),this,SLOT(ZSlotDoCapture()),Qt::DirectConnection);
            //do not start at default.
            //wait for uart command :start/stop.
            //this->m_timerCap->start(10);
            this->m_timerCtl=new QTimer;
            QObject::connect(this->m_timerCtl,SIGNAL(timeout()),this,SLOT(ZSlotCheckGblStartFlag()),Qt::DirectConnection);
            this->m_timerCtl->start(1000);//1s.

            //set flag.
            this->m_bRunning=true;
            //enter event-loop until exit() was called.
            this->exec();

            this->m_timerCtl->stop();
            delete this->m_timerCtl;

            this->m_timerCap->stop();
            delete this->m_timerCap;
        }
    }while(0);

    //here we set global request exit flag to help other thread to exit.
    gGblPara.m_bGblRst2Exit=true;

    //stop camera.
    this->m_cam->ZStopCapture();
    this->m_cam->ZUnInitCAM();
    this->m_cam->ZCloseCAM();
    //free memory.
    if(this->m_pImgData)
    {
        free(this->m_pImgData);
        this->m_pImgData=NULL;
    }
    qDebug()<<"thread "<<this->m_devName<<" exit okay.";
    //set flags.
    this->m_bRunning=false;
    emit this->ZSigThreadFinished();
}
qint32 ZImgCapThread::convYUV2RGBPixel(qint32 y,qint32 u,qint32 v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}
qint32 ZImgCapThread::convYUV2RGBBuffer(unsigned char *yuv,unsigned char *rgb,unsigned int width,unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 =yuv[in + 3] << 24 |yuv[in + 2] << 16 |yuv[in + 1] <<  8 |yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = this->convYUV2RGBPixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = this->convYUV2RGBPixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}
