# -*- coding: cp1252 -*-
import socket           # f�r Socket und Netzwerkbehandlung erforderlich
import struct           # zum Verpacken der Events in ein Netzwerkformat
import asyncore         # zum Verwalten von asynchronen Socket Threads
import time             # f�r Performance Anpassungen bei schnell aufeinderfolgenden Sende-Anweisungen
import event            # die Event Klasse
from Queue import Queue # die Warteschlange f�r den Storage Subscriber

# Gloabale Variablen
# HOST kennzeichnet den Zielrechner, auf dem der Famouso EventChannelHandler l�uft
# PORT kennzeichnet den entpsrechenden Port des ECH

HOST = "127.0.0.1"
PORT = 5005

# EventChannel Klasse
# von dieser Klassen werden der EventChannelPublisher, Subscriber und Storagesubscriber abgeleitet
# diese Klasse bietet alle grundlegenden Schnittstellen zur Kommunikation mit dem Netzwerk
# und wird selbst von asyncore.dispatcher abgeleitet, um die Sockets asynchron zu verwalten

class EventChannel(asyncore.dispatcher):

    def __init__(self,  subject):               # Konstruktor, ben�tigt subject als String
        # es folgen drei Attribute, welche gemeinsam das Datenpacket bilden, welches direkt an den ECH versendet wird
        self.subject = subject
        self.size = 0
        self.data_package = ""
        # Aufruf des Konstruktors vom dispatcher
        asyncore.dispatcher.__init__(self)
        # Erstellen und verbinden des Sockets
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect((HOST, PORT))
        # Puffer um das Sockethandling abzuschlie�en, bevor das erste Datenpacket versendet wird
        time.sleep(0.2)

        # Definition der Handler Anweisungen vom Asyncore Modul
    def handle_connect(self):   # Handler f�r das Verbinden von Sockets
        pass

    def handle_expt(self):      # Handler f�r das Auftreten von Exceptions
        print "Asyncore Exception, Socket wird geschlossen..."
        self.close()

    def handle_close(self):     # Handler f�r das Schliessen von Socketverbindungen
        print "Handle_Close, Socket wird geschlossen..."
        self.close()
    
    def handle_error(self):     # Handler f�r das Auftreten von Fehlern jeglicher Art in den Anwendungen (zB. fehlerhafter Callback)
        print "Handle_Error, Socket wird geschlossen...Anwendung fehlerhaft"
        self.close()
    
    def announce(self,  subject):                           # Announce Methode ben�tigt ein subject als String
        if self.subject == subject:                         # Pr�fung ob das angegebene Subject auch zum Subject des Konstruktors passt
            print "starte announce auf: " + str(subject)
            self.subject = subject.decode("hex")            # Umwandlung des Subject Strings in Hex Code
            subject_len = len(self.subject)                 # Pr�fen der Subjectl�nge auf 8 Byte, sollte das Subject kleiner sein,
            zero_data = struct.pack("!b", 0x00)             # wird es mit 00 aufgef�llt (zeroByte)
        
            while subject_len < 8:                          # Auff�llen auf 8 Byte
                subject_len = subject_len + 1
                self.subject = self.subject + zero_data
                
            self.data_package = "V" + self.subject          # Anf�gen des Op Codes an das Subject, 'V' stellt den Op Code f�r Announce dar
            self.socket.send(self.data_package)             # Versenden des Announce Daten Pakets
        else: 
            print "falsches Subject?"

    def publish(self, myEvent):                             # Publish Methode, ben�tigt ein Event
        self.size = myEvent.size                            # entnehmen der Size aus dem Event
        self.subject = myEvent.subject.decode("hex")        # Umwandlung des Subject Strings in Hex Code
        subject_len = len(self.subject)                     # Pr�fen der Subjectl�nge auf 8 Byte, Verfahren analog der Announce Methode
        zero_data = struct.pack("!b", 0x00)
        
        while subject_len < 8:
            subject_len = subject_len + 1
            self.subject = self.subject + zero_data

        # Verpacken des Op Codes 'R' (f�r Publish), des Subjects, der Umwandlung der Size in einen Netzwerk Integer Wert und anf�gen des Content Strings

        self.data_package = "R"+self.subject+struct.pack("!i", self.size)+myEvent.content
        self.socket.send(self.data_package)                 # Versenden des Daten Packets
        
    def socket_close(self):                                 # Socket Close Methode, notwendig um einen sauberen unsubscribe oder unannounce durchzuf�hren
        self.close()

    def subscribe(self,  subject):                          # Subscribe Methode, ben�tigt Subject als String
        self.subject = subject.decode("hex")                # Umwandlung des Subject Strings in Hex Code
        subject_len = len(self.subject)                     # Pr�fen der Subjectl�nge auf 8 Byte, Verfahren analog der Announce Methode
        zero_data = struct.pack("!b", 0x00)
        
        while subject_len < 8:
            subject_len = subject_len + 1
            self.subject = self.subject + zero_data
        print "starte subscribe auf: " + str(subject)
        
        self.data_package = "P"+self.subject                # Bauen des Daten Packets aus dem Op Code 'P' f�r Subscribe und dem Subject
        self.socket.send(self.data_package)                 # Versenden des Daten Packets
