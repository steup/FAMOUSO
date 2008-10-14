# -*- coding: cp1252 -*-
# Storage Subscriber Klasse
# beinhaltet Queue und löst einen Callback bei voller Queue aus
# diese Queue speichert empfangene Events komplett (mit Subject und Size)
# mittels der getQueueData() Methode kann dann von aussen auf diese Events zugegriffen werden

# import famouso Modul und Subscriber
from famouso import *
from subscriber import *

class StorageSubscriberEventChannel(SubscriberEventChannel):    # direkte Ableitung vom Subscriber

    def __init__(self,  subject, Qsize):                        # Konstruktor erfordert subject und Größe der Queue
        if Qsize < 1:                                           # bei einer Queue Größe < 1, wird sie auf 1 gesetzt
            Qsize = 1
        self.__myQueue = Queue(Qsize)                           # Erzeugen der Queue mit entsprechender Größe
        SubscriberEventChannel.__init__(self,subject)           # Aufruf des Subscriber Konstruktors, um auch ein Callback Attribut zu erhalten
    
    def subscribe(self, subject, callback_func):                # Subscribe Methode erfordert Subject und Callback
        self.callback = callback_func                           # Übergabe der Callbackfunktion
        EventChannel.subscribe(self, subject)                   # Aufruf der EventChannel Methode

    def unsubscribe(self):                                      # Unsubscribe löst socket_close aus
        self.socket_close()

# Handler für das 'Lauschen' am Socket
# Es wird zu Beginn die Queue Size geprüft
# sollte noch Platz in der Queue sein wird analog dem 'normalen' Subscriber ein Event eingelesen und in die Queue gelegt
# ist die Queue allerdings voll, wird sofort der Callback ausgelöst
# zusätzlich wird das nächste Event direkt wieder in die Queue gepackt

# Vom Callback aus muss also dafür gesorgt werden, die Daten aus der Queue zu entnehmen und sie damit zu leeren(!)

    def handle_read(self):
        if not self.__myQueue.full():
            head = self.socket.recv(13)
            self.myEvent = event.Event(head[1:8], "1,1,1,1,1")
            time.sleep(0.1)
            self.myEvent.content = self.socket.recv(int(ord(head[12])))
            self.__myQueue.put(self.myEvent)
            print "Event auf: " +str(hex(ord(str(self.subject[0])))) +" empfangen und in Queue eingefuegt."
            print "Neue Queue Size: " + str(self.__myQueue.qsize())
            print "Callback wird ausgeloest, sobald Queue voll"
        else:
            print "Queue ist voll, Callback wird ausgeloest:"
            self.callback(self)
            print "Queue wurde geleert"
            head = self.socket.recv(13)
            self.myEvent = event.Event(head[1:8], "1,1,1,1,1")
            time.sleep(0.1)
            self.myEvent.content = self.socket.recv(int(ord(head[12])))
            self.__myQueue.put(self.myEvent)
            print "Event auf: " +str(hex(ord(str(self.subject[0])))) +" empfangen und in Queue eingefuegt."
            print "Neue Queue Size: " + str(self.__myQueue.qsize())
            print "Callback wird ausgeloest, sobald Queue voll"
            
# diese Methoder ermöglicht es, von aussen (zB über den Callback) auf die Daten der Queue zuzugreifen
# ein Aufruf dieser Methode entnimmt genau ein Objekt aus der Queue
# daher sollte im Callback zb eine Schleife zum Entleeren der Queue verwendet werden

    def getQueueData(self):
        if not self.__myQueue.empty():
            return self.__myQueue.get()
        else:
            print "Queue ist leer."

# Zusätzliches Überschreiben der nicht verwendeten Methoden
# damit sie von einem Publisher Objekt aus nicht verwendet werden können

    def handle_write(self):
        pass

    def announce(self):
        pass

    def publish(self):
        pass
        
    def unannounce(self):
        pass
