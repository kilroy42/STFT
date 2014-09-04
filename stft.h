#ifndef _STFT_H_
#define _STFT_H_

#include "Arduino.h"

#include "config.h"

#define sbi(reg, bitmask) *reg |= bitmask
#define cbi(reg, bitmask) *reg &= ~bitmask

#ifdef FASTSPI

#define SPIFastWrite(v)			SPDR = v;
#define NOP						__asm__("nop");
#define SPIWait() 				{ while (!(SPSR & _BV(SPIF))) ; }

#else

#define SPIFastWrite(v)			SPITransfer(v)
#define NOP 					;
#define SPIWait() 				;

#endif

inline void SPITransfer(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & _BV(SPIF))) ;
}

#define RGB0(r, g, b)			((r&248) | (g>>5))					// RRRRRGGG
#define RGB1(r, g, b)			(((g&28)<<3) | (b>>3))				// GGGBBBBB
#define RGB(r, g, b)			( (b>>3) | ((g>>2)<<5) | ((r>>3)<<11) )
//								(RGB1(r, g, b) | ((RGB0(r, g, b) << 8)))

#define SETFONT(tft, __font__)	tft.setFont(font ## __font__ ## Data, font ## __font__ ## Offsets, font ## __font__ ## Height, font ## __font__ ## FirstChar)

#define ALIGN_LEFT				0
#define ALIGN_RIGHT				1
#define ALIGN_CENTER			2

#ifdef MODEL_ILI9341
#define ILI9341	0
#endif

#ifdef MODEL_ST7735
#define ST7735 1
#endif

class STFT
{
	protected:

		volatile uint8_t    *P_RS, *P_CS, *P_RST;
		volatile uint8_t    B_RS, B_CS, B_RST;
		uint8_t             ap_cs, ap_rst, ap_ser;

		uint16_t			fgColorText;
		uint16_t			bgColorText;
		uint16_t			grey2word[16];

		uint8_t				fontFirstChar;
		uint16_t*			fontOffsets;
		uint8_t*			fontData;
		
		uint8_t				model;

	protected:

		void				LCD_Write_COM(char VL);
		void				LCD_Write_DATA(char VH,char VL);
		void				LCD_Write_DATA(char VL);
		void				LCD_Write_COM_DATA(uint8_t com, uint16_t data);
		
		void				updateTextColor();

	public:

		uint8_t				fcl, fch;
		uint8_t				fontSize;
		uint8_t				width;
		uint16_t			height;

	public:

							STFT(uint8_t model, uint8_t CS, uint8_t RST, uint8_t SER);

		// must be called to initialize display
		void				init();

		// clear screen with black background
		void				clrScr();

		// draw functions
		void				setColor(uint16_t rgb);

		void				fillRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1);
		void				fillRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1, uint16_t color);
		void				drawRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1);

		// font functions
		void				setTextBackColor(uint16_t color);
		void				setTextColor(uint16_t color);

		void				setFont(uint8_t* data, uint16_t* offsets, uint8_t size, uint8_t firstChar);
		uint16_t			getStringWidth(char* txt);

		uint8_t				drawChar(uint8_t startx, uint16_t starty, uint8_t c, uint8_t rotate, uint16_t limitX);
		uint8_t				drawString(uint8_t startx, uint16_t starty, char* txt, uint8_t rotate);
		uint8_t				drawStringLimit(uint8_t startx, uint16_t starty, char* txt, uint8_t rotate, int16_t limitX, uint8_t align);
		
		// specialized functions for RAW writing
		void				startRaw();
		void				startRaw(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1);
		void				endRaw();

		// scroll area
		void				setVerticalScrollingDefinition(uint16_t tfa, uint16_t vsa, uint16_t bfa);
		void				setVerticalScrollingStartAddress(uint16_t vsp);

};

#endif
