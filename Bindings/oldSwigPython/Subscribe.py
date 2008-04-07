#!/usr/bin/python
# Importiere die FAMOUSO-Bindings in Python
import time, famouso, signal

done=True

def sigint_handler(signum, frame):
	global done
	done=False

signal.signal(signal.SIGINT, sigint_handler)

print "Erzeuge Event und trage Subject fuer Subscribtion ein"
event=famouso.famouso_event_t()
event.subject=0xf200000000000000L
print "Subject:",event.subject

print "Aboniere das Subject"
famouso.famouso_subscribe(event.subject)

print "Poll das Event"
while done:
	if famouso.famouso_poll(event):
		print "Len:",event.len,"Daten:",event.data
		continue
	time.sleep(0.001)

famouso.famouso_unsubscribe(event.subject)
