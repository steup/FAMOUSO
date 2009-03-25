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

