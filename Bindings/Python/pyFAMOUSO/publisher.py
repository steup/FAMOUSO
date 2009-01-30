# -*- coding: cp1252 -*-
# Publisher Klasse

# import des Famouso Moduls
from famouso import *


class PublisherEventChannel(EventChannel):      # Ableitung des EventChannel

    def announce(self, subject=None):                # Aufruf der Eventchannel Announce Methode
        EventChannel.announce(self, subject)

    def publish(self, myEvent):                 # Aufruf der Eventchannel Publish Methode
        EventChannel.publish(self, myEvent)

    def unannounce(self):                       # Unannounce schliesst den aktiven Socket
        self.socket_close()

# Zusätzliches Überschreiben der nicht verwendeten Methoden
# damit sie von einem Publisher Objekt aus nicht verwendet werden können

    def handle_write(self):
        pass

    def handle_read(self):
        pass

    def subscribe(self):
        pass
        
    def unsubscribe(self):
        pass
