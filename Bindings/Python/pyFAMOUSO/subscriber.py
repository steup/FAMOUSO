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
# Subscriber Klasse

#import famouso Modul
from famouso import *

class SubscriberEventChannel(EventChannel):     # Abgeleitet vom EventChannel
    read_buffer = ''				##< buffer to store incoming data from ech

    def __init__(self,  subject):               # Konstruktor benötigt subject
        EventChannel.__init__(self, subject)    # Konstruktor vom Eventchannel aufrufen
        self.callback = 1                       # Hinzufügen eines Callback Attributs

    def subscribe(self, subject, callback_func):    # Subscribe Methode erfordert Subject und Callback
        self.callback = callback_func               # Übergabe der Callbackfunktion
        EventChannel.subscribe(self, subject)       # Aufruf der EventChannel Methode

    def unsubscribe(self):      # Unsubscribe löst socket_close aus
        self.socket_close()

# Handler für das 'Lauschen' am Socket
# Es werden zuerst 13 Byte gelesen
# d.h. von einem Event wird der Op Code, das Subject und die Size ermittelt
# danach werden entsprechend der erhaltenen Size noch weitere Byte gelesen
# diese bilden dann den Content des Events

    def handle_read(self):
	self.read_buffer += self.socket.recv(8192)		# try to read as much as possible
	while len(self.read_buffer) > 13:			# header = 13 bytes
		subject = self.read_buffer[1:9]
		length = struct.unpack('!I', self.read_buffer[9:13])[0]
		if (len(self.read_buffer) < 13 + length):		# must wait for incoming data
			return
		content = self.read_buffer[13:13 + length]
		self.callback(event.Event(subject, content))	# report event to callback
		self.read_buffer = self.read_buffer[13 + length:]	# shift data buffer

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
