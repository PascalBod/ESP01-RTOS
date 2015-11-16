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

We will conform to the Espressif way. It refers to [files stored on Baidu](http://pan.baidu.com/s/1gd3T14n). Downloading from there can be quite slow. Files [can be found on Google Drive](https://drive.google.com/folderview?id=0B5bwBE9A5dBXaExvdDExVFNrUXM&usp=sharing) as well.

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

Reference document seems to be [2A-ESP8266__IOT_SDK_User_Manual__EN_v1.4.pdf](http://bbs.espressif.com/viewtopic.php?f=51&t=1024).

* go into ``app`` directory and run ``./gen_misc.sh``. Enter following parameters:
  * boot version: 1 (boot V1.2+)
  * bin: 1 (user1.bin)
  * SPI speed: to be checked. Let's keep default value for now
  * SPI mode: same
  * SPI size: to be checked. Let's choose 2 (512KB + 512KB) for now

Displayed results:

```
!!!
-152710429
152710428
Support boot_v1.2 and +
Generate user1.1024.new.2.bin successully in folder bin/upgrade.
boot.bin------------>0x00000
user1.1024.new.2.bin--->0x01000
!!!
make: warning:  Clock skew detected.  Your build may be incomplete.
```

* perform a ``make clean`` and run same process again, for ``user2.bin``

Displayed results:

```
!!!
303209664
303209665
Support boot_v1.2 and +
Generate user2.1024.new.2.bin successully in folder bin/upgrade.
boot.bin------------>0x00000
user2.1024.new.2.bin--->0x81000
!!!
make: warning:  Clock skew detected.  Your build may be incomplete.
```
### Installation of the flash download tool ###

[This page](http://bbs.espressif.com/viewtopic.php?f=57&t=433) gives indications about how to install the flash download tool. It requires Python 2.6 or 2.7, and a few specific libraries. 

To install on OS X:

* Python 2.7.10 is already installed. But the installation of [wxPython](http://www.wxpython.org/download.php) required library fails. According to [wxPython forum](http://wxpython-users.1045709.n5.nabble.com/Installation-fails-on-OSX-despite-change-to-security-settings-td5724774.html), one solution is to install and use Anaconda Python.
* download [Anaconda Graphical Installer for Python 2.7](https://www.continuum.io/downloads)
* open a new terminal and enter commands

```
conda install wxpython
conda install pyserial
```

# Reference material #

* ESP8266
  * [Getting Started with ESP8266](http://www.esp8266.com/wiki/doku.php?id=getting-started-with-the-esp8266)
  * [Espressif documentation](http://bbs.espressif.com/viewtopic.php?f=67&t=225)
  * [ESP8266 KiCAD files](https://github.com/jdunmire/kicad-ESP8266)
* Misc.
  * [FTDI USB TTL Serial Cables](http://www.ftdichip.com/Products/Cables/USBTTLSerial.htm)
  * [CoolTerm: free terminal emulator for OS X](http://freeware.the-meiers.org/)