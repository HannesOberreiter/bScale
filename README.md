# bScale

Small Arduino Project for a DIY beehive scale, with SIM900 commands to send to webservice.

# Hardware

* Arduino UNO
* SIM900 Shield
* atTiny85
* N-Mosfet
* HX711 Sensor
* > 200kg platform load cell

# How does / should it work?

UNO will collect data from sensors and send them to the shield. Uno send SIM900 commands. 
atTiny85 is a simple counter, it will send each 30 minutes a pulse to the N-Mosfet gate. The Mosfet will open circuit to Uno and activate it.

# The webservice

The made for app.btree.at a web beekeeping software. There the data will be shown in tables and statistics. But you can change the webservice to any you want.


