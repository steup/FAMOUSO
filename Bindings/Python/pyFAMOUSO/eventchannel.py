# -*- coding: cp1252 -*-
import socket           # für Socket und Netzwerkbehandlung erforderlich
import struct           # zum Verpacken der Events in ein Netzwerkformat
import asyncore         # zum Verwalten von asynchronen Socket Threads
import time             # für Performance Anpassungen bei schnell aufeinderfolgenden Sende-Anweisungen
import event            # die Event Klasse
from Queue import Queue # die Warteschlange für den Storage Subscriber
import config

# EventChannel Klasse
# von dieser Klassen werden der EventChannelPublisher, Subscriber und Storagesubscriber abgeleitet
# diese Klasse bietet alle grundlegenden Schnittstellen zur Kommunikation mit dem Netzwerk
# und wird selbst von asyncore.dispatcher abgeleitet, um die Sockets asynchron zu verwalten

class EventChannel(asyncore.dispatcher):

    def __init__(self,  subject):               # Konstruktor, benötigt subject als String
        # es folgen drei Attribute, welche gemeinsam das Datenpacket bilden, welches direkt an den ECH versendet wird
        self.subject = subject
        # Aufruf des Konstruktors vom dispatcher
        asyncore.dispatcher.__init__(self)
        # Erstellen und verbinden des Sockets
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect((config.HOST, config.PORT))
        # Puffer um das Sockethandling abzuschließen, bevor das erste Datenpacket versendet wird
        time.sleep(0.2)

        # Definition der Handler Anweisungen vom Asyncore Modul
    def handle_connect(self):   # Handler für das Verbinden von Sockets
        pass

    def handle_expt(self):      # Handler für das Auftreten von Exceptions
        self.close()
        print "handle_expt() -- Asyncore Exception, socket closed..."

    def handle_close(self):     # Handler für das Schliessen von Socketverbindungen
        self.close()
        print "handle_close() -- Asyncore Exception, socket closed..."

    def handle_error(self):     # Handler für das Auftreten von Fehlern jeglicher Art in den Anwendungen (zB. fehlerhafter Callback)
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

    def announce(self,  subject=None):                    # Announce Methode benötigt ein subject als String
	if subject and self.subject != subject:
            raise ValueError("stored subject and announce() subject mismatch!")

	data_package = "V" + self.encoded_subject()         # Anfügen des Op Codes an das Subject, 'V' stellt den Op Code für Announce dar
	self.socket.send(data_package)                      # Versenden des Announce Daten Pakets

    def publish(self, myEvent):                             # Publish Methode, benötigt ein Event
        if self.subject != myEvent.subject:                 # Prüfung ob das angegebene Subject auch zum Subject des Konstruktors passt
            raise ValueError("stored subject and announce() subject mismatch!")

        # Verpacken des Op Codes 'R' (für Publish), des Subjects, der Umwandlung der Size in einen Netzwerk Integer Wert und anfügen des Content Strings
        data_package = "R" + self.encoded_subject() + \
		struct.pack("!i", myEvent.size) + myEvent.content
        self.socket.send(data_package)                      # Versenden des Daten Packets

    def socket_close(self):                                 # Socket Close Methode, notwendig um einen sauberen unsubscribe oder unannounce durchzuführen
        self.close()

    def subscribe(self,  subject=None):             # Subscribe Methode, benötigt Subject als String
        if subject and self.subject != subject:
            raise ValueError("stored subject and announce() subject mismatch!")
        self.data_package = "P"+self.encoded_subject()      # Bauen des Daten Packets aus dem Op Code 'P' für Subscribe und dem Subject
        self.socket.send(self.data_package)                 # Versenden des Daten Packets
