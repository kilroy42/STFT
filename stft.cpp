
#include "config.h"

#include "stft.h"

#ifdef MODEL_ILI9341
const uint8_t ili9341_s5p_initCode[] PROGMEM = {

    0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,                     0x42,
    0xCF, 0x00, 0XC1, 0X30,                                 0x42,
    0xE8, 0x85, 0x00, 0x78,                                 0x42,
    0xEA, 0x00, 0x00,                                       0x42,
    0xEA, 0x00, 0x00,                                       0x42,
    0xED, 0x64, 0x03, 0x12, 0x81,                           0x42,
    0xF7, 0x20,                                             0x42,

    // Power control VRH[5:0] = 4.60 V
    0xC0, 0x23,                                             0x42,

    // Power control SAP[2:0];BT[3:0] = adjust power to lowest possible
    0xC1, 0x10,                                             0x42,

    // VCM control 1 Contrast VCOMH = 4.250 VCOML = -1.500
    0xC5, 0x3e, 0x28,                                       0x42,

    // VCM control 2
    0xC7, 0x86,                                             0x42,

    // Memory Access Control - 0x08 = BGR color filter panel, 0x40 = Column Address Order 1
    0x36, 0x48,                                             0x42,

    // COLMOD: Pixel Format Set 16bits for pixel
    0x3A, 0x55,                                             0x42,

    // Frame Rate Control (In Normal Mode/Full Colors) - division ratio: 1 - 24 clocks
    0xB1, 0x00, 0x18,                                       0x42,

    // Display Function Control
    0xB6, 0x08, 0x82, 0x27,                                 0x42,

    // Exit sleep
    0x11,                                                   0x42,

    // EOF - not sent to controller
    0x42
};
#endif

#ifdef MODEL_ST7735
const uint8_t st7735_initCode[] PROGMEM = {

    // ST7735R Frame Rate
    0xB1, 0x01, 0x2C, 0x2D,                                 0x42,
    0xB2, 0x01, 0x2C, 0x2D,                                 0x42,
    0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,               0x42,

    // Column inversion 
    0xB4, 0x07,                                             0x42,
 
    // ST7735R Power Sequence
    0xC0, 0xA2, 0x02, 0x84,                                 0x42,
    0xC1, 0xC5,                                             0x42,
    0xC2, 0x0A, 0x00,                                       0x42,
    0xC3, 0x8A, 0x2A,                                       0x42,
    0xC4, 0x8A, 0xEE,                                       0x42,

    // VCOM
    0xC5, 0x0E,                                             0x42,
 
    // MX, MY, RGB mode 
    0x36, 0xC0,                                             0x42,            // fixed: C8 is RGB swapped, C0 is RGB correct

    // ST7735R Gamma Sequence
    0xe0, 0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,            0x42,
    0xe1, 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10,            0x42,
    0x2a, 0x00, 0x00, 0x00, 0x7f,                           0x42,
    0x2b, 0x00, 0x00, 0x00, 0x9f,                           0x42,

    // Enable test command
    0xF0, 0x01,                                             0x42,

    // Disable ram power save mode
    0xF6, 0x00,                                             0x42,

    // 65k mode
    0x3A, 0x05,                                             0x42,

    // Display on
    0x29,                                                   0x42,

    // EOF - not sent to controller
    0x42
};
#endif

STFT::STFT(uint8_t model, uint8_t CS, uint8_t RST, uint8_t SER)
{
	ap_cs   = CS;
	ap_rst  = RST;
	ap_ser  = SER;

	P_CS    = portOutputRegister(digitalPinToPort(CS));
	B_CS    = digitalPinToBitMask(CS);
	if (RST)
	{
		P_RST   = portOutputRegister(digitalPinToPort(RST));
		B_RST   = digitalPinToBitMask(RST);
	}
	else
	{
		P_RST = NULL;
	}
	P_RS    = portOutputRegister(digitalPinToPort(SER));
	B_RS    = digitalPinToBitMask(SER);

	this->model = model;
}
		
void STFT::LCD_Write_COM(char VL)
{
	cbi(P_RS, B_RS);
	SPITransfer(VL);
}

void STFT::LCD_Write_DATA(char VH,char VL)
{
	sbi(P_RS, B_RS);
	SPITransfer(VH);
	SPITransfer(VL);
}

void STFT::LCD_Write_DATA(char VL)
{
	sbi(P_RS, B_RS);
	SPITransfer(VL);
}

void STFT::LCD_Write_COM_DATA(uint8_t com, uint16_t data)
{
	LCD_Write_COM(com);
	LCD_Write_DATA(data>>8, data);
}

void STFT::init()
{
	uint8_t* cur;

	pinMode(ap_cs,  OUTPUT);
	if (P_RST)
		pinMode(ap_rst, OUTPUT);
	pinMode(ap_ser, OUTPUT);

	// SPI begin
	digitalWrite(SS, HIGH);
	pinMode(SS, OUTPUT);
	SPCR |= _BV(MSTR);
	SPCR |= _BV(SPE);
	pinMode(SCK, OUTPUT);
	pinMode(MOSI, OUTPUT);
	SPCR&=  ~0x03;
	SPSR|=   0x01;
	// SPI begin end

	if (P_RST)
	{
		// reset
		sbi(P_RST, B_RST);
		delay(5);
		cbi(P_RST, B_RST);
		delay(15);
		sbi(P_RST, B_RST);
		delay(15);
	}
	cbi(P_CS, B_CS);

	switch(model)
	{
#ifdef MODEL_ST7735
		case ST7735:
			width = 128;
			height = 160;

			LCD_Write_COM(0x11);
			delay(12);

			cur = (uint8_t *)st7735_initCode;
			break;
#endif
#ifdef MODEL_ILI9341
		case ILI9341:
			width = 240;
			height = 320;
			
			cur = (uint8_t *)ili9341_s5p_initCode;
			break;
#endif
	}

	while (pgm_read_byte(cur) != 0x42)
	{
		LCD_Write_COM(pgm_read_byte(cur++));
		while (pgm_read_byte(cur) != 0x42)
			LCD_Write_DATA(pgm_read_byte(cur++));
		cur++;
	}

	delay(120);

#ifdef MODEL_ILI9341
	switch(model)
	{
		case ILI9341:
			LCD_Write_COM(0x29);
			LCD_Write_COM(0x2c);
			break;
	}
#endif

	sbi (P_CS, B_CS);

	fcl = 0;
	fch = 0;

	fgColorText = 0xffff;
	bgColorText = 0x0000;
}

void STFT::clrScr()
{
  fillRect(0, 0, width-1, height-1, 0);
}

void STFT::startRaw()
{
	cbi(P_CS, B_CS);
}

void STFT::startRaw(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1)
{
	cbi(P_CS, B_CS);
	LCD_Write_COM_DATA(0x2a, x0);
	LCD_Write_DATA(x1>>8, x1);
	LCD_Write_COM_DATA(0x2b, y0);
	LCD_Write_DATA(y1>>8, y1);
	LCD_Write_COM(0x2c);
	sbi(P_RS, B_RS);
}

void STFT::endRaw()
{
	sbi(P_CS, B_CS);
}

void STFT::setColor(uint16_t rgb)
{
	fcl = rgb&0xFF;
	fch = rgb>>8;
}

void STFT::setFont(uint8_t* data, uint16_t* offsets, uint8_t size, uint8_t firstChar, uint8_t type)
{
	fontData = data;
	fontOffsets = offsets;
	fontSize = size;
	fontFirstChar = firstChar;
	fontType = type;
	updateTextColor();
}

void STFT::updateTextColor()
{
	int16_t r = (fgColorText & 0xf800) >> 11;
	int16_t g = (fgColorText & 0x07e0) >> 5;
	int16_t b = (fgColorText & 0x001f);

	int16_t r2 = (bgColorText & 0xf800) >> 11;
	int16_t g2 = (bgColorText & 0x07e0) >> 5;
	int16_t b2 = (bgColorText & 0x001f);

	uint16_t r3, g3, b3;

	for (uint16_t i=0; i<16; i++)
	{
		r3 = r2 + (r-r2) * i / 15;
		g3 = g2 + (g-g2) * i / 15;
		b3 = b2 + (b-b2) * i / 15;
		grey2word[i] = (r3 << 11) | (g3 << 5) | b3;
	}
}

void STFT::setTextColor(uint16_t color)
{
	fgColorText = color;
	updateTextColor();
}

void STFT::setTextBackColor(uint16_t color)
{
	bgColorText = color;
	updateTextColor();
}

void STFT::drawRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1)
{
	uint8_t h = fch;
	uint8_t l = fcl;
	int16_t size = x1 - x0 + 1;

	startRaw(x0, y0, x1, y0);
	while (size--)
	{
		SPIFastWrite(h);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
		SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 10 NOPs
	}
	SPIWait();
	endRaw();

	size = x1 - x0 + 1;
	startRaw(x0, y1, x1, y1);
	while (size--)
	{
		SPIFastWrite(h); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
		SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 10 NOPs
	}
	SPIWait();
	endRaw();

	size = y1 - y0 - 1; // two pixels less since we painted them in X direction already
	if (size>0)
	{
		startRaw(x0, y0+1, x0, y1-1);
		while (size--)
		{
			SPIFastWrite(h); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
			SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 10 NOPs
		}
		SPIWait();
		endRaw();
	}

	size = y1 - y0 - 1; // two pixels less since we painted them in X direction already
	if (size>0)
	{
		startRaw(x1, y0+1, x1, y1-1);
		while (size--)
		{
			SPIFastWrite(h); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
			SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 10 NOPs
		 }
		SPIWait();
		endRaw();
	}
}

void STFT::fillRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1)
{
	uint8_t h = fch;
	uint8_t l = fcl;
	uint32_t size = (uint32_t)(y1-y0+1) * (uint32_t)(x1-x0+1);

	startRaw(x0, y0, x1, y1);
	while (size--)
	{
		SPIFastWrite(h); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
		SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; // 6 NOPs
	}
	SPIWait();
	endRaw();
}

void STFT::fillRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1, uint16_t color)
{
	uint8_t h = color >> 8;
	uint8_t l = color & 0xFF;
	uint32_t size = (uint32_t)(y1-y0+1) * (uint32_t)(x1-x0+1);

//fprintf(stderr, "%i %i %i %i = %i\n", x0, y0, x1, y1, size);

	startRaw(x0, y0, x1, y1);
	while (size--)
	{
		SPIFastWrite(h); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
		SPIFastWrite(l); NOP; NOP; NOP; NOP; NOP; NOP; // 6 NOPs
	}
	SPIWait();
	endRaw();
}

uint8_t STFT::drawChar(uint8_t startx, uint16_t starty, uint8_t c, uint8_t rotate, uint16_t limitX)
{
	uint8_t		sizeX, sizeY, offsetY, data, x, flag;
	uint16_t	y, offset, wdata, i, j;

	uint8_t*	buf;

	if (!limitX)
		return 0;

	if (c == 32)
	{
		sizeX = fontSize / 6;
		sizeY = 0;
		offsetY = 0;
	}
	else
	{
		// get start of font data
		offset = pgm_read_word(fontOffsets + (c - fontFirstChar));

		buf = fontData + offset;

		// first 3 bytes are size (X+Y) and offsetY
		sizeX = pgm_read_byte(buf++);
		sizeY = pgm_read_byte(buf++);
		offsetY = pgm_read_byte(buf++);
	}

	if (limitX > sizeX)
		limitX = sizeX;

	switch(rotate + fontType)
	{
#ifdef TEXT_ROTATE_0
		// 0°
		case 0:
			// tell display we want to write a sizeX * sizeY area
			flag = 1;

			startRaw(startx, starty, startx+limitX-1, starty+fontSize-1);

			// paint empty area above char
			if (offsetY)
			{
				wdata = bgColorText;
				i = offsetY * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}
			if (limitX < sizeX)
			{
				uint8_t todo = limitX;
				uint8_t skip = sizeX - limitX;

				for (i=0; i<sizeX*sizeY; i++)
				{
					if (flag)
					{
						data = pgm_read_byte(buf++);
						wdata = grey2word[data>>4];
						flag = 0;
					}
					else
					{
						wdata = grey2word[data&15];
						flag = 1;
					}

					if (todo)
					{
						SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
						SPIFastWrite(wdata & 0xFF); // CHECK: maybe some NOPs here
						todo--;
					}
					else
					{
						skip--;

						if (skip == 0)
						{
							todo = limitX;
							skip = sizeX - limitX;
						}
					}
				}
			}
			else
			{
				for (i=0; i<sizeX*sizeY; i++)
				{
					if (flag)
					{
						data = pgm_read_byte(buf++);
						wdata = grey2word[data>>4];
						flag = 0;
					}
					else
					{
						wdata = grey2word[data&15];
						flag = 1;
					}
					SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF);
				}
				SPIWait();
			}
			
			// paint empty area below char
			if (offsetY + sizeY < fontSize)
			{
				wdata = bgColorText;
				i = (fontSize - sizeY - offsetY) * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			endRaw();
			break;
#endif
#ifdef TEXT_ROTATE_90
		// 90°
		case 1:

			fcl = 255;
			fch = 255;

			if (offsetY)
				fillRect(startx-offsetY, starty, startx, starty+limitX-1, bgColorText);

			if (offsetY + sizeY < fontSize)
				fillRect(startx-fontSize+1, starty, startx-sizeY-offsetY, starty+limitX-1, bgColorText);

			startRaw(startx-offsetY-sizeY+1, starty, startx-offsetY, starty+limitX-1);

			j = (sizeY-1) * sizeX;

			// and write it: 1 byte = 2 pixels with value 0..15
			for (x=limitX-1; x!=255; x--)
			{
				i = j;
				j++;

				for (y=sizeY-1; y!=65535; y--)
				{
					if (i&1)
						wdata = grey2word[pgm_read_byte(buf + (i>>1)) & 15];
					else
						wdata = grey2word[pgm_read_byte(buf + (i>>1)) >> 4];

					SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF);

					i-= sizeX;
				}
			}
			SPIWait();

			endRaw();
			break;
#endif
#ifdef TEXT_ROTATE_180
		// 180°
		case 2:
			startRaw(startx - limitX+1, starty - fontSize+1, startx, starty);

			// paint empty area above char
			if (offsetY + sizeY < fontSize)
			{
				wdata = bgColorText;
				i = (fontSize - sizeY - offsetY) * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			buf+= sizeX*sizeY >> 1;

			// uneven product of sizeX*sizeY -> fetch first 4 bit data and throw it away
			if ((sizeX&1) && (sizeY&1))
			{
				data = pgm_read_byte(buf);
				flag = 0;
			}
			else
				flag = 1;

			if (limitX < sizeX)
			{
				uint8_t todo = 0;
				uint8_t skip = sizeX - limitX;

				for (i=0; i<sizeX*sizeY; i++)
				{
					if (flag)
					{
						data = pgm_read_byte(--buf);
						wdata = grey2word[data&15];
						flag = 0;
					}
					else
					{
						wdata = grey2word[data>>4];
						flag = 1;
					}

					if (todo)
					{
						SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
						SPIFastWrite(wdata & 0xFF); // CHECK: maybe some NOPs here
						todo--;
					}
					else
					{
						skip--;

						if (skip == 0)
						{
							todo = limitX;
							skip = sizeX - limitX;
						}

					}
				}
			}
			else
			{
				for (i=0; i<sizeX*sizeY; i++)
				{
					if (flag)
					{
						data = pgm_read_byte(--buf);
						wdata = grey2word[data&15];
						flag = 0;
					}
					else
					{
						wdata = grey2word[data>>4];
						flag = 1;
					}
					SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF);
				}
				SPIWait();
			}

			// paint empty area below char
			if (offsetY)
			{
				wdata = bgColorText;
				i = offsetY * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			endRaw();
			break;
#endif
#ifdef TEXT_ROTATE_270
		// 270°
		case 3:
			if (offsetY)
				fillRect(startx, starty-limitX+1, startx+offsetY-1, starty, bgColorText);
			
			if (offsetY + sizeY < fontSize)
				fillRect(startx+offsetY+sizeY, starty-limitX+1, startx+fontSize-1, starty, bgColorText);

			startRaw(startx+offsetY, starty-limitX+1, startx+offsetY+sizeY-1, starty);

			for (x=0; x<limitX; x++)
			{
				i = limitX-1-x;

				for (y=0; y<sizeY; y++)
				{
					if (i&1)
						wdata = grey2word[pgm_read_byte(buf + (i>>1)) & 15];
					else
						wdata = grey2word[pgm_read_byte(buf + (i>>1)) >> 4];

					SPIFastWrite(wdata >> 8); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF);

					i+= sizeX;
				}
			}

			endRaw();
			break;
#endif
#ifdef TEXT_ROTATE_0_RLE
		// 0° RLE
		case 4:
			startRaw(startx, starty, startx+limitX-1, starty+fontSize-1);

			// paint empty area above char
			if (offsetY)
			{
				wdata = bgColorText;
				i = offsetY * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			i = pgm_read_word(fontOffsets + (c - fontFirstChar + 1)) - offset - 3;

			if (limitX < sizeX)
			{
				uint8_t todo = limitX;
				uint8_t skip = sizeX - limitX;

				while (i--)
				{
					data = pgm_read_byte(buf++);
					wdata = grey2word[data & 15];

					x = (data>>4) + 1;
					while (x--)
					{
						if (todo)
						{
							SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
							SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 9 NOPs
							todo--;
						}
						else
						{
							skip--;

							if (skip == 0)
							{
								todo = limitX;
								skip = sizeX - limitX;
							}
						}
					}
				}
				SPIWait();
			}
			else
			{
				while (i--)
				{
					data = pgm_read_byte(buf++);
					wdata = grey2word[data & 15];

					x = (data>>4) + 1;
					while (x--)
					{
						SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
						SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 13 NOPs
					}
				}
				SPIWait();
			}

			// paint empty area below char
			if (offsetY + sizeY < fontSize)
			{
				wdata = bgColorText;
				i = (fontSize - sizeY - offsetY) * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			endRaw();
			break;
#endif
#ifdef TEXT_ROTATE_180_RLE
		case 6:
			startRaw(startx - limitX+1, starty - fontSize+1, startx, starty);

			// paint empty area below char
			if (offsetY + sizeY < fontSize)
			{
				wdata = bgColorText;
				i = (fontSize - sizeY - offsetY) * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			i = pgm_read_word(fontOffsets + (c - fontFirstChar + 1)) - offset - 3;

			buf+= i;

			if (limitX < sizeX)
			{
				uint8_t todo = 0;
				uint8_t skip = sizeX - limitX;

				while (i--)
				{
					data = pgm_read_byte(--buf);
					wdata = grey2word[data & 15];

					x = (data>>4) + 1;
					while (x--)
					{
						if (todo)
						{
							SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
							SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 9 NOPs
							todo--;
						}
						else
						{
							skip--;

							if (skip == 0)
							{
								todo = limitX;
								skip = sizeX - limitX;
							}
						}
					}
				}
				SPIWait();
			}
			else
			{
				while (i--)
				{
					data = pgm_read_byte(--buf);
					wdata = grey2word[data & 15];

					x = (data>>4) + 1;
					while (x--)
					{
						SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
       					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 13 NOPs (because x is 8 bit and thus faster)
       				}
       				SPIWait();
				}
			}
			
			// paint empty area below char
			if (offsetY)
			{
				wdata = bgColorText;
				i = offsetY * limitX;
				while (i--)
				{
					SPIFastWrite(wdata >> 8);   NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 17 NOPs
					SPIFastWrite(wdata & 0xFF); NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; // 11 NOPs
				}
				SPIWait();
			}

			endRaw();
			break;
#endif
		default:
			return 0;
	}

	return limitX;
}

uint8_t STFT::drawString(uint8_t startx, uint16_t starty, char* txt, uint8_t rotate)
{
	switch(rotate)
	{
#if (!defined NO_TEXT_ROTATE_0) && (!defined NO_TEXT_ROTATE_0_RLE)
		case 0:
			startx+= drawChar(startx, starty, *txt++, rotate, 0xFF);
			while (*txt)
			{
				fillRect(startx, starty, startx, starty+fontSize-1, bgColorText);
				startx++;
				startx+= drawChar(startx, starty, *txt++, rotate, 0xFF);
			}
			break;
#endif
#ifdef TEXT_ROTATE_90
		case 1:
			starty+= drawChar(startx, starty, *txt++, rotate, 0xFF);
			while (*txt)
			{
				fillRect(startx-fontSize+1, starty, startx, starty, bgColorText);
				starty++;
				starty+= drawChar(startx, starty, *txt++, rotate, 0xFF);
			}
			break;
#endif
#if (!defined NO_TEXT_ROTATE_180) && (!defined NO_TEXT_ROTATE_180_RLE)
		case 2:
			startx-= drawChar(startx, starty, *txt++, rotate, 0xFF);
			while (*txt)
			{
				fillRect(startx, starty-fontSize+1, startx, starty, bgColorText);
				startx--;
				startx-= drawChar(startx, starty, *txt++, rotate, 0xFF);
			}
			break;
#endif
#ifdef TEXT_ROTATE_270
		case 3:
			starty-= drawChar(startx, starty, *txt++, rotate, 0xFF);
			while (*txt)
			{
				fillRect(startx, starty, startx+fontSize-1, starty, bgColorText);
				starty--;
				starty-= drawChar(startx, starty, *txt++, rotate, 0xFF);
			}
			break;
#endif
	}
}
		
uint16_t STFT::getStringWidth(char* txt)
{
	uint16_t width = 0;
	uint16_t offset;

	while (*txt)
	{
		if (*txt == 32)
		{
			width+= fontSize / 6 + 1;
		}
		else
		{
			offset = pgm_read_word(fontOffsets + (*txt - fontFirstChar));
			width+= pgm_read_byte(fontData + offset) + 1;
		}
		txt++;
	}

	if (width)
		width--;

	return width;
}

uint8_t STFT::drawStringLimit(uint8_t startx, uint16_t starty, char* txt, uint8_t rotate, int16_t limitX, uint8_t align)
{
	uint8_t 	width;
	uint16_t	lineWidth;

	if (align == ALIGN_RIGHT || align == ALIGN_CENTER)
	{
		lineWidth = getStringWidth(txt);

		if (lineWidth > limitX)
		{
			align = ALIGN_LEFT;
			lineWidth = limitX;
		}

		if (align == ALIGN_RIGHT)
			lineWidth = limitX - lineWidth;
		else
			lineWidth = (limitX - lineWidth) >> 1;
	}
	else lineWidth = 0;

	switch(rotate)
	{
#if (!defined NO_TEXT_ROTATE_0) && (!defined NO_TEXT_ROTATE_0_RLE)
		case 0:
			// if right / centered, draw left fill area
			if (lineWidth)
			{
				fillRect(startx, starty, startx+lineWidth-1, starty+fontSize-1, bgColorText);
	
				startx+= lineWidth;
				limitX-= lineWidth;
			}
	
			// draw chars
			width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
			startx+= width;
			limitX-= width;
			while (*txt && limitX>=0)
			{
				fillRect(startx-1, starty, startx-1, starty+fontSize-1, bgColorText);
	
				width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
				startx+= width;
				limitX-= width;
			}
	
			// if limitX not reached yet, pad fill to the right
			if (limitX > 0)
				fillRect(startx-1, starty, startx+limitX-1, starty+fontSize-1, bgColorText);
			break;
#endif
#ifdef TEXT_ROTATE_90
		case 1:
			// if right / centered, draw left fill area
			if (lineWidth)
			{
				fillRect(startx-fontSize+1, starty, startx, starty+lineWidth-1, bgColorText);
	
				starty+= lineWidth;
				limitX-= lineWidth;
			}
	
			// draw chars
			width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
			starty+= width;
			limitX-= width;
			while (*txt && limitX>=0)
			{
				fillRect(startx-fontSize+1, starty-1, startx, starty-1, bgColorText);
	
				width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
				starty+= width;
				limitX-= width;
			}
	
			// if limitX not reached yet, pad fill to the right
			if (limitX > 0)
				fillRect(startx-fontSize+1, starty-1, startx, starty+limitX-1, bgColorText);
			break;
#endif
#if !(defined NO_TEXT_ROTATE_180) && (!defined NO_TEXT_ROTATE_180_RLE)
		case 2:
			// if right / centered, draw left fill area
			if (lineWidth)
			{
				fillRect(startx-lineWidth+1, starty-fontSize+1, startx, starty, bgColorText);
	
				startx-= lineWidth;
				limitX-= lineWidth;
			}
	
			// draw chars
			width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
			startx-= width;
			limitX-= width;
			while (*txt && limitX>=0)
			{
				fillRect(startx+1, starty-fontSize+1, startx+1, starty, bgColorText);
	
				width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
				startx-= width;
				limitX-= width;
			}
			
			// if limitX not reached yet, pad fill to the right
			if (limitX > 0)
				fillRect(startx-limitX+1, starty-fontSize+1, startx+1, starty, bgColorText);
			break;
#endif
#ifdef TEXT_ROTATE_270
		case 3:
			// if right / centered, draw left fill area
			if (lineWidth)
			{
				fillRect(startx, starty-lineWidth+1, startx+fontSize-1, starty, bgColorText);
	
				starty-= lineWidth;
				limitX-= lineWidth;
			}

			// draw chars
			width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
			starty-= width;
			limitX-= width;
			while (*txt && limitX>=0)
			{
				fillRect(startx, starty+1, startx+fontSize-1, starty+1, bgColorText);
	
				width = drawChar(startx, starty, *txt++, rotate, limitX) + 1;
				starty-= width;
				limitX-= width;
			}
	
			// if limitX not reached yet, pad fill to the right
			if (limitX > 0)
				fillRect(startx, starty-limitX+1, startx+fontSize-1, starty+1, bgColorText);
			break;
#endif
	}
}

void STFT::setVerticalScrollingDefinition(uint16_t tfa, uint16_t vsa, uint16_t bfa)
{
	cbi(P_CS, B_CS);
	LCD_Write_COM(0x33);
	LCD_Write_DATA(tfa>>8);
	LCD_Write_DATA(tfa&0xFF);
	LCD_Write_DATA(vsa>>8);
	LCD_Write_DATA(vsa&0xFF);
	LCD_Write_DATA(bfa>>8);
	LCD_Write_DATA(bfa&0xFF);
	sbi(P_CS, B_CS);
}

void STFT::setVerticalScrollingStartAddress(uint16_t vsp)
{
	cbi(P_CS, B_CS);
	LCD_Write_COM(0x37);
	LCD_Write_DATA(vsp>>8);
	LCD_Write_DATA(vsp&0xFF);
	sbi(P_CS, B_CS);
}
