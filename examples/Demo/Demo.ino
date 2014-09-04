#include "Arduino.h"

#include "stft.h"

#include "font1.h"
#include "numbers1.h"

/*
        Wiring of ILI9341 display:

        Display         Arduino
       ======================================================================================
        SDO/MISO        12 - fixed
        LED             +3.3V or PWM (Attention! Needs more than the max 40mA an Arduino pin can deliver!)
        SCK             13 - fixed
        SDI/MOSI        11 - fixed
        D/C             9 - fourth parameter below
        RESET           2 - third parameter below
        CS              10 - second parameter below
        GND             GND
        VCC             +3.3V


        Wiring of ST7735 display:
        
        Display         Arduino
       ======================================================================================
        GND             GND
        VCC             +3.3V
        RESET           2 - third parameter below
        AO              9 - fourth parameter below
        SDA             11 - fixed
        SCK             13 - fixed
        CS              10 - second parameter below
        LED+            +3.3V or PWM
        LED-            GND
*/

//STFT tft(ST7735, 10, 2, 9);			// ST7735 display
STFT tft(ILI9341, 10, 2, 9);			// ILI9341 display

void setup()
{
	tft.init();
	tft.clrScr();
}

void grid()
{
	uint16_t	i;

	tft.setColor(RGB(64, 64, 64));

	for (i=0; i<tft.width; i+=10)
		tft.drawRect(i, 0, i, tft.height-1);
	for (i=0; i<tft.height; i+=10)
		tft.drawRect(0, i, tft.width-1, i);
}

void demo1()
{
	uint16_t	value;

	tft.startRaw(0, 0, tft.width-1, tft.height-1);
	for (uint16_t y=0; y<tft.height; y++)
		for (uint16_t x=0; x<tft.width; x++)
		{
			value = RGB(x, y*51/64, 0);

			SPIFastWrite(value >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
			SPIFastWrite(value & 0xFF);
		}
	tft.endRaw();
}

void demo2(uint8_t mode)
{
	uint8_t		x0, x1, x;
	uint16_t	y0, y1, y;

	for (uint8_t j=0; j<255; j++)
	{
		x0 = rand() % tft.width;
		x1 = rand() % tft.width;
		y0 = rand() % tft.height;
		y1 = rand() % tft.height;

		if (x0 > x1)
		{
			x = x0; x0 = x1; x1 = x;
		}
		if (y0 > y1)
		{
			y = y0; y0 = y1; y1 = y;
		}

		tft.setColor( (rand() & 0xFF) | ((rand() & 0xFF)<<8) );
		if (mode == 0)
			tft.drawRect(x0, y0, x1, y1);
		else
			tft.fillRect(x0, y0, x1, y1);
	}
}

void demo3()
{
	char 		abc[] = "abcdefghijklmnqrstuvwxyz";
	char 		abcs[] = "abc";
	char		time[] = "12:34";
	uint16_t	y = 1;

	grid();

	tft.setTextColor(RGB(255, 255, 0));
	tft.setTextBackColor(RGB(255, 0, 0));

	// Small Font
	SETFONT(tft, Arlrdbd12);

	// All rotated 0°
	tft.drawStringLimit(30, y, abcs, ROTATE0, tft.width-60, ALIGN_LEFT);
	y+= tft.fontSize + 1;
	tft.drawStringLimit(30, y, abcs, ROTATE0, tft.width-60, ALIGN_CENTER);
	y+= tft.fontSize + 1;
	tft.drawStringLimit(30, y, abcs, ROTATE0, tft.width-60, ALIGN_RIGHT);
	y+= tft.fontSize + 1;

	tft.drawStringLimit(30, y, abc, ROTATE0, tft.width-60, ALIGN_LEFT);
	y+= tft.fontSize + 1;
    
	// All rotated 90°
	tft.drawStringLimit(tft.width-2, 1, abc, ROTATE90, tft.height-2, ALIGN_CENTER);

	// All rotated 180°
	tft.drawStringLimit(tft.width-31, tft.height-2, abcs, ROTATE180, tft.width-60, ALIGN_CENTER);

	// All rotated 270°
	tft.drawStringLimit(1, tft.height-2, abc, ROTATE270, tft.height-2, ALIGN_CENTER);
        
	// Big Font
	SETFONT(tft, Arlrdbd56);

	// All rotated 0°
	tft.drawStringLimit(22, y, time, ROTATE0, tft.width-44, ALIGN_CENTER);

        if (tft.width>=240)
        {
        	// All rotated 180°
	        tft.drawStringLimit(210, 280, time, ROTATE180, 180, ALIGN_CENTER);
        }
}

void next()
{
	delay(1000);
	tft.clrScr();
}

void loop()
{
	demo1();
	next();
	demo2(0);
	next();
	demo2(1);
	next();
	demo3();

	delay(4000);
	next();
}
