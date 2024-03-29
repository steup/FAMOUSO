# -*- coding: cp1252 -*-
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

import socket           # f�r Socket und Netzwerkbehandlung erforderlich
import struct           # zum Verpacken der Events in ein Netzwerkformat
import asyncore         # zum Verwalten von asynchronen Socket Threads
import time             # f�r Performance Anpassungen bei schnell aufeinderfolgenden Sende-Anweisungen
import event            # die Event Klasse
from Queue import Queue # die Warteschlange f�r den Storage Subscriber
import config

# EventChannel Klasse
# von dieser Klassen werden der EventChannelPublisher, Subscriber und Storagesubscriber abgeleitet
# diese Klasse bietet alle grundlegenden Schnittstellen zur Kommunikation mit dem Netzwerk
# und wird selbst von asyncore.dispatcher abgeleitet, um die Sockets asynchron zu verwalten

class EventChannel(asyncore.dispatcher):

    def __init__(self,  subject):               # Konstruktor, ben�tigt subject als String
        # es folgen drei Attribute, welche gemeinsam das Datenpacket bilden, welches direkt an den ECH versendet wird
        self.subject = subject
        # Aufruf des Konstruktors vom dispatcher
        asyncore.dispatcher.__init__(self)
        # Erstellen und verbinden des Sockets
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect((config.HOST, config.PORT))
        # Puffer um das Sockethandling abzuschlie�en, bevor das erste Datenpacket versendet wird
        time.sleep(0.2)

        # Definition der Handler Anweisungen vom Asyncore Modul
    def handle_connect(self):   # Handler f�r das Verbinden von Sockets
        pass

    def handle_expt(self):      # Handler f�r das Auftreten von Exceptions
        self.close()
        print "handle_expt() -- Asyncore Exception, socket closed..."

    def handle_close(self):     # Handler f�r das Schliessen von Socketverbindungen
        self.close()
        print "handle_close() -- Asyncore Exception, socket closed..."

    def handle_error(self):     # Handler f�r das Auftreten von Fehlern jeglicher Art in den Anwendungen (zB. fehlerhafter Callback)
        self.close()
        print "handle_error() -- Asyncore Exception, socket closed..."

    def encoded_subject(self):
	# versuche das subject als Hex-Zahl, Fallback zu ASCII
	try:
		s = self.subject.decode("hex")
	except:
		s = self.subject
	if len(s) > 8:
		raise TypeError("subject must be <= 8 chars or <= 16 hex digits!")
	return struct.pack("8s", s)

    def announce(self,  subject=None):                    # Announce Methode ben�tigt ein subject als String
	if subject and self.subject != subject:
            raise ValueError("stored subject and announce() subject mismatch!")

	data_package = "V" + self.encoded_subject()         # Anf�gen des Op Codes an das Subject, 'V' stellt den Op Code f�r Announce dar
	self.socket.send(data_package)                      # Versenden des Announce Daten Pakets

    def publish(self, myEvent):                             # Publish Methode, ben�tigt ein Event
        if self.subject != myEvent.subject:                 # Pr�fung ob das angegebene Subject auch zum Subject des Konstruktors passt
            raise ValueError("stored subject and announce() subject mismatch!")

        # Verpacken des Op Codes 'R' (f�r Publish), des Subjects, der Umwandlung der Size in einen Netzwerk Integer Wert und anf�gen des Content Strings
        data_package = "R" + self.encoded_subject() + \
		struct.pack("!i", myEvent.size) + myEvent.content
        self.socket.send(data_package)                      # Versenden des Daten Packets

    def socket_close(self):                                 # Socket Close Methode, notwendig um einen sauberen unsubscribe oder unannounce durchzuf�hren
        self.close()

    def subscribe(self,  subject=None):             # Subscribe Methode, ben�tigt Subject als String
        if subject and self.subject != subject:
            raise ValueError("stored subject and announce() subject mismatch!")
        self.data_package = "P"+self.encoded_subject()      # Bauen des Daten Packets aus dem Op Code 'P' f�r Subscribe und dem Subject
        self.socket.send(self.data_package)                 # Versenden des Daten Packets
