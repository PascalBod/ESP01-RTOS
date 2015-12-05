## *Eclipse* configuration ##

Download and install [Eclipse IDE for C/C++ Developers](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/mars1) (*Mars1* version).

Start *Eclipse*, with workspace set to `~/Dev/workspace`.

Import `firstTrial` project:

* **File / Import... / C/C++ / Existing Code as Makefile Project**
* project name: `firstTrial`
* existing code location: `~/Dev/ESP-01/code/firstTrial`
* languages: `C`
* toolchain for indexer settings: `<none>`

In project properties:

* for **C/C++ General / Paths and Symbols / GNU C**, add a new include path for `~/DevTools/Espressif/ESP8266_RTOS_SDK/include/espressif`
* for **C/C++ Build**, uncheck **Use default build command**, and set build command to `make BOOT=new APP=0 SPI_SPEED=2 SPI_MODE=QIO SPI_SIZE_MAP=0`
* for **C/C++ Build / Environment**, add those variables:
  * `SDK_PATH`: `/Users/<username>/DevTools/Espressif/ESP8266_RTOS_SDK`
  * `BIN_PATH`: `/Users/<username>/Dev/ESP-01/bin/firstTrial`
  * `PATH`: `/Volumes/case-sensitive/esp-open-sdk/xtensa-lx106-elf/bin:/usr/local/opt/gnu-sed/libexec/gnubin:${PATH}`