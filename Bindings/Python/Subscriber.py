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

# Famouso
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal, struct
from threading import Thread

## Globale Variablen

LogChannel = "Velocity"
LogSubscriber = subscriber.SubscriberEventChannel(LogChannel)

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

class LoggerCallback:
    def LogCallback(self, myEvent):
        #self.receivedString = myEvent.content
        print "recv: " + str(myEvent.content)
        print str(myEvent.subject)


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


