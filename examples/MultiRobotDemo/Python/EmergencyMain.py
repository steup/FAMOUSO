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

# include additional paths
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

from PyQt4 import QtGui ,  QtCore
from Ui_EmergencySwitch import Ui_Emergency as Dlg

import re

class LoopThread(Thread):

    def __init__(self):
        Thread.__init__(self)

    def run(self):
        print "neue asyncore.loop gestartet"
        asyncore.loop()
        print "asyncore.loop beendet"
        myapp.OBJCOUNT = 0

## Callback Klasse

class LoggerCallback:
    def LogCallback(self, myEvent):
        print "recv from "+str(hex(ord(str(myEvent.subject[0]))))+ " : " + str(myEvent.content)
        if str(myEvent.content[1])=="\0":
		print "No Emergency situation"
		dialog.EmgergencySituation=False
		dialog.labelOutput.setText("Alles ok!")
	else:
		print "Emergency situation !!!"
		dialog.EmgergencySituation=True
		dialog.labelOutput.setText("STOP !!!")

class EmergencyDialog(QtGui.QDialog, Dlg):

    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.setupUi(self)
        self.EmergencySituation=False
        self.connect(self.pushButtonQuit, QtCore.SIGNAL("clicked()"), self.onQuit)
        self.connect(self.pushButtonEmergency, QtCore.SIGNAL("clicked()"), self.onEmergency)
        self.connect(self.pushButtonTerminate, QtCore.SIGNAL("clicked()"), self.onTerminate)
        self.subject = "48756D616E446574"
	self.pub = publisher.PublisherEventChannel(self.subject)
	self.pub.announce(self.subject)

    def onQuit(self):
        print "Aus Maus"
        LogSubscriber.unsubscribe()
	e=event.Event(self.subject,str('\0\0B'))
        self.pub.publish(e)
        self.pub.close()
        self.close()

    def onTerminate(self):
	self.labelOutput.setText("Alles ok!")
	self.EmergencySituation=False
	e=event.Event(self.subject,str('\0\0B'))
	self.pub.publish(e)

    def onEmergency(self):
        if  self.EmergencySituation==True:
            print "Emergency detected already !"
        if  self.EmergencySituation==False:
            print "Emergency situation !"
            self.labelOutput.setText("STOP !!!")
            self.EmergencySituation=True
	    e=event.Event(self.subject,str('\0\1B'))
	    self.pub.publish(e)

# Main
app = QtGui.QApplication(sys.argv)
dialog = EmergencyDialog()
dialog.show()

subject = "48756D616E446574"
myCallbackObj = LoggerCallback()
LogSubscriber = subscriber.SubscriberEventChannel(subject)
LogSubscriber.subscribe(subject, myCallbackObj.LogCallback)
newLoopThread = LoopThread()
newLoopThread.start()

sys.exit(app.exec_())
