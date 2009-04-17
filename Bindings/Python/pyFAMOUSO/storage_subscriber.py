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
            self.myEvent = event.Event(head[1:9], "1,1,1,1,1")
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
            self.myEvent = event.Event(head[1:9], "1,1,1,1,1")
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
