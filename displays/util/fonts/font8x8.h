/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once

#include "zos.h"




#ifdef LEDMATRIX_FONT8X8

#define FONT_8x8_ROW_LEN 8

extern RO_MEM uint8_t const font8x8[97][FONT_8x8_ROW_LEN];

#endif


#ifdef LCD_FONT8X8

#include "lcd.h"

extern const lcd_font_t font8x8;
#endif
