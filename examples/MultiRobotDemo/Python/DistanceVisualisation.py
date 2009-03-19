#!/usr/bin/env python

# make Famouso Python Bindings Package available in the path
# for the package loader
import sys
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
RobotID=4

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
        e=struct.unpack("BcB",myEvent.content)
        if e[2] == RobotID:
            if e[0] < 60:
                displayDistance.update(e[0])
            else:
                displayDistance.update(60)


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
