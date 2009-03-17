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
