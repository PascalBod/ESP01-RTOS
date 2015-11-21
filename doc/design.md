# Hardware and software set up #

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

I tried the Espressif way, but without real success. It relies on a VirtualBox Linux guest, and a Python download tool. Setting up the virtual machine was OK. But I was not able to use the download tool. So, let's switch to the open way.

Steps described below are for *OS X El Capitan* (10.11.1).

## Requirements and dependencies ##

* install *brew*:

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
* install dependencies:

```
brew tap homebrew/dupes
brew install binutils coreutils automake wget gawk libtool gperf gnu-sed --with-default-names grep
export PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
```
* create a virtual disk with case-sensitive file system:

```
mkdir -p ~/DevTools/espopensdk
sudo hdiutil create ~/DevTools/espopensdk/case-sensitive.dmg -volname "case-sensitive" -size 10g -fs "Case-sensitive HFS+"
sudo hdiutil mount ~/DevTools/espopensdk/case-sensitive.dmg
cd /Volumes/case-sensitive
```

## Building ##

* clone repository:

```
git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
```
* build the project. We choose the separated (non-standalone) SDK:

```
cd esp-open-sdk
make STANDALONE=n
```
Make process stops with many error messages similar to this one:

```
/Library/Developer/CommandLineTools/usr/bin/../include/c++/v1/iterator:413:13: error: unknown type name 'ptrdiff_t'
```
A solution is given [here](https://github.com/pfalcon/esp-open-sdk/issues/45):

```
sed -i.bak '/__need_size_t/d' ./crosstool-NG/.build/src/gmp-5.1.3/gmp-h.in
make STANDALONE=n
```
Once the SDK is installed, following information is displayed:

```
Xtensa toolchain is built, to use it:

export PATH=/Volumes/case-sensitive/esp-open-sdk/xtensa-lx106-elf/bin:$PATH

Espressif ESP8266 SDK is installed. Toolchain contains only Open Source components
To link external proprietary libraries add:

xtensa-lx106-elf-gcc -I/Volumes/case-sensitive/esp-open-sdk/sdk/include -L/Volumes/case-sensitive/esp-open-sdk/sdk/lib
```
* add following lines to `~/.bash_profile`:

```
export PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
export PATH=/Volumes/case-sensitive/esp-open-sdk/xtensa-lx106-elf/bin:$PATH
```

# Reference material #

* ESP8266
  * [Getting Started with ESP8266](http://www.esp8266.com/wiki/doku.php?id=getting-started-with-the-esp8266)
  * [Espressif documentation](http://bbs.espressif.com/viewtopic.php?f=67&t=225)
  * [ESP8266 KiCAD files](https://github.com/jdunmire/kicad-ESP8266)
* Misc.
  * [brew](http://brew.sh/)
  * [FTDI USB TTL Serial Cables](http://www.ftdichip.com/Products/Cables/USBTTLSerial.htm)
  * [CoolTerm: free terminal emulator for OS X](http://freeware.the-meiers.org/)