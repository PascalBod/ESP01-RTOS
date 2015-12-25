# How to use *Eclipse* #

## *Eclipse* installation ##

Download and install [Eclipse IDE for C/C++ Developers](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/mars1) (*Mars1* version).

Start *Eclipse*, with workspace set to `~/Dev/workspace`.

## RTOS project creation ##

Let's name our first RTOS Eclipse project `rtos1`. Create associated directories: `~/Dev/ESP-01/code/rtos1` and `~/Dev/ESP-01/bin/rtos1`.

Copy contents of `~DevTools/Espressif/ESP8266_RTOS_SDK/examples/project_template` to `rtos1`. Then, import it as a project:

* **File / Import... / C/C++ / Existing Code as Makefile Project**
* project name: `rtos1`
* existing code location: `~/Dev/ESP-01/code/rtos1`
* languages: `C`
* toolchain for indexer settings: `<none>`

Modify project properties:

* for **C/C++ General / Paths and Symbols / GNU C**, add following include paths:

```
${HOME}/DevTools/Espressif/ESP8266_RTOS_SDK/include
${HOME}/DevTools/Espressif/ESP8266_RTOS_SDK/include/espressif
```
* for **C/C++ Build**, uncheck **Use default build command**, and set build command to `make BOOT=new APP=0 SPI_SPEED=2 SPI_MODE=QIO SPI_SIZE_MAP=0`
* for **C/C++ Build / Environment**, add those variables:
  * `SDK_PATH`: `${HOME}/DevTools/Espressif/ESP8266_RTOS_SDK`
  * `BIN_PATH`: `${HOME}/Dev/ESP-01/bin/rtos1`
  * `PATH`: `/Volumes/case-sensitive/esp-open-sdk/xtensa-lx106-elf/bin:/usr/local/opt/gnu-sed/libexec/gnubin:${PATH}`

To (re)build the binaries: **Project / Build Project**

## UART file addition ##

From *firstTrial* project, copy files providing UART handling: source code, include file and makefiles. Don't forget to refresh *Eclipse* project afterwards, if the copy operation was performed outside of *Eclipse*.

For a reason I didn't take time yet to clarify, `true` symbol used in `uart.c` is not defined. Consequently, I added an include file named `miscPB.h`, that defines `true` and `false`, if they are not defined yet.

## First task ##

Add a new header file and associated source file, to declare and implement a task. Use the **C++ source template** when creating the implementation file, otherwise *Eclipse* won't be able to correctly handle include files (another point I have to clarify when I have some time...)

Create and start the task in the main initialization function. Our first task implements an infinite loop, which prints a message on the serial link every second.

Build, program, run. That's it.

[Next: SoftAP example](designSoftAP.md)

[Home](design.md)

## Reference documentation for going on ##

* [*RTOS SDK Programming Guide*](http://bbs.espressif.com/download/file.php?id=933)
* [*RTOS SDK API Reference*](http://bbs.espressif.com/download/file.php?id=935)
* [code samples](http://bbs.espressif.com/viewforum.php?f=31)
* [*FreeRTOS Quick Start Guide*](http://www.freertos.org/FreeRTOS-quick-start-guide.html)

[Back to home](design.md)