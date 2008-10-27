# -*- coding: cp1252 -*-
# Subscriber Klasse

#import famouso Modul
from famouso import *

class SubscriberEventChannel(EventChannel):     # Abgeleitet vom EventChannel

    def __init__(self,  subject):               # Konstruktor ben�tigt subject
        EventChannel.__init__(self, subject)    # Konstruktor vom Eventchannel aufrufen
        self.callback = 1                       # Hinzuf�gen eines Callback Attributs

    def subscribe(self, subject, callback_func):    # Subscribe Methode erfordert Subject und Callback
        self.callback = callback_func               # �bergabe der Callbackfunktion
        EventChannel.subscribe(self, subject)       # Aufruf der EventChannel Methode

    def unsubscribe(self):      # Unsubscribe l�st socket_close aus
        self.socket_close()

# Handler f�r das 'Lauschen' am Socket
# Es werden zuerst 13 Byte gelesen
# d.h. von einem Event wird der Op Code, das Subject und die Size ermittelt
# danach werden entsprechend der erhaltenen Size noch weitere Byte gelesen
# diese bilden dann den Content des Events

    def handle_read(self):
        head = self.socket.recv(13)                         # 13 Byte f�r Kopf eines Events
        self.myEvent = event.Event(head[1:8], self.socket.recv(int(ord(head[12]))))  # Generieren eines 'dummy' Events um den Content aufzunehmen
#        time.sleep(0.1)                                     # Performance Puffer
        #self.myEvent.content = self.socket.recv(int(ord(head[12]))) # Lesen des Contents
        self.callback(self.myEvent)                         # Aurufen des Callbacks mit Event als �bergabe

# Zus�tzliches �berschreiben der nicht verwendeten Methoden
# damit sie von einem Publisher Objekt aus nicht verwendet werden k�nnen

    def handle_write(self):
        pass

    def announce(self):
        pass

    def publish(self):
        pass

    def unannounce(self):
        pass
