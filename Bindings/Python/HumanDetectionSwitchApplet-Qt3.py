# Famouso
from pyFAMOUSO.famouso import *

import signal, struct, sys
from qt import *

# standard bibliothek
from threading import Thread

class MyDialog(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("MyDialog")

        self.setSizeGripEnabled(1)


        self.radioHuman = QRadioButton(self,"radioHuman")
        self.radioHuman.setGeometry(QRect(50,10,150,30))

        LayoutWidget = QWidget(self,"Layout1")
        LayoutWidget.setGeometry(QRect(220,10,140,30))
        Layout1 = QHBoxLayout(LayoutWidget,0,6,"Layout1")

        self.buttonClose = QPushButton(LayoutWidget,"buttonClose")
        self.buttonClose.setAutoDefault(1)
        Layout1.addWidget(self.buttonClose)

        self.languageChange()

        self.resize(QSize(410,50).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.subject = "48756D616E446574"
        self.pub = publisher.PublisherEventChannel(self.subject)
        self.pub.announce(self.subject)
        self.state=0;

        self.connect(self.radioHuman, SIGNAL("clicked()"), self.PublishHumanDetection)
        self.connect(self.buttonClose,SIGNAL("clicked()"), self.exit_dialog)


    def languageChange(self):
        self.setCaption(self.__tr("Human Detection Switch Applet"))
        self.radioHuman.setText(self.__tr("Human detected"))
        self.buttonClose.setText(self.__tr("&Close"))
        self.buttonClose.setAccel(QKeySequence(self.__tr("Alt+C")))


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
            quit()

    def __tr(self,s,c = None):
        return qApp.translate("MyDialog",s,c)

config.HOST="192.168.0.1"

if __name__ == "__main__":
    a = QApplication(sys.argv)
    QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
    w = MyDialog()
    a.setMainWidget(w)
    w.show()
    a.exec_loop()
