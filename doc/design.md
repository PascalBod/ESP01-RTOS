# Hardware set up #

## Prerequisites ##

Following elements are required:

* one ESP-01 board
* one FTDI TTL-232R-3V3-WE cable
* one 3.3V power supply
* two 1k resistors
* a way to connect above elements (breadboard, etc.)
* a computer with a terminal emulator. For OS X, I use *CoolTerm*

## ESP-01 wiring ##

Seen from the chip and antenna side, the board looks like:

```
     ----------------------------
     |             -----        |
TXD  | O O  GND    |   |        |
CH_PD| O O  GPIO2  ----- []     |
RST  | O O  GPIO0  ----         |
VCC  | O O  RXD    |  |         |
     |             ----         |
     ----------------------------
```

VCC is 3.3V. RXD/TXD are 3.3V TTL.

CH_PD must be pulled to 3.3V via a resistor (1k).

RST must be pulled to 3.3V via a resistor (1k).

TXD has to be connected to the RXD wire of the FTDI cable. RXD has to
be connected to the TXD wire of the FTDI cable.

## FTDI cable wiring ##

| Pin | Color  | Use | Type   |
| --- | ------ | --- | ------ |
| 1   | black  | GND |        |
| 2   | brown  | CTS | Input  |
| 3   | red    | VCC |        |
| 4   | orange | TXD | Output |
| 5   | yellow | RXD | Input  |
| 6   | green  | RTS | Output |

VCC is at +5V. It must not be used.

## Schematic ##

![](ESP-01-1.png)

## First connection ##

Connect the FTDI USB cable to a computer. Using a terminal emulator,
connect to the serial-over-USB port. Configuration:

* 9600 b/s
* 8 data bits
* no parity
* 1 stop bit
* no flow control

Send `AT+GMR` command. Reply is `0018000902-AI03`.

## First firmware download ##

### Overview ###

[This page](http://bbs.espressif.com/viewtopic.php?f=67&t=821) from Espressif web site explains how to install Espressif SDK in four steps.

First step refers to [files stored on Baidu](http://pan.baidu.com/s/1gd3T14n). Downloading from there can be quite slow but it worked for me. It seems that some of those files [can be found on Google Drive](http://www.esp8266.com/viewtopic.php?f=9&t=430) as well.

### Virtual image installation and configuration ###

* download and install [VirtualBox](https://www.virtualbox.org/wiki/Downloads). Advised version is 4.3.12, but is not supported by the version of OS X I use. Consequently, I install version 5.0.10.
* download [virtual image containing the SDK](http://pan.baidu.com/s/1gd3T14n). Password is `qudl`.
* from VirtualBox, import the virtual image
* declare shared folder
* copy IoT SDK source code to shared folder, and copy IoT Demo source code to the `app` folder
* start the virtual machine
* thanks to **Preferences / Keyboard Input Methods**, add support for AZERTY keyboard (for a MacBook, beware: some keys are not at the usual place, e.g. `-` or `_`)
* 

# Reference material #

* ESP8266
  * [Getting Started with ESP8266](http://www.esp8266.com/wiki/doku.php?id=getting-started-with-the-esp8266)
  * [Espressif documentation](http://bbs.espressif.com/viewtopic.php?f=67&t=225)
  * [ESP8266 KiCAD files](https://github.com/jdunmire/kicad-ESP8266)
* Misc.
  * [FTDI USB TTL Serial Cables](http://www.ftdichip.com/Products/Cables/USBTTLSerial.htm)
  * [CoolTerm: free terminal emulator for OS X](http://freeware.the-meiers.org/)