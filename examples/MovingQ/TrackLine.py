#!/usr/bin/python

import cv
import math
import time
import sys
import numpy

cv.NamedWindow("camera", 1)
cv.MoveWindow ('Camera', 10, 10)

sys.path.append('../../Bindings/Python/')

# load the Famouso Python Bindings
from pyFAMOUSO.famouso import *

# try to get the device number from the command line
if len(sys.argv) < 2:
  device= 0
else:
  device = int (sys.argv [1])
  
capture = cv.CaptureFromCAM(device)
frame = cv.QueryFrame(capture)

frame_size=cv.GetSize (frame)
gray_frame = cv.CreateImage( frame_size, cv.IPL_DEPTH_8U, 1 )
canny_result = cv.CreateImage( frame_size, cv.IPL_DEPTH_8U, 1 );
    
canny_init=300
canny_link=150
    
frame_on=False
canny_on=False 
init_on=True
border_on=True
points_on=False
    
forrun=frame_size[1]/2
corridor=20
old_target=[frame_size[0]/2,forrun]
epsilon=50
angle=0

font=cv.InitFont(cv.CV_FONT_VECTOR0,0.5,0.5,0,1); 
font_dist=20

subject = "WayPoint"
pub = publisher.PublisherEventChannel(subject)
pub.announce(subject)

while True:
  
    color_dst = cv.CreateImage( frame_size, cv.IPL_DEPTH_8U, 3 )
    
    ## 1. capture the current image
    frame = cv.QueryFrame(capture)
    if frame is None:
      break
      
    ## 2. copy current frame to a gray scale frame -> dst
    cv.CvtColor( frame, gray_frame, cv.CV_BGR2GRAY );

    ## 3. line detection in dst 
    ##        canny_init - initial edge detction
    ##        canny_link - responsible for edge links
    cv.Canny(gray_frame, canny_result, canny_init, canny_link, 3);
    ## Look for all points detected
    if init_on==True:
      points = numpy.nonzero(canny_result[(forrun-corridor/2):(forrun+corridor/2),:])
    else:
      left = old_target[0]-epsilon
      if left<0:
	left=0
      right = old_target[0]+epsilon
      if right>=frame_size[0]:
	right=frame_size[0]-1
      print left, right
      points= numpy.nonzero(canny_result[(forrun-corridor/2):(forrun+corridor/2)][:,left:right])
      print points
      if points:
	for i in range(0,len(points[1])):
	  points[1][i] = points[1][i] + left

    cv.CvtColor(canny_result, color_dst, cv.CV_GRAY2BGR );

    if frame_on==True:
      frame = color_dst
    else:
      cv.CvtColor( gray_frame, frame, cv.CV_GRAY2BGR );

    if border_on==True:
      start=(0,frame_size[1]/2)
      end=(frame_size[0],frame_size[1]/2)	    
      cv.Line( frame, start, end, cv.CV_RGB(0,255,0), 3, 8 )   
      start=(0,frame_size[1]/2+corridor/2)
      end=(frame_size[0],frame_size[1]/2+corridor/2)	    
      cv.Line( frame, start, end, cv.CV_RGB(0,255,0), 1, 8 ) 
      start=(0,frame_size[1]/2-corridor/2)
      end=(frame_size[0],frame_size[1]/2-corridor/2)	    
      cv.Line( frame, start, end, cv.CV_RGB(0,255,0), 1, 8 ) 
	  
      start=(old_target[0]-epsilon,frame_size[1]/2-corridor/2)
      end=(old_target[0]-epsilon,frame_size[1]/2+corridor/2)	    
      cv.Line( frame, start, end, cv.CV_RGB(0,255,0), 1, 8 ) 

      start=(old_target[0]+epsilon,frame_size[1]/2-corridor/2)
      end=(old_target[0]+epsilon,frame_size[1]/2+corridor/2)	    
      cv.Line( frame, start, end, cv.CV_RGB(0,255,0), 1, 8 ) 

    if  len(points[1])>0:
      if points_on==True:
      	for i in range(0,len(points[1])):
	   cv.Circle( frame, (points[1][i], points[0][i]+(forrun-corridor/2)), 5, cv.CV_RGB(255,255,0), 2 );
      left=numpy.max(points[1])
      right=numpy.min(points[1])
      center=int((left+right)/2)
      old_target[0]=center
 
      left_points=[]
      right_points=[]
      for i in range(0,len(points[1])):
	if points[1][i]<center:
	  left_points.append(points[1][i])
	else:
	  right_points.append(points[1][i])
      if len(left_points)>0:
	diff_left=numpy.max(left_points)-numpy.min(left_points)
      if len(right_points)>0:
	diff_right=numpy.max(right_points)-numpy.min(right_points)
      if len(left_points)> 0 and len(right_points)>0:
	diff_mean=(diff_left+diff_right)/2
      if len(left_points)> 0 and len(right_points)==0:
	diff_mean=diff_left
      if len(left_points)== 0 and len(right_points)>0:
	diff_mean=diff_right
      
      if diff_mean<>0:
	angle=math.atan2(corridor, diff_mean)
      else:
	angle=0
      
    cv.Circle( frame, (old_target[0], old_target[1]), 5,  cv.CV_RGB(255,0,0), 2 );	
    
    if len(points[1])>0:
      start=(old_target[0],old_target[1])
      end=(int(old_target[0]+math.cos(angle)*30),int(old_target[1]-math.sin(angle)*30))
      cv.Line( frame, start, end, cv.CV_RGB(255,0,0), 3, 8 ) 
    
    output="Next positon x=%3.1f y=%3.1f"%(old_target[0],old_target[1])
    cv.PutText( frame,output,(20, 20), font, cv.CV_RGB(0,0,0));	

    if frame_on==False:
      output="Video (no contour)  visible   (f)"
    else:
      output="(no video) Contoure visible    (f)"	
    cv.PutText(frame , output, (350,300), font, cv.CV_RGB(0,255,0));      
    if points_on==False:
      output="Points              visible   (p)"
    else:
      output="Points              invisible (p)"	
    cv.PutText(frame , output, (350,300+font_dist), font, cv.CV_RGB(0,255,0));   
    if init_on==True:
      output="Initiation phase              (i)"
    else:
      output="Running mode                  (i)"	
    cv.PutText(frame , output, (350,300+2*font_dist), font, cv.CV_RGB(0,255,0));

    cv.ShowImage("camera", frame)
    message=struct.pack('IIi',old_target[0], old_target[1], int(angle*180/math.pi)) 
    e=event.Event(subject,message)
    pub.publish(e)

    k = cv.WaitKey (5)
    if k>0:
      k = chr(k%256)
      if k == 'q':
	break
      if k == 'E':
	canny_init=canny_init+5
	print "canny_init = %d"%canny_init
      if k == 'e':
	canny_init=canny_init-5
	print "canny_init = %d"%canny_init
      if k == 'L':
	canny_link=canny_link+5
	print "canny_link = %d"%canny_link
      if k == 'l':
	canny_link=canny_link-5
	print "canny_link = %d"%canny_link
      if k == 'f':
	if frame_on == False:
	  frame_on = True
	else:
	  frame_on = False
      if k == 'b':
	if border_on == False:
	  border_on = True
	else:
	  border_on = False
      if k == 'p':
	if points_on == False:
	  points_on = True
	else:
	  points_on = False
      if k == 'i':
	if init_on==False:
	  init_on = True
	else:
	  init_on = False