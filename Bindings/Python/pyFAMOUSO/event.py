# -*- coding: cp1252 -*-
# Event Klasse
# jedes Event besitzt ein feste Format:
# 1. Byte: Op - Code, er signalisiert, um welche Art von Event es sich handelt,
# der Op Code selbst, wird allerdings erst kurz vor dem Versenden in der Eventchannel.py angefügt
# 2. - 9. Byte: kennzeichnet das subject in Hex, also zB. 'f200000000000000'
# das subject wird dabei als String geführt
# 10. - 13. Byte: die Länge des contents, sie wird im Konstruktor explizit berechnet
# und muss nicht manuell angegeben werden
# ab dem 14. Byte kann somit der Inhalt, als String verpackt, folgen

class Event():

    def __init__(self, subject, content):   #Konstruktor der Eventklasse
        self.subject = subject
        self.size = len(content)
        self.content = content
