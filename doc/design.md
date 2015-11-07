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

## FTDI cable wiring ##

| Color  | Use |
| ------ | --- |
| black  | GND |
| brown  | CTS |
| red    | VCC |
| orange | TXD |
| yellow | RXD |
| green  | RTS |

VCC is at +5V. It must not be used.