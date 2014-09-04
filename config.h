/*
	Fast SPI:

		Pros:
			- It's faaaaast
		
		Cons:
			- Uses a little bit more code space (can be as high as 1kb in big projects)
			- Only works with 16MHz clock
			- Only tested on ATMega328p

	If you have glitches or absolutely need the extra bytes, try turning it off.
*/

#define FASTSPI

/*
	Memorysaving:

	- Only comment in MODEL_* you want to use
	- If you don't need all rotation of text, comment out the TEXT_ROTATE_* defines you don't need
*/

#define MODEL_ST7735
#define MODEL_ILI9341

#define TEXT_ROTATE_0
#define TEXT_ROTATE_90
#define TEXT_ROTATE_180
#define TEXT_ROTATE_270
#define TEXT_ROTATE_0_RLE
#define TEXT_ROTATE_180_RLE
