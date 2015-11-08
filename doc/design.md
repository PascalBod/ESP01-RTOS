# Hardware set up #

## Prerequisites ##

Following elements are required:
* one ESP-01 board
* one FTDI TTL-232R-3V3-WE cable
* one breadboard
* one 3.3V power supply

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

| Color  | Use | Type   |
| ------ | --- | ------ |
| black  | GND |        |
| brown  | CTS | Input  |
| red    | VCC |        |
| orange | TXD | Output |
| yellow | RXD | Input  |
| green  | RTS | Output |

VCC is at +5V. It must not be used.

## First connection ##

Connect the FTDI USB cable to a computer. Using a terminal emulator,
connect to the serial-over-USB port. Configuration:
* 9600 b/s
* 8 data bits
* no parity
* 1 stop bit
* no flow control

Send `AT+GMR` command. Reply is `0018000902-AI03`.

# Reference material #

* [Getting Started with ESP8266](http://www.esp8266.com/wiki/doku.php?id=getting-started-with-the-esp8266)
* [FTDI USB TTL Serial Cables](http://www.ftdichip.com/Products/Cables/USBTTLSerial.htm)
* [ESP8266 KiCAD files](https://github.com/jdunmire/kicad-ESP8266)