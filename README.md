STFT
====

Fast Arduino library for SPI displays Ili9341 and ST7735

Features
--------

* Low code usage (configurable in config.h to remove unneeded code parts)
  * Minimum sketch size for Ili9341: 2100 bytes
  * Minimum sketch size for ST7735: 2124 bytes
* Low memory usage
* hand optimized SPI code for maximum speed
* Fonts:
  * Font compression to save flash space (4bit packed and RLE encoding)
  * 0, 90, 180, 270 degree rotating (only 0 and 180 bit for RLE fonts)
  * antialiased fonts
  * auto cropping and expanding
  * align left and right or center
* Ability to drive multiple displays (same type and different types): each additional display only uses one extra pin
* example sketches to show performance and features
* hardware scrolling window support for Ili9341 

Compatibility
-------------

* Has been tested with ATMega328P @ 16 MHz
* If the chip is not running at a different frequence, disable FASTSPI

Configuration
-------------

### Wiring of Ili9341 display

```
Display         Arduino
==================================================================================================
SDO/MISO        12 - fixed
LED             +3.3V or PWM (Attention! Needs more than the max 40mA an Arduino pin can deliver!)
SCK             13 - fixed
SDI/MOSI        11 - fixed
D/C             9 - fourth parameter
RESET           2 - third parameter
CS              10 - second parameter
GND             GND
VCC             +3.3V
```

### Wiring of ST7735 display

```
Display         Arduino
=====================================
GND             GND
VCC             +3.3V
RESET           2 - third parameter
AO              9 - fourth parameter
SDA             11 - fixed
SCK             13 - fixed
CS              10 - second parameter
LED+            +3.3V or PWM
LED-            GND
```

### Configuration in config.h

```#define FASTSPI```

Pro:

* Fast

Contra:

* Uses a little bit more code space (can be as high as 1kb in big projects)
* Only works with 16MHz clock
* Only tested on ATMega328p

```
#define MODEL_ST7735
#define MODEL_ILI9341
```

Comment out all models you don't use

```
#define TEXT_ROTATE_0
#define TEXT_ROTATE_90
#define TEXT_ROTATE_180
#define TEXT_ROTATE_270
#define TEXT_ROTATE_0_RLE
#define TEXT_ROTATE_180_RLE
```

Comment out all rotations you don't need


Font handling
-------------

Fonts are generated with generateFont.py (needs cairo and pango for Python).

Usage:

```
generateFont.py [-h] [-s SIZE] [-a {packed4,rle,unpacked}]
                [-f FIRSTCHAR] [-l LASTCHAR] [-e EXTRACHARS]
                fontname
```

Example:

`python generateFont.py arlrdbd.ttf -s 12 -a packed4 -e äöüÄÖÜß > font1.h`

Will generate a `font1.h` file with a 12 point font and chars 33-127 and extra chars äöüÄÖÜß (these will be mapped 128..134)

#### Packed4 fonts

Two pixels are compressed into one byte, usually the best compression for smaller fonts (up to 16-18 points size).

Rotation can be performed 0, 90, 180, 270 degrees

#### RLE packed fonts

A simple RLE compression is applied. Fonts need to be bigger (18-... points) to be effective

Rotation can only be performed 0 and 180 degrees
