# Image Process Application
>>we use queue to connect different threads.  
## Image Capture Thread
There are two video device node in /dev directoy.  
/dev/video0,/dev/video1.  
We use V4L2 framework to grab yuyv(yuv422) data from camera.  
Main camera data flow:  
1.capture thread->yuv queue->h264 encode thread.  
2.capture thread->QImage queue->local display window.  
3.capture thread->QImage queue->image process thread.  

Aux camera data flow:  
1.capture thread->QImage queue->local display window.  
2.capture thread->QImage queue->image process thread.  

## Image Process Thread

## H264 Encode Thread
## TCP Transmit Thread