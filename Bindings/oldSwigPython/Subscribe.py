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

print "Erzeuge Event und trage Subject fuer Subscribtion ein"
event=famouso.event_t()
event.subject=0xf100000000000000L
print "Subject:",event.subject

print "Aboniere das Subject"
S=famouso.SubscriberEC(event.subject)
S.subscribe()
print "Poll das Event"
z=1
while done:
	if S.poll(event):
		print "Len:",event.len,"Daten:",event.data[0:event.len]
		continue
	time.sleep(1)

#S.unsubscribe()
