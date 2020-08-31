# water-meter
Photoelectronic Sensor connected to a WeMos D1 Mini for monitoring the consumption data of an analog water meter and store it in InfluxDB.

As the usual water meters used by the water providers in germany do not offer any impuls / digital interface, it was quite impossible to monitor the water usage for the smarthome.
To overcome this limitation, the below description is basically my implementation of the descriptions and inspirations I´ve taken from [HomeMatic Forum](https://homematic-forum.de/forum/viewtopic.php?t=35461) work of "klassisch".

The sensor was placed over the analog 1l counter of the water meter and is creating an impulse (voltage drop) each time the needle passes by. This is counted via interrupt of an input pin of an WeMos D1 Mini ESP8266 board. The WeMos is connected via 2.4GHz Wifi. Every second the counter value is checked and if needed written to InfluxDB.

Important note is to not use the USB connectivity for power supply, but really use external 5V supply. With the USB connection some impulse signals might get lost.


## Part List
* Arduino (WeMos) D1 Mini        6€
* Buck Converter (15V to 5V)     2€
* Junction box                   2€
* Wago Connectors (3x)           4€
* Power Supply 15V               2€
* Barrel Connector               1€
* Resistor 10k Ω                 2€
* some cables                    5€
* Sunx EX-26A                  >25€

Total                          ~50€


## Connection Diagram

<img src="https://raw.githubusercontent.com/Froschie/water-meter/master/wemos_water_meter.png" width="600" height="360" alt="Connection Diagram">


## Physical WeMos and Sensor Installation

<img src="https://raw.githubusercontent.com/Froschie/water-meter/master/wemos_cabling.png" width="500" height="500" alt="WeMos Caling"><img src="https://raw.githubusercontent.com/Froschie/water-meter/master/sensor_mounted.png" width="500" height="331" alt="Sensor Mounted">


## Arduino IDE Configuration

In Arduino IDE please make sure that:
* "ESP8266" support has been installed via "Board Manager" (using the Board Manager URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`)
* "ESP8266 Influxdb" support has been installed via "Library Manager"

Example of Board Settings in Arduino IDE (please select correct COM port):

![Arduino IDE Board Config](https://raw.githubusercontent.com/Froschie/water-meter/master/arduino_ide_boardconfig.png)
