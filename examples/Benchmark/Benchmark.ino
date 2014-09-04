#include "Arduino.h"

#include <stft.h>

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
        Serial.begin(9600);

        tft.init();
        tft.setTextColor(RGB(255, 255, 0));
        tft.setTextBackColor(RGB(255, 0, 0));
}

int32_t start, end;

inline void begin(char* txt)
{
        Serial.println(txt);        
        start = millis();
}

inline void show()
{
        end = millis();
        Serial.print(100, DEC);
        Serial.print(" lines in ");
        Serial.print(end-start, DEC);
        Serial.println("msecs");
}

void bench1()
{
        uint8_t i, j;
        char txt[] = "abcdefghijklmnopqrstuvwxyz";
        
        // Small Font
        SETFONT(tft, Arlrdbd12);
        
        begin("Draw text 100 times (rotated 0 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(0, i, txt, ROTATE0, tft.width, ALIGN_LEFT);        
        show();

        begin("Draw text 100 times (rotated 90 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(tft.width-1-i, 0, txt, ROTATE90, tft.height, ALIGN_LEFT);
        show();
        
        begin("Draw text 100 times (rotated 180 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(tft.width-1, tft.height-1-i, txt, ROTATE180, tft.width, ALIGN_LEFT);
        show();

        begin("Draw text 100 times (rotated 270 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(i, tft.height-1, txt, ROTATE270, tft.height, ALIGN_LEFT);
        show();
}

void bench2()
{
        uint8_t i, j;
        char txt[] = "12:34:56";
        
        // Big Font
        SETFONT(tft, Arlrdbd56);
        
        begin("Draw big text 100 times (rotated 0 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(0, i, txt, ROTATE0, tft.width, ALIGN_CENTER);
        show();

        begin("Draw big text 100 times (rotated 180 degrees)");
        for (i = 0; i<100; i++)
                tft.drawStringLimit(tft.width-1, tft.height-1-i, txt, ROTATE180, tft.width, ALIGN_CENTER);
        show();
}

void bench3()
{
        begin("draw some rectangles");
        for (uint8_t j=0; j<5; j++)
        {
                tft.setColor(RGB(255, 0, 0));
                for (uint8_t i=0; i<(tft.width>>1)-1; i++)
                        tft.drawRect(i, i, tft.width-1-i, tft.height-1-i);
                tft.setColor(RGB(0, 255, 0));
                for (uint8_t i=0; i<(tft.width>>1)-1; i++)
                        tft.drawRect(i, i, tft.width-1-i, tft.height-1-i);
                tft.setColor(RGB(0, 0, 255));
                for (uint8_t i=0; i<(tft.width>>1)-1; i++)
                        tft.drawRect(i, i, tft.width-1-i, tft.height-1-i);
        }
        show();
}

void loop()
{
        tft.clrScr();
        bench1();
        
        tft.clrScr();
        bench2();
        
        tft.clrScr();
        bench3();
}


