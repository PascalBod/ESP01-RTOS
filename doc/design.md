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

First step is to install the development environment. There are several ways to do so:

* the [Espressif way](http://bbs.espressif.com/viewtopic.php?f=67&t=821)
* the [Espressif Community way](https://github.com/esp8266/esp8266-wiki/wiki/Toolchain)
* the [open way](https://github.com/pfalcon/esp-open-sdk)

We will conform to the Espressif way.

It refers to [files stored on Baidu](http://pan.baidu.com/s/1gd3T14n). Downloading from there can be quite slow. Files [can be found on Google Drive](https://drive.google.com/folderview?id=0B5bwBE9A5dBXaExvdDExVFNrUXM&usp=sharing) as well.

### Virtual image installation and configuration ###

* download and install [VirtualBox](https://www.virtualbox.org/wiki/Downloads). Advised version is 4.3.12, but is not supported by the version of OS X I use. Consequently, I install version 5.0.10
* download virtual image, which contains the toolchain
* from VirtualBox, import the virtual image
* declare shared folder
* download [IoT Non-OS SDK](http://bbs.espressif.com/viewtopic.php?f=46&t=1124)
* copy IoT SDK to shared folder, and copy IoT Demo source code to the `app` folder
* start the virtual machine
* thanks to **Preferences / Keyboard Input Methods**, add support for AZERTY keyboard (for a MacBook, beware: some keys are not at the usual place, e.g. `-` or `_`)
* upgrade to Guest Additions 5.0.10:
  * click on **Devices / Insert Guest Additions CD Image...** and mount it. For me, it was mounted at ``/media/esp8266/VBOXADDITIONS_5.0.10_104061``
  * open a terminal, go into this directory, and run command

```
sudo ./VBoxLinuxAdditions.run
```
* reboot the virtual machine: ``sudo reboot``
* read UID and GID for *esp8266* user from ``/etc/passwd``. In my case: ``1000:1000``.
* mount shared folder:

```
sudo mount -o gid=1000,uid=1000 -t vboxsf share /mnt/Share
```

### Build of IoT Demo ###

* go into ``app`` directory and run ``make`` command. It should display following results:

```
!!!
No boot needed.  
Generate eagle.flash.bin and eagle.irom0text.bin successully in folder bin.  
eagle.flash.bin-------->0x00000  
eagle.irom0text.bin---->0x40000  
!!!
```
* run ``./gen_misc.sh``. Several questions have to be answered, regarding boot version, SPI speed, etc. For first run, I used default answers. In the end, displayed message is the same than above, with a warning about *clock skew*.

# Reference material #

* ESP8266
  * [Getting Started with ESP8266](http://www.esp8266.com/wiki/doku.php?id=getting-started-with-the-esp8266)
  * [Espressif documentation](http://bbs.espressif.com/viewtopic.php?f=67&t=225)
  * [ESP8266 KiCAD files](https://github.com/jdunmire/kicad-ESP8266)
* Misc.
  * [FTDI USB TTL Serial Cables](http://www.ftdichip.com/Products/Cables/USBTTLSerial.htm)
  * [CoolTerm: free terminal emulator for OS X](http://freeware.the-meiers.org/)