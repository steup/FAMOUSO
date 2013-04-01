#!/usr/bin/env python

import sys

if sys.platform == 'win32':
    sys.path.append('..\\..\\..\\Bindings\\Python')
else:
    sys.path.append('../../../Bindings/Python/')

from pyFAMOUSO.publisher import PublisherEventChannel
from pyFAMOUSO.event import Event

from virtual_sensor_ui import Ui_Frame as Dlg
from PyQt4 import QtCore, QtGui

from struct import *
from pyFAMOUSO.famouso import *

class GUI( QtGui.QFrame, Dlg):
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.setupUi(self)

        self.connect(self.pushButton_Connect, QtCore.SIGNAL("clicked()"), self.connectToFamouso)

        self.connect(self.sliderX, QtCore.SIGNAL("valueChanged(int)"), self.slideX)
        self.connect(self.sliderY, QtCore.SIGNAL("valueChanged(int)"), self.slideY)
        self.connect(self.sliderZ, QtCore.SIGNAL("valueChanged(int)"), self.slideZ)
        self.connect(self.sliderR, QtCore.SIGNAL("valueChanged(int)"), self.slideR)
        self.connect(self.dial,    QtCore.SIGNAL("valueChanged(int)"), self.changeDistance)

        self.bConnected = False

        self.pos_X = 0
        self.pos_Y = 0
        self.pos_Z = 0
        self.rot   = 0

        self.distance = 0

        self.subject    = "distance"
        self.sender_id  = 1


    def connectToFamouso(self):
        if self.bConnected :
            self.sendEvent(msg=2)
            self.pub.close()
            self.pushButton_Connect.setText("Announce")
            self.bConnected = False
        else:
            self.pub = publisher.PublisherEventChannel(self.subject)
            self.pub.announce(self.subject)
            self.pushButton_Connect.setText("Unannounce")
            self.bConnected = True
            self.sendEvent(msg=1)

    def slideX(self, val):
        self.label_X.setText(str(val/10.0))
        self.pos_X = val
        self.sendEvent()

    def slideY(self, val):
        self.label_Y.setText(str(val/10.0))
        self.pos_Y = val
        self.sendEvent()

    def slideZ(self, val):
        self.label_Z.setText(str(val/10.0))
        self.pos_Z = val
        self.sendEvent()

    def slideR(self, val):
        self.label_R.setText(str(val))
        self.rot = val
        self.sendEvent()

    def changeDistance(self, val):
        self.label_Dist.setText(str(val))
        self.distance = val
        self.sendEvent()

    def sendEvent(self, msg=0):
        if self.bConnected:
            msg = pack('bbhhhhh',   self.sender_id,
                                    msg,
                                    self.distance,
                                    self.pos_X,
                                    self.pos_Y,
                                    self.pos_Z,
                                    self.rot )

            self.pub.publish( event.Event( self.subject, msg) )


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    dialog = GUI()
    dialog.show()
    sys.exit(app.exec_())
