#include "zserverthread.h"
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include "zgblpara.h"
ZServerThread::ZServerThread(ZImgProcessThread *processThread)
{
    this->m_processThread=processThread;
    this->m_bRunning=false;
}
qint32 ZServerThread::ZStartThread()
{
    this->start();
    return 0;
}
qint32 ZServerThread::ZStopThread()
{
    this->exit(0);
    return 0;
}
bool ZServerThread::ZIsRunning()
{
    return this->m_bRunning;
}
void ZServerThread::run()
{
    do{
        // Automatically start up and clean up UDT module.
        UDT::startup();

        addrinfo hints;
        addrinfo* res;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_flags=AI_PASSIVE;
        hints.ai_family=AF_INET;
#ifdef USE_TCP
        hints.ai_socktype = SOCK_STREAM;
#else
        hints.ai_socktype = SOCK_DGRAM;
#endif

        std::string service("9000");
        if (0!=getaddrinfo(NULL, service.c_str(), &hints, &res))
        {
            qDebug()<<"illegal port number or port is busy.";
            break;
        }

        this->m_serv=UDT::socket(res->ai_family,res->ai_socktype,res->ai_protocol);

        // UDT Options
        //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
        //UDT::setsockopt(serv, 0, UDT_MSS, new int(9000), sizeof(int));
        //UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(10000000), sizeof(int));
        //UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(10000000), sizeof(int));

        if(UDT::ERROR==UDT::bind(this->m_serv,res->ai_addr,res->ai_addrlen))
        {
            qDebug()<<"<error>:"<<UDT::getlasterror().getErrorMessage();
            break;
        }

        freeaddrinfo(res);

        qDebug()<<"Server is ready at port:"<<service.c_str();
        if (UDT::ERROR==UDT::listen(this->m_serv,10))
        {
            qDebug()<<"error:listen(),"<<UDT::getlasterror().getErrorMessage();
            break;
        }

        //start timer to accpet connection.
        this->m_timer=new QTimer;
        QObject::connect(this->m_timer,SIGNAL(timeout()),this,SLOT(ZSlotTimeoutSchedule()),Qt::DirectConnection);
        this->m_timer->start(1000);

        //set flag.
        this->m_bRunning=true;
        //enter event-loop until exit() was called.
        this->exec();

        this->m_timer->stop();
        delete this->m_timer;
    }while(0);

    //here we set global request exit flag to help other thread to exit.
    gGblPara.m_bGblRst2Exit=true;

    UDT::close(this->m_serv);
    UDT::cleanup();

    qDebug()<<"thread server exit okay.";
    //set flag.
    this->m_bRunning=false;
    emit this->ZSigThreadFinished();
}
void ZServerThread::ZSlotTimeoutSchedule()
{
    //if the global request exit flag was set.
    //when we exit event-loop.
    if(gGblPara.m_bGblRst2Exit)
    {
        this->exit(0);
        return;
    }
    ud_set fdSet;
    UD_ZERO(&fdSet);
    UD_SET(this->m_serv,&fdSet);
    struct timeval tv;
    tv.tv_sec=1;//1s.
    tv.tv_usec=0;
    qint32 nRet=UDT::select(this->m_serv+1,&fdSet,NULL,NULL,&tv);
    if(nRet<0)
    {
        //something error occured.
        //here we set global request exit flag to help other thread to exit.
        gGblPara.m_bGblRst2Exit=true;
        return;
    }else if(nRet==0)
    {
        //timeout.do select() again.
        return;
    }else{
        if(UD_ISSET(this->m_serv,&fdSet))
        {
            sockaddr_storage clientaddr;
            int addrlen = sizeof(clientaddr);
            UDTSOCKET sockClient;
            //accept.
            if (UDT::INVALID_SOCK==(sockClient=UDT::accept(this->m_serv,(sockaddr*)&clientaddr,&addrlen)))
            {
                qDebug()<<"error:accept(),"<<UDT::getlasterror().getErrorMessage();
                return;
            }

            char clienthost[NI_MAXHOST];
            char clientservice[NI_MAXSERV];
            getnameinfo((sockaddr *)&clientaddr,addrlen,clienthost,sizeof(clienthost),clientservice,sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
            qDebug()<<"new connection:"<<clienthost<<":"<<clientservice;
            //CreateThread(NULL, 0, recvdata, new UDTSOCKET(sockClient), 0, NULL);
            //when client connects send captured images to it.
            ZServiceThread *newPC=new ZServiceThread(sockClient);
            QObject::connect(this->m_processThread,SIGNAL(ZSigImgProcessed(QImage,QImage)),newPC,SLOT(ZSlotTxImg2PC(QImage,QImage)),Qt::DirectConnection);
            QObject::connect(this->m_processThread,SIGNAL(ZSigDiffXYT(QRect,QRect,qint32,qint32,qint32)),newPC,SLOT(ZSlotDiffXYT(QRect,QRect,qint32,qint32,qint32)),Qt::DirectConnection);
            QObject::connect(newPC,SIGNAL(finished()),newPC,SLOT(deleteLater()));
            newPC->ZStartThread();
        }
    }
}
