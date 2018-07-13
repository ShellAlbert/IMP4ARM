#include "zh264encthread.h"
#include <zgblpara.h>
#include <QDebug>
extern "C"
{
#include <x264.h>
#include <x264_config.h>
}
ZH264EncThread::ZH264EncThread()
{
    this->m_bExitFlag=false;

    this->m_queueYUV=NULL;
    this->m_semaYUVUsed=NULL;
    this->m_semaYUVFree=NULL;

    this->m_queueH264=NULL;
    this->m_semaH264Used=NULL;
    this->m_semaH264Free=NULL;
}
qint32 ZH264EncThread::ZBindYUVQueue(QQueue<QByteArray> *queueYUV,QSemaphore *semaYUVUsed,QSemaphore *semaYUVFree)
{
    this->m_queueYUV=queueYUV;
    this->m_semaYUVUsed=semaYUVUsed;
    this->m_semaYUVFree=semaYUVFree;
    return 0;
}
qint32 ZH264EncThread::ZBindH264Queue(QQueue<QByteArray> *queueH264,QSemaphore *semaH264Used,QSemaphore *semaH264Free)
{
    this->m_queueH264=queueH264;
    this->m_semaH264Used=semaH264Used;
    this->m_semaH264Free=semaH264Free;
    return 0;
}
qint32 ZH264EncThread::ZStartThread()
{
    //check yuv queue.
    if(this->m_queueYUV==NULL || this->m_semaYUVUsed==NULL || this->m_semaYUVFree==NULL)
    {
        qDebug()<<"<error>:no bind yuv queue,cannot start.";
        return -1;
    }
    //check h264 queue.
    if(this->m_queueH264==NULL || this->m_semaH264Used==NULL || this->m_semaH264Free==NULL)
    {
        qDebug()<<"<error>:no bind h264 queue,cannot start.";
        return -1;
    }

    this->start();
    return 0;
}
qint32 ZH264EncThread::ZStopThread()
{
    this->m_bExitFlag=true;
    return 0;
}
void ZH264EncThread::run()
{
    x264_t *pHandle=NULL;
    x264_nal_t *pNals=NULL;
    int iNal=0;

    x264_param_t *pParam=(x264_param_t*)malloc(sizeof(x264_param_t));
    x264_picture_t *pPicIn=(x264_picture_t*)malloc(sizeof(x264_picture_t));
    //x264_picture_t *pPicOut=(x264_picture_t*)malloc(sizeof(x264_picture_t));
    //set default parameters.
    x264_param_default(pParam);
    //set width*height.
    pParam->i_width=gGblPara.m_widthCAM1;
    pParam->i_height=gGblPara.m_heightCAM1;
    pParam->i_threads=X264_SYNC_LOOKAHEAD_AUTO;
    pParam->i_csp=X264_CSP_I422;
    pParam->rc.i_lookahead=0;
    pParam->i_fps_num=30;
    pParam->i_fps_den=1;
    //static const char * const x264_profile_names[] = { "baseline", "main", "high", "high10", "high422", "high444", 0 };
    x264_param_apply_profile(pParam,x264_profile_names[4]);
    pHandle=x264_encoder_open(pParam);

    x264_picture_alloc(pPicIn,X264_CSP_I422,pParam->i_width,pParam->i_height);

    qDebug()<<"<MainLoop>:H264EncThread starts.";
    long int pts=0;
    char *pEncodeBuffer=new char[1*1024*1024];
    while(!gGblPara.m_bGblRst2Exit)
    {
        x264_picture_t picOut;
        int nEncodeBytes=0;

        //1.fetch data from yuv queue.
        QByteArray baImgData;
        this->m_semaYUVUsed->acquire();//已用信号量减1.
        baImgData=this->m_queueYUV->dequeue();
        this->m_semaYUVFree->release();//空闲信号量加1.
        qDebug()<<"<h264 encode thread> fetch img from yuv queue,size:"<<baImgData.size();
        //2.do h264 encode.
        char *pYUV=baImgData.data();

        int yIndex=0;
        int uIndex=0;
        int vIndex=0;

        char *y=(char*)pPicIn->img.plane[0];
        char *u=(char*)pPicIn->img.plane[1];
        char *v=(char*)pPicIn->img.plane[2];

        //将交错存储的YUV数据分开.
        qDebug()<<gGblPara.m_widthCAM1<<","<<gGblPara.m_heightCAM1;
        int num=gGblPara.m_widthCAM1*gGblPara.m_heightCAM1*2-4;
        for(int i=0;i<num;i=i+4)
        {
            *(y+(yIndex++))=*(pYUV+i);
            *(u+(uIndex++))=*(pYUV+i+1);
            *(y+(yIndex++))=*(pYUV+i+2);
            *(v+(vIndex++))=*(pYUV+i+3);
        }
        //pPicIn->i_type=X264_TYPE_P;//
        //pPicIn->i_type=X264_TYPE_IDR;
        pPicIn->i_type=X264_TYPE_I;
        //pPicIn->i_type=X264_TYPE_B;
        //pPicIn->i_type=X264_TYPE_AUTO;


        //i_pts参数需要递增，否则会出现警告:x264[warning]:non-strictly-monotonic PTS.
        pPicIn->i_pts=pts++;
        //进行编码.
        qint32 ret=x264_encoder_encode(pHandle,&pNals,&iNal,pPicIn,&picOut);
        if(ret<0)
        {
            qDebug()<<"<error>:x264 encode failed.";
        }else{
            for(int j=0;j<iNal;j++)
            {
                /* Size of payload (including any padding) in bytes. */
                //int     i_payload;
                /* If param->b_annexb is set, Annex-B bytestream with startcode.
                 * Otherwise, startcode is replaced with a 4-byte size.
                 * This size is the size used in mp4/similar muxing; it is equal to i_payload-4 */
                //uint8_t *p_payload;
                //fwrite(pNals[j].p_payload,1,pNals[j].i_payload,fp);
                //qDebug()<<"encode size:"<<pNals[j].i_payload;
                memcpy(pEncodeBuffer+nEncodeBytes,pNals[j].p_payload,pNals[j].i_payload);
                nEncodeBytes+=pNals[j].i_payload;
            }
            //flush encode.
#if 0
            while(1)
            {
                if(0==x264_encoder_encode(pHandle,&pNals,&iNal,NULL,&picOut))
                {
                    break;
                }
                for(int j=0;j<iNal;j++)
                {
                    memcpy(pEncodeBuffer+nEncodeBytes,pNals[j].p_payload,pNals[j].i_payload);
                    nEncodeBytes+=pNals[j].i_payload;
                }
            }
#endif
            qDebug()<<"encode bytes:"<<nEncodeBytes;
        }
#if 0
        //3.put encode pkt to h264 queue.
        QByteArray baEncodedData(baImgData);
        this->m_semaH264Free->acquire();//空闲信号量减1.
        this->m_queueH264->enqueue(baEncodedData);
        this->m_semaH264Used->release();//已用信号量加1.
#endif
    }
    x264_picture_clean(pPicIn);
    x264_encoder_close(pHandle);
    qDebug()<<"<MainLoop>:H264EncThread ends.";
    return;
}
