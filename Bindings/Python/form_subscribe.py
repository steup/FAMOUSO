# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'sub.ui'
#
# Created: Thu Jun 12 12:28:31 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Subscriber")
        Form.resize(QtCore.QSize(QtCore.QRect(0,0,374,86).size()).expandedTo(Form.minimumSizeHint()))

        self.pushButton = QtGui.QPushButton(Form)
        self.pushButton.setGeometry(QtCore.QRect(170,30,75,23))
        self.pushButton.setObjectName("pushButton")

        self.lineEdit = QtGui.QLineEdit(Form)
        self.lineEdit.setGeometry(QtCore.QRect(30,30,113,20))
        self.lineEdit.setObjectName("lineEdit")

        self.label = QtGui.QLabel(Form)
        self.label.setGeometry(QtCore.QRect(30,10,46,14))
        self.label.setObjectName("label")

        self.pushButton_2 = QtGui.QPushButton(Form)
        self.pushButton_2.setGeometry(QtCore.QRect(280,30,75,23))
        self.pushButton_2.setObjectName("pushButton_2")

        self.retranslateUi(Form)
        QtCore.QObject.connect(self.pushButton_2,QtCore.SIGNAL("clicked()"),Form.close)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Subscriber", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton.setText(QtGui.QApplication.translate("Form", "Subscribe", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "Subject", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_2.setText(QtGui.QApplication.translate("Form", "Close", None, QtGui.QApplication.UnicodeUTF8))

