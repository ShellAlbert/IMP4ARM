#include "zgblpara.h"

#include <QDebug>
#include <zmaintask.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "zhistogram.h"
#include <QDateTime>
#include <QSettings>
#ifdef BUILD_GUI
#include <QApplication>
#include <QFontDatabase>
#else
#include <QCoreApplication>
#endif
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <signal.h>
void gSIGHandler(int sigNo)
{
    switch(sigNo)
    {
    case SIGINT:
    case SIGKILL:
    case SIGTERM:
        qDebug()<<"prepare to exit...";
        gGblPara.m_bGblRst2Exit=true;
        break;
    default:
        break;
    }
}
int main(int argc,char **argv)
{
#ifdef BUILD_GUI
    QApplication app(argc,argv);
//    qint32 fontID=QFontDatabase::addApplicationFont(":/font/DejaVuSerif.ttf");
//    QString fontName=QFontDatabase::applicationFontFamilies(fontID).at(0);
//    QFont font(fontName);
//    app.setFont(font);
#else
    QCoreApplication app(argc,argv);
#endif
    //parse command line arguments.
    QCommandLineOption opDebug("d");//-d debug mode.
    QCommandLineOption opVerbose("v");//-v verbose mode.
    QCommandLineOption opDumpCamInfo("w");//-w write camera info to file.
    QCommandLineOption opCapture("c");//-c capture log.
    QCommandLineOption opTransfer("t");//-t transfer video to pc.
    QCommandLineOption opSpeed("s");//-s transfer speed monitor.
    QCommandLineOption opUART("y");//-y hex uart data.
    QCommandLineOption opXMode("x");//-x xMode,resize image to 1/2 before matchTemplate.
    QCommandLineOption opFMode("f");//-f fMode,use feature extractor.
    QCommandLineOption opHelp("h");//-h help.
    QCommandLineParser parser;
    parser.addOption(opDebug);
    parser.addOption(opVerbose);
    parser.addOption(opDumpCamInfo);
    parser.addOption(opCapture);
    parser.addOption(opTransfer);
    parser.addOption(opSpeed);
    parser.addOption(opUART);
    parser.addOption(opXMode);
    parser.addOption(opFMode);
    parser.addOption(opHelp);
    parser.process(app);

    if(parser.isSet(opHelp))
    {
        qDebug()<<APPName;
        qDebug()<<" -d :debug mode.";
        qDebug()<<" -v :show verbose message.";
        qDebug()<<" -w :dump camera data to /tmp/videoX file and quit.";
        qDebug()<<" -c :print capture log.";
        qDebug()<<" -t :transfer video to pc.";
        qDebug()<<" -s :transfer speed monitor.";
        qDebug()<<" -y :hex uart data.";
        qDebug()<<" -x :xMode,resize images 1/2 before match template.";
        qDebug()<<" -f :fMode,feature extractor.";
        qDebug()<<" -h :show help text.";
        qDebug()<<"Build on "<<QString(__DATE__)<<QString(__TIME__);
        return 0;
    }
    if(parser.isSet(opDebug))
    {
        gGblPara.m_bDebugMode=true;
    }
    if(parser.isSet(opVerbose))
    {
        gGblPara.m_bVerbose=true;
    }
    if(parser.isSet(opDumpCamInfo))
    {
        gGblPara.m_bDumpCamInfo2File=true;
    }
    if(parser.isSet(opCapture))
    {
        gGblPara.m_bCaptureLog=true;
    }
    if(parser.isSet(opTransfer))
    {
        gGblPara.m_bTransfer2PC=true;
        if(parser.isSet(opSpeed))
        {
            gGblPara.m_bTransferSpeedMonitor=true;
        }
    }
    if(parser.isSet(opUART))
    {
        gGblPara.m_bDumpUART=true;
    }
    if(parser.isSet(opXMode))
    {
        gGblPara.m_bXMode=true;
    }
    if(parser.isSet(opFMode))
    {
        gGblPara.m_bFMode=true;
    }

    //这里作一个优先级判断，若xMode和fMode同时启动，则只启动fMode
    if(gGblPara.m_bXMode && gGblPara.m_bFMode)
    {
        gGblPara.m_bXMode=false;
        gGblPara.m_bFMode=true;
    }

    //write pid to file.
    if(gGblPara.writePid2File()<0)
    {
        return -1;
    }

    //read config file.
    QFile fileIni("IMP4ARM.ini");
    if(!fileIni.exists())
    {
        qDebug()<<"<error>:IMP4ARM.ini config file missed!";
        qDebug()<<"<error>:a template file was generated!";
        qDebug()<<"<error>:please modify it by manual and run again!";
        gGblPara.resetCfgFile();
        return 0;
    }
    gGblPara.readCfgFile();

#if 0
    cv::Mat mat1 = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::Mat mat2 = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

    ZImgProcessThread pp;
    qint64 tStartMS,tEndMS;
    
    //这里直接做图像匹配(就是李老师算法的效果).
    tStartMS=QDateTime::currentDateTime().toMSecsSinceEpoch();
    cv::Mat result=pp.ZDoTemplateMatchDirectly(mat1,mat2,true);
    //emit this->ZSigImgTemplateMatchDirectly(cvMat2QImage(result));
    tEndMS=QDateTime::currentDateTime().toMSecsSinceEpoch();
    qDebug()<<"matchTemplate costs:"<<tEndMS-tStartMS<<"(ms)";

    return 0;
#endif


    ZMainTask task;
    if(task.ZDoInit()<0)
    {
        if(gGblPara.m_bVerbose)
        {
            qDebug()<<"<error>:app exit due to previous error!";
        }
        return -1;
    }
#ifdef BUILD_GUI
    //task.showFullScreen();
    task.showMaximized();
#endif

    //install signal handler.
    signal(SIGINT,gSIGHandler);

    return app.exec();
}
