#!/usr/bin/python
# Importiere die FAMOUSO-Bindings in Python
import time, famouso, signal

done=True

def sigint_handler(signum, frame):
        global done
        done=False

signal.signal(signal.SIGINT, sigint_handler)


print "Erzeuge ein Event von einer C-Struktur und trage alles wesentliche ein"
event=famouso.famouso_event_t()
event.len=8
event.data="QNode000"
event.subject=0xf200000000000000L
print "Subject:",event.subject,"Laenge:",event.len,"Daten: ",event.data

print "Kuendige das Subject an"
famouso.famouso_announce(event.subject)

print "Publiziere das Event"
while done:
	famouso.famouso_publish(event)
	time.sleep(0.1)
