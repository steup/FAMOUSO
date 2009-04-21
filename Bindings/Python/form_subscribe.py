# -*- coding: utf-8 -*-
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

