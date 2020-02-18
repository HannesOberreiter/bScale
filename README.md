# bScale for app.btree.at

Small Arduino project for a DIY beehive scale, with SIM900 for connection to webservice. This project is made to work with the webapplication **[app.btree.at](https://www.btree.at/app)**, an online database for beekeeper but you can use any webservice you want.

# New improved build from Piotr from Poland

Please see following markdown with description of a improved build version for the beescale:
[Piotr BeeScale](PiotrBeeScale.md).

Big chances are the separation of the battery unit from the UNO and SIM module and a higher voltage battery storage and solar charging in mind.

Thanks to [@PiotrBeeScale](https://github.com/PiotrBeeScale)


# Hardware

* UNO R3 MEGA ([AliExpress ca. € 2.70](http://de.aliexpress.com/item/high-quality-UNO-R3-MEGA328P-for-Arduino-UNO-R3-NO-USB-CABLE/32350449393.html))
* SIMCON SIM900 Shield (Important with power up pin!) ([Ebay China € 21.99](http://www.ebay.at/itm/SIMCOM-SIM900-Quad-band-GSM-GPRS-Shield-Development-Board-for-Arduino-/231857925209?hash=item35fbcf2c59))
* atTiny85 + Base ([Ebay ca. € 6,00 ](http://www.ebay.at/itm/3x-Atmel-ATTiny85-20PU-und-3x-DIL-Sockel-/251833156761?hash=item3aa26d0499:g:bYQAAOSwMpZUoV~n))
* N-Mosfet ([Ebay ca. < € 1,00](http://www.ebay.at/itm/5-BUZ11-Fairchild-MOSFET-Transistor-50V-30A-75W-0-04R-854773-/311597076091?hash=item488ca20e7b:g:O3AAAOSwLVZVyb-J))
* HX711 Sensor ([Ebay China < € 1,00](http://www.ebay.at/itm/5PCS-Weighing-Sensor-AD-Module-Dual-channel-24-bit-A-D-Conversion-HX711-Shieding-/272298400315?hash=item3f663fda3b:g:zKkAAOSwEjFXfP2J))
* < 200kg platform load cell ([AliExpress ca. € 17,00](http://de.aliexpress.com/item/1PCS-X-100KG-150KG-electronic-platform-scale-load-cell-pressure-balanced-cantilever-load-weight-sensor/1743007676.html))
* LPDP 5V ([Ebay ca. € 1,00](http://www.ebay.at/itm/LP2950CZ-5-0-LowDrop-Spannungsregler-5V-0-1A-TO92-/112028436978?hash=item1a156a49f2:g:itQAAOSwjXRXYUp4))

and battery case, batteries, resistors, jumper wire, scale plattform, Total cost was under € 100,00.

# Dirty Build:
![Fritzing](https://github.com/HannesOberreiter/bScale/blob/master/img/Sketch_Steckplatine.png?raw=true "Sketch Fritzing")


# How does / should it work?

UNO will collect data from sensors and send them to the shield. Uno forwards the data to SIM900, which connects to the webservice.
atTiny85 is a simple counter, it will send each 120 minutes a pulse to the N-Mosfet gate. The Mosfet will open circuit to activate Uno.

The UNO will tell the atTiny85 when the sending was completed. If the sending is not completed within 60 seconds, the atTiny will shutdown the UNO even it is not completed (No connection to internet etc.).

# Sensor Reading
For the Sensor reading you need to install the DHT Library and because a bug in Version 1.3 in this library you also need to install the [Adafruid Unified Sensor Library.](https://cloud.githubusercontent.com/assets/181073/21481640/5e14ebd8-cb6a-11e6-972e-c4b4efda6ad1.png)

# The webservice

You can send the data to the beekeeping application (not free) [app.btree.at](https://www.btree.at/app). There the data will be shown in tables and statistics. But you can change the webservice to any you want.

# Videos

**German:**
* [b.tree Open Source Bienenstockwaage kalibrieren ](https://www.youtube.com/watch?v=fo4yXhe6Qlo)
* [b.tree Open Source Bienenstockwaage atTiny85 programmieren ](https://youtu.be/5xHgviWt0F4)

# Images (more in img folder!)

**SIM900 Power On Pin**

On some SIM900 Modules the power up pin is not connected and you need to smolder the right position. Please see your specific Module description.
![SIM900 smoldering](https://github.com/HannesOberreiter/bScale/blob/master/img/sim900_smoldering.jpg)

If you use a single weight cell like me, the whole build is a little bit "shaky". To prevent to much bending on the iron bars I added this screws on all four sides with just milimeters of space.
![Screws](https://github.com/HannesOberreiter/bScale/blob/master/img/scale_image2.jpg)

# Special thanks to
Andi_H from bienenforum.com for the idea and lots of code pieces

bodge awesome HX711.h library https://github.com/bogde/HX711

Nathan Seidle SparkFun Electronics for calibrate code https://github.com/sparkfun/HX711-Load-Cell-Amplifier

My father for help with the electronics.

# Tests

* 2016-08-30 First field test the battery would handle 10 days. With each 30 minutes open the mosfet for 1 minute. Changed the code to only open the gate each 2 hours and the UNO will tell the atTiny when it is finsihed. Lets see how much of an improvment this will give us. Upload works fine, data also seems ok, Hannes.
* 2016-10-18 Second field test, with the new code (each 2 hours) did the battery last for 50 days. The battery still has power but not enough to open the GSM connection. I will try now instead of alkaline batteries lithium batteries. They should work better with the spike of energy needed for the GSM connection


# License
GNU GPLv3
