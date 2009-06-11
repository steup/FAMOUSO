################################################################################
##
## Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
## All rights reserved.
##
##    Redistribution and use in source and binary forms, with or without
##    modification, are permitted provided that the following conditions
##    are met:
##
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in
##      the documentation and/or other materials provided with the
##      distribution.
##
##    * Neither the name of the copyright holders nor the names of
##      contributors may be used to endorse or promote products derived
##      from this software without specific prior written permission.
##
##
##    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
##    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
##    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
##    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
##    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
##    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
##    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
##    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##
## $Id$
##
################################################################################

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
import serial

RobotID=51

# ----- Definition -----------------------------------
#~ Distance Crash Human SteeringL SteeringR;

centerDistance=0
velocity_right=0
velocity_left=0
crash=0

def DistanceCallBack(myEvent):
    global centerDistance
    [ID, Dl, Dc, Dr]=struct.unpack('BBBB',myEvent.content)
    if ID==RobotID:
        centerDistance=int(Dc)
        if int(Dc)>75:
            output=struct.pack('BBBBBB',65,100,0,0,0,0)
        else:
            output=struct.pack('BBBBBB',65,20,0,0,0,0)
        ser.write(output)

def Crashed_CallBack(myEvent):
    global crash
    crash=1
    print "Crash !!"
    output=struct.pack('BBBBBB',65,0,0,1,0,0)
    ser.write(output)

def HumanDetCallBack(myEvent):
    [ID, human]=struct.unpack('BB',myEvent.content)
    if int(human)==1:
        output=struct.pack('BBBBBB',65,0,1,0,0,0)
        print "Emergency situation !!!"
    else:
        output=struct.pack('BBBBBB',65,0,2,0,0,0)
        print "Emergency situation terminated !!!"
    ser.write(output)


def sig_handler(signum, frame):
        DistanceSub.unsubscribe()
        Crashed_Sub.unsubscribe()
        print "Alle FAMOUSO Kanaele geschlossen"
        #ser.close()
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
DistanceSub = subscriber.SubscriberEventChannel(DistanceSubject)
DistanceSub.subscribe(DistanceSubject, DistanceCallBack)

Crashed_Subject = "Crashed_"
Crashed_Sub = subscriber.SubscriberEventChannel(Crashed_Subject)
Crashed_Sub.subscribe(Crashed_Subject, Crashed_CallBack)

HumanDetSubject = "HumanDet"
HumanDetSub = subscriber.SubscriberEventChannel(HumanDetSubject)
HumanDetSub.subscribe(HumanDetSubject, HumanDetCallBack)

ser=serial.Serial()
ser.baudrate=19200
ser.port='COM1'
ser.timeout=0.05
ser.open()
if ser.isOpen():
    print "Serielle Verbindung geoeffnet"
else:
    print "Fehler mit der seriellen Verbindung"
while 1:
    asyncore.poll(timeout=0.01)
    line=ser.readline()
    if len(line)>1:
        if line.find("B")==1:
            list=line[2:-1].split(' ')
            if len(list) >= 2:
                velocity_left=list[1]
            if len(list) >= 3:
                velocity_right=list[2]
                output=struct.pack('BBB',RobotID,int(velocity_left),int(velocity_right))
                e=event.Event(VelocitySubject,output)
                VelocityPub.publish(e)
            if len(list) >= 4:
                sensor=list[3]
                #print sensor
                if robotActive==False:
                    print "Roboter wurde aktivert"
                    robotActive=True
