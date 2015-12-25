# SoftAP example #

Before going further with Espressif RTOS, let's first test some Wi-Fi functions we'll use later. In this example, we configure the ESP8266 as an access point.

## Access point ##

Espressif forum provides [source code excerpt](http://bbs.espressif.com/viewtopic.php?f=31&t=227) for the non-OS SDK. That's easy to re-use it for an RTOS version.

Resulting source code implements a callback that intercepts Wi-Fi events that can be received:

* station connection
* station disconnection
* probe request receipt

[Next: inter-task communication](designInterTask.md)

[Home](design.md)

