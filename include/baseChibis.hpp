#ifndef BASE_CHIBIS_HPP
#define BASE_CHIBIS_HPP

#include <avr/pgmspace.h>
#include "halDisplay.hpp"

#define NUM_PIXELS			SCREEN_WIDTH * SCREEN_HEIGHT
#define LEN_IMG_BYTE_ARR		NUM_PIXELS / 8

const bool CHIBIS_DEBUG = false;

// 128x60px
extern const unsigned char IMG_DECOMPRESSED[];
extern const unsigned char HAPPY_CHIBI [LEN_IMG_BYTE_ARR] PROGMEM;
extern const unsigned char BLANK_CHIBI [LEN_IMG_BYTE_ARR] PROGMEM;
extern const unsigned char * ALL_CHIBIS[] PROGMEM;
extern unsigned char chibiOutputImage [LEN_IMG_BYTE_ARR];

typedef struct _COMPRESSED_CHIBI
{
	unsigned char BytesOfSize;
	unsigned char * Data;

} COMPRESSED_CHIBI, *PTR_COMPRESSED_CHIBI;


typedef struct _ENCODED_PAIR
{
	unsigned char Value;
	unsigned char Reps;
} ENCODED_PAIR, *PTR_ENCODED_PAIR;

typedef enum _CHIBIS_STATUS
{
	CHIBIS_STATUS_SUCCESS 			= 0,
	CHIBIS_STATUS_FAILED  			= 1,
	CHIBIS_STATUS_INVALID_COORDS	= 2,
	CHIBIS_STATUS_MAX
} CHIBIS_STATUS, *PTR_CHIBIS_STATUS;

typedef enum _ENUM_ALL_CHIBIS
{
	NUM_BLANK_CHIBI = 0,
	NUM_HAPPY_CHIBI = 1,
	NUM_CHIBI_MAX
} ENUM_ALL_CHIBIS, *PTR_ENUM_ALL_CHIBIS;


void rleDecompressImage(const unsigned char * CompImage);

unsigned char * chibisAnimateBlankToSmile(unsigned char Frame);

void chibisLoadBaseOutputFrame(unsigned char Index);

CHIBIS_STATUS chibisDrawPixel(unsigned char OriginX, unsigned char OriginY, char OffsetX, char OffsetY);

#endif