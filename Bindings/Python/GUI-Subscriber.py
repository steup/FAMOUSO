# Famouso
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal, struct
from threading import Thread

from PyQt4 import QtCore, QtGui
from pygame.locals import *
from form_subscribe import Ui_Form

## Asyncore Loop Class

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


class StartQT4(QtGui.QMainWindow):

        OBJCOUNT = 0

        def __init__(self, parent=None):
            QtGui.QWidget.__init__(self, parent)
            self.ui = Ui_Form()
            self.ui.setupUi(self)
            QtCore.QObject.connect(self.ui.pushButton,QtCore.SIGNAL("clicked()"), self.subscribe_dialog)
            QtCore.QObject.connect(self.ui.pushButton_2,QtCore.SIGNAL("clicked()"), self.exit_dialog)

        def sig_handler(signum, frame):
            LogSubscriber.unsubscribe()
            print "Alle Sockets geschlossen, mit ENTER beenden"
            sys.exit(1)

        def subscribe_dialog(self):
            self.subject = str(self.ui.lineEdit.text())
            myCallbackObj = LoggerCallback()
            LogSubscriber = subscriber.SubscriberEventChannel(self.subject)
            LogSubscriber.subscribe(self.subject, myCallbackObj.LogCallback)
            if self.OBJCOUNT == 0:
                self.newLoopThread = LoopThread()
                self.newLoopThread.start()
                self.OBJCOUNT = self.OBJCOUNT + 1

        def exit_dialog(self):
            self.mysub.close()


if __name__ == "__main__":
	app = QtGui.QApplication(sys.argv)
	myapp = StartQT4()
	myapp.show()
	sys.exit(app.exec_())
