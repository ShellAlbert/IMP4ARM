# Image Process Application
>>we split functions to multiple threads.  
>>we use queue to connect different threads.  
## Image Capture Thread
There are two video device nodes in /dev directoy.  
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
We use openCV library to process two images.  
A small template image was cut from the main camera.it works as a template.  
matchTemplate was used to find a rectangle area in the aux camera image.  
To reduce cpu load and keep time low, i resize image to 1/2 of original.  

 
## H264 Encode Thread
input:CSP_I420   
I frame to make sure the decoder can decode frames without others frames help.   

## TCP Transmit Thread