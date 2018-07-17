#include "zimgdisplayer.h"
#include <QPainter>
#include <QDebug>
ZImgDisplayer::ZImgDisplayer(qint32 nCenterX,qint32 nCenterY,bool bMainCamera,QWidget *parent) : QWidget(parent)
{
    this->m_nCenterX=nCenterX;
    this->m_nCenterY=nCenterY;
    this->m_nTriggerCounter=0;
    this->m_colorRect=QColor(0,255,0);
    this->m_bMainCamera=bMainCamera;

    //we supply the motor move buttons for SlaveCamera.
    if(!this->m_bMainCamera)
    {
        for(qint32 i=0;i<4;i++)
        {
            this->m_tbMotorCtl[i]=new QToolButton(this);
            this->m_tbMotorCtl[i]->setIconSize(QSize(48,48));
            this->m_tbMotorCtl[i]->setStyleSheet("QToolButton{background:transparent;border-style:flat;}");
            switch(i)
            {
            case 0:
                this->m_tbMotorCtl[i]->setIcon(QIcon(":/motor/images/motor/up.png"));
                break;
            case 1:
                this->m_tbMotorCtl[i]->setIcon(QIcon(":/motor/images/motor/down.png"));
                break;
            case 2:
                this->m_tbMotorCtl[i]->setIcon(QIcon(":/motor/images/motor/left.png"));
                break;
            case 3:
                this->m_tbMotorCtl[i]->setIcon(QIcon(":/motor/images/motor/right.png"));
                break;
            }
        }
    }
}
void ZImgDisplayer::ZSetSensitiveRect(QRect rect)
{
    this->m_rectSensitive=rect;
    if(!this->m_bMainCamera)
    {
        if(rect==this->m_rectSensitiveOld)
        {
            this->m_bStretchFlag=true;
        }else{
            this->m_bStretchFlag=false;
            this->m_rectSensitiveOld=rect;
        }
    }else{
        this->m_rectSensitiveOld=rect;
    }
}
void ZImgDisplayer::ZSetCAMParameters(qint32 nWidth,qint32 nHeight,qint32 nFps,QString camID)
{
    this->m_nCamWidth=nWidth;
    this->m_nCamHeight=nHeight;
    this->m_nCAMFps=nFps;
    this->m_camID=camID;
}
void ZImgDisplayer::ZSetPaintParameters(QColor colorRect)
{
    this->m_colorRect=colorRect;
}
//QSize ZImgDisplayer::sizeHint() const
//{
//    return QSize(IMG_SCALED_W,IMG_SCALED_H);
//}
void ZImgDisplayer::resizeEvent(QResizeEvent *event)
{
    if(!this->m_bMainCamera)
    {
        //calculate the center point.
        //    X
        //  X  X
        //   X
        QPoint ptCenter;
        ptCenter.setX(this->width()-48*3);
        ptCenter.setY((this->height()-48*3)/2);
        //because our icons are 48x48.
        this->m_tbMotorCtl[0]->move(ptCenter.x(),ptCenter.y()-48);//up.
        this->m_tbMotorCtl[1]->move(ptCenter.x(),ptCenter.y()+48);//down.
        this->m_tbMotorCtl[2]->move(ptCenter.x()-48,ptCenter.y());//left.
        this->m_tbMotorCtl[3]->move(ptCenter.x()+48,ptCenter.y());//right.
    }
    QWidget::resizeEvent(event);
}
void ZImgDisplayer::ZSlotDispImg(const QImage &img)
{
    this->m_img=img;
    this->m_nTriggerCounter++;
    this->update();
}
void ZImgDisplayer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    //the cross + size,width*height.
    qint32 nCrossW=20;
    qint32 nCrossH=20;
    //the skip pixels,empty black.
    qint32 nSkipPixels=6;

    QPainter painter(this);
    if(this->m_img.isNull())
    {
        painter.fillRect(QRectF(0,0,this->width(),this->height()),Qt::black);
        QPen pen(Qt::green,4);
        painter.setPen(pen);
        painter.drawLine(QPointF(0,0),QPointF(this->width(),this->height()));
        painter.drawLine(QPointF(this->width(),0),QPointF(0,this->height()));
        return;
    }
    QFont tFont=painter.font();
    tFont.setPointSize(16);
    painter.setFont(tFont);

    //draw the image.
    QRectF rectIMG(0,0,this->width(),this->height());
    painter.drawImage(rectIMG,this->m_img);

    painter.setPen(QPen(Qt::green,3,Qt::SolidLine));

    //draw the camera resolution & fps.
    QString camInfo;
    camInfo.append(tr("%1*%2\n").arg(this->m_nCamWidth).arg(this->m_nCamHeight));
    camInfo.append(tr("%1fps\n").arg(this->m_nCAMFps));
    camInfo.append(QString::number(this->m_nTriggerCounter,10));
    //here plus 10 to avoid last text missing.
    QRect rectCAMInfo(0,0,painter.fontMetrics().width(camInfo)+10,painter.fontMetrics().height()*3);//3 lines.
    painter.drawText(rectCAMInfo,camInfo);

    //draw the calibrate center (x,y).

    //calculate the scale ratio.
    float fRatioWidth,fRatioHeight;
    if(this->width()>this->m_nCamWidth)
    {
        fRatioWidth=this->width()/(this->m_nCamWidth*1.0);
    }else if(this->width()<this->m_nCamWidth)
    {
        fRatioWidth=this->m_nCamWidth/(this->width()*1.0);
    }else{
        fRatioWidth=1.0f;
    }
    if(this->height()>this->m_nCamHeight)
    {
        fRatioHeight=this->height()/(this->m_nCamHeight*1.0);
    }else if(this->height()<this->m_nCamHeight){
        fRatioHeight=this->m_nCamHeight/(this->height()*1.0);
    }else{
        fRatioHeight=1.0f;
    }

    //draw the top part.
    QPointF ptTop;
    if(this->m_nCenterY*fRatioHeight-nCrossH<0)
    {
        ptTop=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight);
    }else{
        ptTop=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight-nCrossH);
        QPointF ptTop2(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight-nSkipPixels);
        painter.drawLine(ptTop,ptTop2);
    }
    //draw the bottom part.
    QPointF ptBottom;
    if(this->m_nCenterY*fRatioHeight+nCrossH>this->height())
    {
        ptBottom=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight);
    }else{
        ptBottom=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight+nCrossH);
        QPointF ptBottom2(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight+nSkipPixels);
        painter.drawLine(ptBottom2,ptBottom);
    }
    //draw the left part.
    QPointF ptLeft;
    if(this->m_nCenterX*fRatioWidth-nCrossW<0)
    {
        ptLeft=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight);
    }else{
        ptLeft=QPointF(this->m_nCenterX*fRatioWidth-nCrossW,this->m_nCenterY*fRatioHeight);
        QPointF ptLeft2(this->m_nCenterX*fRatioWidth-nSkipPixels,this->m_nCenterY*fRatioHeight);
        painter.drawLine(ptLeft,ptLeft2);
    }
    //draw the right part.
    QPointF ptRight;
    if(this->m_nCenterX*fRatioWidth+nCrossW>this->width())
    {
        ptRight=QPointF(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight);
    }else{
        ptRight=QPointF(this->m_nCenterX*fRatioWidth+nCrossW,this->m_nCenterY*fRatioHeight);
        QPointF ptRight2(this->m_nCenterX*fRatioWidth+nSkipPixels,this->m_nCenterY*fRatioHeight);
        painter.drawLine(ptRight2,ptRight);
    }
    //////////////////////////////////////////////////////////////////////////////////
    //draw the camID.
    qint32 nWidthCAMID=painter.fontMetrics().width(this->m_camID);
    qint32 nHeightCAMID=painter.fontMetrics().height();
    //here plus 10 to avoid last text missing.
    QRect rectCAMID(this->width()-nWidthCAMID,this->height()-nHeightCAMID,nWidthCAMID+10,nHeightCAMID);
    painter.drawText(rectCAMID,this->m_camID);

    //////////////////////////////////////////////////////////////////////////////////
    //if i am the main camera.
    if(this->m_bMainCamera)
    {
        qint32 nFontHeight=painter.fontMetrics().height();

        QString mainTips("Main");
        qint32 nMainTipsWidth=painter.fontMetrics().width(mainTips);
        //here plus 10 to avoid the last text missing.
        QRect rectMainTips(this->width()-nMainTipsWidth,0,nMainTipsWidth+10,nFontHeight);
        painter.drawText(rectMainTips,mainTips);

        //draw the calibration (x,y) text.
        QString calibrateXY;
        calibrateXY.append("Calibrated:(");
        calibrateXY.append(QString::number(this->m_nCenterX,10));
        calibrateXY.append(",");
        calibrateXY.append(QString::number(this->m_nCenterY,10));
        calibrateXY.append(")");

        qint32 nCalibrateWidth=painter.fontMetrics().width(calibrateXY);
        //QString str("Calibrate:(320,240)");
        //qDebug()<<calibrateXY<<"="<<nCalibrateWidth<<","<<painter.fontMetrics().width(str);
        //here plus 10 to avoid the last text missing.
        QRect rectCalibrateXY(0,this->height()-nFontHeight,nCalibrateWidth+10,nFontHeight);
        painter.drawText(rectCalibrateXY,calibrateXY);

        //draw the sensitive rectangle.
        QPen penRect(this->m_colorRect,4,Qt::DashLine);
        painter.setPen(penRect);
        QRect rectSensitiveRatio;
        rectSensitiveRatio.setX(this->m_rectSensitiveOld.x()*fRatioWidth);
        rectSensitiveRatio.setY(this->m_rectSensitiveOld.y()*fRatioHeight);
        rectSensitiveRatio.setWidth(this->m_rectSensitiveOld.width()*fRatioWidth);
        rectSensitiveRatio.setHeight(this->m_rectSensitiveOld.height()*fRatioHeight);
        painter.drawRect(rectSensitiveRatio);

    }else{
        //i am the aux camera.
        qint32 nFontHeight=painter.fontMetrics().height();

        QString auxTips("Aux");
        qint32 nAuxTipsWidth=painter.fontMetrics().width(auxTips);
        //here plus 10 to avoid the last text missing.
        QRect rectAuxTips(this->width()-nAuxTipsWidth,0,nAuxTipsWidth+10,nFontHeight);
        painter.drawText(rectAuxTips,auxTips);

        //draw the calibration (x,y) text.
        QString calibrateXY("Calibrated:(");
        calibrateXY.append(QString::number(this->m_nCenterX,10));
        calibrateXY.append(",");
        calibrateXY.append(QString::number(this->m_nCenterY,10));
        calibrateXY.append(")->(");
        calibrateXY.append(QString::number(this->m_rectSensitive.x(),10));
        calibrateXY.append(",");
        calibrateXY.append(QString::number(this->m_rectSensitive.y(),10));
        calibrateXY.append(")");
        qint32 nCalibrateWidth=painter.fontMetrics().width(calibrateXY);
        //here plus 10 to avoid the last text missing.
        QRect rectCalibrateXY(0,this->height()-nFontHeight,nCalibrateWidth+10,nFontHeight);
        painter.drawText(rectCalibrateXY,calibrateXY);

        //stretch rectangle or not?
        if(this->m_bStretchFlag)
        {
            qint32 newX=this->m_rectSensitiveOld.x()+this->m_nRatio*10;
            qint32 newY=this->m_rectSensitiveOld.y()+this->m_nRatio*10;
            qint32 newWidth=this->m_rectSensitiveOld.width()-this->m_nRatio*10*2;
            qint32 newHeight=this->m_rectSensitiveOld.height()-this->m_nRatio*10*2;
            if(newWidth>10 && newHeight>10)
            {
                this->m_rectSensitiveOld=QRect(newX,newY,newWidth,newHeight);
                this->m_nRatio++;
            }else{
                this->m_rectSensitiveOld=this->m_rectSensitive;
                this->m_nRatio=1;
            }
        }

        //draw the sensitive rectangle.
        QPen penRect(this->m_colorRect,4,Qt::DashLine);
        painter.setPen(penRect);
        QRect rectSensitiveRatio;
        rectSensitiveRatio.setX(this->m_rectSensitiveOld.x()*fRatioWidth);
        rectSensitiveRatio.setY(this->m_rectSensitiveOld.y()*fRatioHeight);
        rectSensitiveRatio.setWidth(this->m_rectSensitiveOld.width()*fRatioWidth);
        rectSensitiveRatio.setHeight(this->m_rectSensitiveOld.height()*fRatioHeight);
        painter.drawRect(rectSensitiveRatio);

        //draw the center of sensitive rectangle.
        qint32 nSenseCenterX=rectSensitiveRatio.x()+rectSensitiveRatio.width()/2;
        qint32 nSenseCenterY=rectSensitiveRatio.y()+rectSensitiveRatio.height()/2;
        //draw the top part.
        QPoint ptSenseTop;
        if(nSenseCenterX-nCrossH<0)
        {
            ptSenseTop=QPoint(nSenseCenterX,nSenseCenterY);
        }else{
            ptSenseTop=QPoint(nSenseCenterX,nSenseCenterY-nCrossH);
            QPoint ptSenseTop2(nSenseCenterX,nSenseCenterY-nSkipPixels);
            painter.drawLine(ptSenseTop,ptSenseTop2);
        }
        //draw the bottom part.
        QPoint ptSenseBottom;
        if(nSenseCenterY+nCrossH>this->height())
        {
            ptSenseBottom=QPoint(nSenseCenterX,nSenseCenterY);
        }else{
            ptSenseBottom=QPoint(nSenseCenterX,nSenseCenterY+nCrossH);
            QPoint ptSenseBottom2(nSenseCenterX,nSenseCenterY+nSkipPixels);
            painter.drawLine(ptSenseBottom,ptSenseBottom2);
        }
        //draw the left part.
        QPoint ptSenseLeft;
        if(nSenseCenterX-nCrossW<0)
        {
            ptSenseLeft=QPoint(nSenseCenterX,nSenseCenterY);
        }else{
            ptSenseLeft=QPoint(nSenseCenterX-nCrossW,nSenseCenterY);
            QPoint ptSenseLeft2(nSenseCenterX-nSkipPixels,nSenseCenterY);
            painter.drawLine(ptSenseLeft,ptSenseLeft2);
        }
        //draw the right part.
        QPoint ptSenseRight;
        if(nSenseCenterX+nCrossW>this->width())
        {
            ptSenseRight=QPoint(nSenseCenterX,nSenseCenterY);
        }else{
            ptSenseRight=QPoint(nSenseCenterX+nCrossW,nSenseCenterY);
            QPoint ptSenseRight2(nSenseCenterX+nSkipPixels,nSenseCenterY);
            painter.drawLine(ptSenseRight,ptSenseRight2);
        }

        //draw a line from Calibrate center(x,y) to sensitive rectangle center(x,y).
        QPoint ptCalibrateXY(this->m_nCenterX*fRatioWidth,this->m_nCenterY*fRatioHeight);
        QPoint ptSensRectXY(rectSensitiveRatio.x()+rectSensitiveRatio.width()/2,rectSensitiveRatio.y()+rectSensitiveRatio.height()/2);
        if(ptCalibrateXY.x()>=0 && ptCalibrateXY.x()<=this->width() && ptCalibrateXY.y()>=0 && ptCalibrateXY.y()<=this->height())
        {
            if(ptSensRectXY.x()>=0 && ptSensRectXY.x()<=this->width() && ptSensRectXY.y()>=0 && ptSensRectXY.y()<=this->height())
            {
                QPen penLine(Qt::yellow,3,Qt::SolidLine);
                painter.setPen(penLine);
                painter.drawLine(ptCalibrateXY,ptSensRectXY);
            }
        }

    }
    //////////////////////////////////////////////////////////////////////////////////
}
