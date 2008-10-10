#!/usr/bin/python
# Importiere die FAMOUSO-Bindings in Python
from generated import famouso
# some additional imports needed for that demo
import time, signal

done=True

def sigint_handler(signum, frame):
        global done
        done=False

signal.signal(signal.SIGINT, sigint_handler)


print "Erzeuge ein Event von einer C-Struktur und trage alles wesentliche ein"
P=famouso.PublisherEC(0xf100000000000000L)

event=famouso.event_t()
event.len=16
event.data="QNode000 Michael"
event.subject=0xf100000000000000L
print "Subject:",event.subject,"Laenge:",event.len,"Daten: ",event.data

print "Kuendige das Subject an"
P.announce()

print "Publiziere das Event"
while done:
	P.publish(event)
	time.sleep(0.1)
