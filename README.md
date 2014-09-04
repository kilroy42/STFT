STFT
====

Fast Arduino library for SPI displays Ili9341 and ST7735

Features
--------

* Low code usage (configurable in config.h to remove unneeded code parts)
* Low memory usage
* hand optimized SPI code for maximum speed
* Fonts:
  * Font compression to save flash space (4bit packed and RLE encoding)
  * 0, 90, 180, 270 degree rotating (only 0 and 180 bit for RLE fonts)
  * antialiased fonts
* Ability to drive multiple displays (same type and different types): each additional display only uses one extra pin
* example sketches to show performance and features

Configuration
-------------

### Wiring of Ili9341 display

`Display         Arduino`
`======================================================================================`
`SDO/MISO        12 - fixed`
`LED             +3.3V or PWM (Attention! Needs more than the max 40mA an Arduino pin can deliver!)`
`SCK             13 - fixed`
`SDI/MOSI        11 - fixed`
`D/C             9 - fourth parameter`
`RESET           2 - third parameter`
`CS              10 - second parameter`
`GND             GND`
`VCC             +3.3V`

### Wiring of ST7735 display

`Display         Arduino`
`======================================================================================`
`GND             GND`
`VCC             +3.3V`
`RESET           2 - third parameter`
`AO              9 - fourth parameter`
`SDA             11 - fixed`
`SCK             13 - fixed`
`CS              10 - second parameter`
`LED+            +3.3V or PWM`
`LED-            GND`

Font handling
-------------

Fonts are generated with generateFont.py (needs cairo and pango for Python).

Usage:

`generateFont.py [-h] [-s SIZE] [-a {packed4,rle,unpacked}]`
`                [-f FIRSTCHAR] [-l LASTCHAR] [-e EXTRACHARS]`
`                fontname`

Example:

`python generateFont.py arlrdbd.ttf -s 12 -a packed4 -e äöüÄÖÜß > font1.h`

Will generate a `font1.h` file with a 12 point font and chars 33-127 and extra chars äöüÄÖÜß (these will be mapped 128..134)
