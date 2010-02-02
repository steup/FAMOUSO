################################################################################
##
## Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#!/usr/bin/env python

# make Famouso Python Bindings Package available in the path
# for the package loader
import sys, os
if sys.platform == 'win32':
	sys.path.append('..\\..\\..\\Bindings\\Python')
else:
	sys.path.append('../../../Bindings/Python/')

# load the Famouso Python Bindings
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal, struct
from threading import Thread

import math
from random import randint
import pygame

from array import array

global old
old = 60

class DistanceVisualiser:
    def __init__(self):
        # set basic parameters such as width and height of the screen and so on
        self.width=400
        self.height=200
        self.act_y=self.height / 2
        self.max=100

        # init pygame and setup screen
        pygame.init()
        pygame.display.init()
        pygame.display.set_mode((self.width,self.height),pygame.DOUBLEBUF|pygame.HWSURFACE)

        # make black surface for clearing the screen... faster than fill?
        self.screen = pygame.display.get_surface()
        self.black = pygame.Surface((self.width,self.height))
        # initialize the balls-array
        self.distance_array = []

        # calculate inital stars
        for i in range(self.max):
            self.distance_array.append(60)

        self.clock = pygame.time.Clock()

    def update(self, value):
        self.screen.fill((0,0,0))
        del self.distance_array[0]
        self.distance_array.append(value)
        # Starupdate
        for dis in xrange(1,self.max-1):
            pygame.draw.line(self.screen,(125,255,155),(dis*4,self.height-self.distance_array[dis]*2),(4*(dis+1),self.height-self.distance_array[dis+1]*2))
        pygame.display.flip()



## Globale Variablen
RobotID=1

## Asyncore Loop Class

class LoopThread(Thread):
    def __init__(self):
        Thread.__init__(self)
    def run(self):
        try:
            print "asyncore.loop beendet"
        except:
            print "asyncore.loop wurde extern beendet"


## Callback Klasse

class CallBack:

    def LogCallback(self, myEvent):
        global old
        if len(myEvent.content)==4:
            e=struct.unpack("BBBB",myEvent.content)
            if e[2] < 60:
                displayDistance.update(e[2])
                old = e[2]
            else:
                displayDistance.update(old)
#      e=struct.unpack("BBB",myEvent.content)
 #       print e
 #       if e[0] == RobotID:
 #           if e[0] < 60:
 #               displayDistance.update(e[0])
 #           else:
 #               displayDistance.update(60)


## Singal Handler

def sig_handler(signum, frame):
        LogSubscriber.unsubscribe()
        print "Alle Sockets geschlossen, mit ENTER beenden"
        sys.exit(1)


## MAIN
if len(sys.argv)>1:
    RobotID=int(sys.argv[1])
signal.signal(signal.SIGINT, sig_handler)

displayDistance=DistanceVisualiser()

myCallBack=CallBack()
LogChannel = "Distance"
LogSubscriber = subscriber.SubscriberEventChannel(LogChannel)
LogSubscriber.subscribe(LogChannel, myCallBack.LogCallback)

## Asyncore Loop & Signal Handling
print "Mit STRG + C beenden."
asyncore.loop()
#while True:
#    asyncore.loop(10,True,None,1)
#    time.sleep(0.01)
#
