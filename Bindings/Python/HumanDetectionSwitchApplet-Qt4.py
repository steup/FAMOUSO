# Famouso
from pyFAMOUSO.famouso import *

# standard bibliothek
import signal, struct
from threading import Thread

from PyQt4 import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("HumanDetectionPublisher")
        Form.resize(QtCore.QSize(QtCore.QRect(0,0,374,86).size()).expandedTo(Form.minimumSizeHint()))

        self.pushButton = QtGui.QRadioButton(Form)
        self.pushButton.setGeometry(QtCore.QRect(50,30,175,23))
        self.pushButton.setObjectName("radioButton")

        self.pushButton_2 = QtGui.QPushButton(Form)
        self.pushButton_2.setGeometry(QtCore.QRect(280,30,75,23))
        self.pushButton_2.setObjectName("pushButton_2")

        self.retranslateUi(Form)
        QtCore.QObject.connect(self.pushButton_2,QtCore.SIGNAL("clicked()"),Form.close)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Human detection switch applet", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton.setText(QtGui.QApplication.translate("Form", "Human detected", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_2.setText(QtGui.QApplication.translate("Form", "Close", None, QtGui.QApplication.UnicodeUTF8))

class StartQT4(QtGui.QMainWindow):

        OBJCOUNT = 0

        def __init__(self, parent=None):
            QtGui.QWidget.__init__(self, parent)
            self.ui = Ui_Form()
            self.ui.setupUi(self)
            QtCore.QObject.connect(self.ui.pushButton,QtCore.SIGNAL("clicked()"), self.PublishHumanDetection)
            QtCore.QObject.connect(self.ui.pushButton_2,QtCore.SIGNAL("clicked()"), self.exit_dialog)
            self.subject = "48756D616E446574"
            self.pub = publisher.PublisherEventChannel(self.subject)
            self.pub.announce(self.subject)
            self.state=0;

        def sig_handler(signum, frame):
            sys.exit(1)

        def PublishHumanDetection(self):
            self.state = ~self.state
            if (self.state == 0):
                e=event.Event(self.subject,str('\0\0B'))
            else:
                e=event.Event(self.subject,str('H\0B'))
            self.pub.publish(e)

        def exit_dialog(self):
            e=event.Event(self.subject,str('\0\0B'))
            self.pub.publish(e)
            self.pub.close()


if __name__ == "__main__":
	app = QtGui.QApplication(sys.argv)
	myapp = StartQT4()
	myapp.show()
	sys.exit(app.exec_())
