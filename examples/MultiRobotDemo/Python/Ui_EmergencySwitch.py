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

from PyQt4 import QtCore, QtGui

class Ui_Emergency(object):
    def setupUi(self, Emergency):
        Emergency.setObjectName("Emergency")
        Emergency.resize(470,165)
        self.groupBox = QtGui.QGroupBox(Emergency)
        self.groupBox.setGeometry(QtCore.QRect(20,10,431,141))
        self.groupBox.setObjectName("groupBox")
        self.pushButtonEmergency = QtGui.QPushButton(self.groupBox)
        self.pushButtonEmergency.setGeometry(QtCore.QRect(290,20,121,31))
        self.pushButtonEmergency.setObjectName("pushButtonEmergency")
        self.pushButtonQuit = QtGui.QPushButton(self.groupBox)
        self.pushButtonQuit.setGeometry(QtCore.QRect(290,100,121,31))
        self.pushButtonQuit.setObjectName("pushButtonQuit")
        self.labelOutput = QtGui.QLabel(self.groupBox)
        self.labelOutput.setGeometry(QtCore.QRect(20,40,241,61))
        self.labelOutput.setStyleSheet("font: 75 48pt ""MS Shell Dlg 2"";")
        self.labelOutput.setObjectName("labelOutput")
        self.pushButtonTerminate = QtGui.QPushButton(self.groupBox)
        self.pushButtonTerminate.setGeometry(QtCore.QRect(290,60,121,31))
        self.pushButtonTerminate.setObjectName("pushButtonTerminate")

        self.retranslateUi(Emergency)
        QtCore.QMetaObject.connectSlotsByName(Emergency)

    def retranslateUi(self, Emergency):
        Emergency.setWindowTitle(QtGui.QApplication.translate("Emergency", "Emergency", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("Emergency", "Emergency state", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonEmergency.setText(QtGui.QApplication.translate("Emergency", "Simulate Emergency", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonQuit.setText(QtGui.QApplication.translate("Emergency", "Quit", None, QtGui.QApplication.UnicodeUTF8))
        self.labelOutput.setText(QtGui.QApplication.translate("Emergency", "Alles ok!", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonTerminate.setText(QtGui.QApplication.translate("Emergency", "Terminate Emergency", None, QtGui.QApplication.UnicodeUTF8))

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    Emergency = QtGui.QDialog()
    ui = Ui_Emergency()
    ui.setupUi(Emergency)
    Emergency.show()
    sys.exit(app.exec_())
