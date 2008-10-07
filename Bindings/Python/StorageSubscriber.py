# Famouso
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal, struct
from threading import Thread

## Globale Variablen

LogChannel = "f2"
Qsize = 3
LogSubscriber = storage_subscriber.StorageSubscriberEventChannel(LogChannel, Qsize)

## Asyncore Loop Class

class LoopThread(Thread):
    def __init__(self):
        Thread.__init__(self)
    def run(self):
        try:
            asyncore.loop()
            print "asyncore.loop beendet"
        except:
            print "asyncore.loop wurde extern beendet"


## Callback Klasse

class LoggerCallback():
    def LogCallback(self, myEvent):
        for x in range(Qsize):
            print str(LogSubscriber.getQueueData().content)
        #print "recv: " + str(myEvent.content)


## Singal Handler

def sig_handler(signum, frame):
        LogSubscriber.unsubscribe()
        print "Alle Sockets geschlossen, mit ENTER beenden"
        sys.exit(1)


## MAIN

myCallbackObj = LoggerCallback()
LogSubscriber.subscribe(LogChannel, myCallbackObj.LogCallback)


## Asyncore Loop & Signal Handling
print "Mit STRG + C beenden."

signal.signal(signal.SIGINT, sig_handler)

MainLoop = LoopThread()
MainLoop.start()

while 1:
    signal = 0 # Listen for signals

