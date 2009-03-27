# include additional paths 
import sys, os
if sys.platform == 'win32':
	sys.path.append('..\\..\\..\\Bindings\\Python')
else:
	sys.path.append('../../../Bindings/Python/')

from threading import Thread
# load the Famouso Python Bindings
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal

# serial port communication
# import serial

RobotID=51

# ----- Definition -----------------------------------
#~ Human=input[2];
#~ Crash=input[1];
#~ VirtualSensor=input[0];

def DistanceCallBack(myEvent):
	# [ID, leftDistance, centerDistance, rightDistance]
		[ID, Dl, Dc, Dr]=struct.unpack('BBBB',myEvent.content)
		if ID==RobotID:
			ser.send ?
			print int(Dc)
		
def CrashCallback(myEvent):
		print "Crash !!"
		
def HumanDetCallback(myEvent):
		print "Human !!!"

def sig_handler(signum, frame):
        LogSubscriber.unsubscribe()
        print "Alle FAMOUSO Kanaele geschlossen"
        ser.close()
        print "Serielle Schnittstelle geschlossen"
        print "Aus Maus"
        sys.exit(1)

# -----------------------------------------------------------
signal.signal(signal.SIGINT, sig_handler)

robotActive=False

# Velocity
VelocitySubject = "Velocity"
VelocityPub = publisher.PublisherEventChannel(VelocitySubject)
VelocityPub.announce(VelocitySubject)

DistanceSubject = "Distance"
LogSubscriber = subscriber.SubscriberEventChannel(DistanceSubject)
LogSubscriber.subscribe(DistanceSubject, DistanceCallBack)

#~ ser=serial.Serial()
#~ ser.baudrate=19200
#~ ser.port='COM1'
#~ ser.timeout=0.05
#~ ser.open()
#~ if ser.isOpen():
	#~ print "Serielle Verbindung geoeffnet"
while 1:
	asyncore.poll(timeout=0.01)
	output=struct.pack('BBB',RobotID,10,10)
	e=event.Event(VelocitySubject,output)
	VelocityPub.publish(e)
	time.sleep(0.2)
	#~ line=ser.readline()
	#~ if len(line)>1:
		#~ if line.find("B")==1:
			#~ list=line[2:-1].split(' ')
			#~ if len(list) >= 2:
				#~ velocity_left=list[1]
			#~ if len(list) >= 3:
				#~ velocity_right=list[2]
				#~ output=struct.pack('BBB',RobotID,int(velocity_left),int(velocity_right))
				#~ e=event.Event(VelocitySubject,output)
				#~ VelocityPub.publish(e)
			#~ if len(list) >= 4:
				#~ sensor=list[3]
				#~ if robotActive==False:
					#~ print "Roboter wurde aktivert"
					#~ robotActive=True
