/*
 * lcd.h
 *
 *  Monochrome LCD interface
 *
 *  A corresponding driver implementing this interface must also
 *  be included in the build.
 *
 */

/*
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */



#pragma once

#include "zos.h"
#include "lcd_config.h"


/** 
 * @addtogroup lib_displays_lcd_monochrome
 * @{
 */


typedef struct
{
	int16_t font_height;
	uint8_t first_char;
	uint8_t last_char;
	uint8_t font_fixed_width;
	const uint16_t *font_table;
	const uint8_t  *font_width_table;
} lcd_font_t;




/**
 * @brief	Initialize the LCD turns it ON
 * @return	Nothing
 */
void lcd_init(void);

/**
 * @brief	Update content of Display RAM to Display device
 * @param	left	: Left coordinate [X coordinate]
 * @param	top		: Top coordinate [Y coordinate]
 * @param	right	: Right coordinate [X coordinate]
 * @param	bottom	: Bottom coorinate [Y coordinate]
 * @return	Nothing
 * @note	@a left should not be greater than right and
 * @a top should not be greater than bottom, if not the behavior
 * is undefined.
 */
void lcd_refresh(int left, int top, int right, int bottom);

/**
 * @brief	Turns On/Off a pixel at (@a x,@a y) in Display device
 * @param	x	: X coordinate of the pixel
 * @param	y	: Y coordinate of the pixel
 * @param	col	: Color of the pixel (0 - OFF[WHITE]; 1 - ON[BLACK])
 * @return	Nothing
 */
void lcd_put_pixel(int x, int y, int col);

/**
 * @brief	Turns On/Off a pixel at (@a x,@a y) in Display RAM
 * @param	x	: X coordinate of the pixel
 * @param	y	: Y coordinate of the pixel
 * @param	col	: Color of the pixel (0 - OFF[WHITE]; 1 - ON[BLACK])
 * @return	Nothing
 * @note	Set pixel will not display the pixel until LCD_Refresh is
 * called for the region containing this pixel.
 */
void lcd_set_pixel(int x, int y, int col);

/**
 * @brief	Draws a rectangle from (top,right) to (bottom,left)
 * @param	left	: Left coordinate [X coordinate]
 * @param	top		: Top coordinate [Y coordinate]
 * @param	right	: Right coordinate [X coordinate]
 * @param	bottom	: Bottom coorinate [Y coordinate]
 * @param	col	: Color of the pixel (0 - OFF[WHITE]; 1 - ON[BLACK])
 * @return	Nothing
 */
void lcd_draw_rect(int left, int top, int right, int bottom, int col);

/**
 * @brief	Fills a rectangle from (top,right) to (bottom,left)
 * @param	left	: Left coordinate [X coordinate]
 * @param	top		: Top coordinate [Y coordinate]
 * @param	right	: Right coordinate [X coordinate]
 * @param	bottom	: Bottom coorinate [Y coordinate]
 * @param	col	: Color of the pixel (0 - OFF[WHITE]; 1 - ON[BLACK])
 * @return	Nothing
 */
void lcd_fill_rect(int left, int top, int right, int bottom, int col);

/**
 * @brief	Draws a line from x0,y0 to x1,y1
 * @param	x0	: X0
 * @param	y0	: Y0
 * @param	x1	: X1
 * @param	y1	: Y1
 * @param	col	: Color of the pixel (0 - OFF[WHITE]; 1 - ON[BLACK])
 * @return	Nothing
 */
void lcd_draw_line(int x0, int y0, int x1, int y1, int col);

/**
 * @brief	Sets the foreground color of font
 * @param	color	: Color 1 - ON[Black], 0 - OFF[WHITE]
 * @return	Nothing
 */
void lcd_set_foreground_color(int color);

/**
 * @brief	Sets the background color of font
 * @param	color	: Color 1 - ON[Black], 0 - OFF[WHITE]
 * @return	Nothing
 */
void lcd_set_background_color(int color);

/**
 * @brief	Sets the current font
 * @param	font	: Pointer to a font structure (swim font)
 * @return	Nothing
 */
void lcd_set_font(const lcd_font_t *font);

/**
 * @brief	Sets the space between two chars
 * @param	space	: Space in pixels
 * @return	Nothing
 * @note	This functions sets the space that will
 * be left between chars in addition to the default
 * space provided by the font itself
 */
void lcd_set_char_spacing(int space);

/**
 * @brief	Sets width for fixed width fonts
 * @param	width	: Width of a single char in pixels
 * @return	Nothing
 * @note	This width is effective only for fonts
 * that has fixed with and having the width table
 * initialized to null
 */
void lcd_set_font_width(int width);

/**
 * @brief	Prints ASCII character at given position
 * @param	xPos	: x Position in pixels
 * @param	yPos	: y Position in pixels
 * @param	ch		: ASCII character (int casted)
 * @return	Next char's x Position in lower half word and
 * next char's y position in upper half word
 */
uint32_t lcd_put_char_xy(int xPos, int yPos, int ch);

/**
 * @brief	Prints ASCII string at given position
 * @param	xPos	: x Position in pixels
 * @param	yPos	: y Position in pixels
 * @param	str		: Pointer to NUL terminated ASCII string
 * @return	nothing
 */
void lcd_put_str_xy(int xPos, int yPos, const char *str);

/**
 * @brief	Prints ASCII string centered on screen
 * @param	str		: Pointer to NUL terminated ASCII string
 * @return	nothing
 */
void lcd_put_str_centered(const char *str);

void lcd_put_str_centered_x(const char *str, int y_pos);

/**
 * @}
*/ 
