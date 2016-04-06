/*
 * @brief LPCXpresso Shield LCD functions
 *
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

#include <stdlib.h>
#include "lcd.h"

#ifdef LCD_FONT8X8
#include "font8x8.h"
#endif


typedef struct
{
    int fcolor;
    int bgcolor;
    int spacing;
    int fwidth;
    const lcd_font_t *font;
#ifdef LCD_ORIENT_PORTRAIT
    uint8_t fbuffer[LCD_X_RES >> 3][LCD_Y_RES];
#else
    uint8_t fbuffer[LCD_Y_RES >> 3][LCD_X_RES];
#endif
} lcd_context_t;



static void set_lcd_ram_address(uint8_t pgAddr, uint8_t colAddr);
static int get_char_width(int index);



static const uint8_t const lcd_init_cmd[] =
{
        0xA2, /* Set LCD Bias to 1/9 */
        0xA0, /* Set ADC Mode [NORMAL] */
        0xC0 | (LCD_FLIP_Y << 3), /* Set SHL mode */
        0x40, /* Set Start line address as 0 */
        0x2C,
        0x2E,
        0x2F,
        0x25,
        0x81,
        0x20,
        0xAF,  /* Turn LCD ON */
        0xB0,
        0x10,
        0x04,
};



static lcd_context_t lcd_context;



/* Initialize and turn on the LCD */
void lcd_init(void)
{
#ifdef LCD_FONT8X8
    lcd_context.font = &font8x8;
#endif
    platform_lcd_init();

    /* Initialize LCD and turn it On */
    for(int i = 0; i < sizeof(lcd_init_cmd); i++)
    {
        platform_lcd_write_cmd(&lcd_init_cmd[i], 1);
        if ((lcd_init_cmd[i] >> 3) == 5)
        {
            zn_rtos_delay_milliseconds(10);
        }
    }
    lcd_refresh(0, 0, LCD_X_RES - 1, LCD_Y_RES - 1);
}

/* Refresh the LCD frame buffer content to device */
void lcd_refresh(int left, int top, int right, int bottom)
{
    /* Sanity check boundaries */
    if (right >= LCD_X_RES)
    {
        right = LCD_X_RES - 1;
    }

    if (bottom >= LCD_Y_RES)
    {
        bottom = LCD_Y_RES - 1;
    }

#ifdef LCD_ORIENT_PORTRAIT
    pg = left;
    left = top;
    top = LCD_X_RES - right - 1;
    right = bottom;
    bottom = LCD_X_RES - pg - 1;
#endif
    for(int pg = top / 8; pg <= bottom / 8; pg ++)
    {
        set_lcd_ram_address(pg, left);
        platform_lcd_write_data(&lcd_context.fbuffer[pg][left], right - left + 1);
    }
}

/* Sets a pixel in display RAM */
void lcd_set_pixel(int x, int y, int col)
{
    if (x > LCD_X_RES || y > LCD_Y_RES)
        return;
#ifdef LCD_ORIENT_PORTRAIT
    if (1)
    {
        int t = x;
        x = y;
        y = LCD_X_RES - t - 1;
    }
#endif
    if (col)
    {
        lcd_context.fbuffer[y/8][x] |= 1 << (y & 7);
    }
    else
    {
        lcd_context.fbuffer[y/8][x] &= ~(1 << (y & 7));
    }
}

/* Sets a pixel in the Display device */
void lcd_put_pixel(int x, int y, int col)
{
    lcd_set_pixel(x, y, col);
    lcd_refresh(x, y, x, y);
}

/* Draw a rectangle with given coordinates and color */
void lcd_draw_rect(int left, int top, int right, int bottom, int col)
{
    for (int i = left; i <= right; i++)
    {
        lcd_set_pixel(i, top, col);
        lcd_set_pixel(i, bottom, col);
    }
    for (int i = top + 1; i < bottom; i++)
    {
        lcd_set_pixel(left, i, col);
        lcd_set_pixel(right, i, col);
    }
    lcd_refresh(left, top, right, bottom);
}

/* Draw a rectangle filled with given color */
void lcd_fill_rect(int left, int top, int right, int bottom, int col)
{
    /* TODO: improve this algorithm */
    for (int i = top; i <= bottom; i++)
    {
        for (int j = left; j <= right; j ++)
        {
            lcd_set_pixel(j, i, col);
        }
    }
    lcd_refresh(left, top, right, bottom);
}

/* Draws a line using given cordinates */
void lcd_draw_line(int x0, int y0, int x1, int y1, int col)
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    lcd_put_pixel(x0, y0, col);
    while(x0 != x1 || y0 != y1)
    {
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
        lcd_put_pixel(x0, y0, col);
    }
}


/* Sets the foreground color of the font */
void lcd_set_foreground_color(int color)
{
    lcd_context.fcolor = color;
}

/* Set the background color of the font */
void lcd_set_background_color(int color)
{
    lcd_context.bgcolor = color;
}

/* Set width of the font for fixed width fonts */
void lcd_set_font_width(int width)
{
    lcd_context.fwidth = width;
}

/* Sets the current font */
void lcd_set_font(const lcd_font_t *font)
{
    lcd_context.font = font;
}

/* Sets the space between two characters */
void lcd_set_char_spacing(int space)
{
    lcd_context.spacing = space;
}

/* Prints an ASCII character at a given position */
uint32_t lcd_put_char_xy(int xPos, int yPos, int ch)
{
    int w, h, r, c;
    const uint16_t *fp;

    if (lcd_context.font == NULL)
    {
        return xPos | (yPos << 16);
    }
    if ((ch < lcd_context.font->first_char) || (ch > lcd_context.font->last_char))
    {
        return xPos | (yPos << 16);
    }
    ch -= lcd_context.font->first_char;
    w = get_char_width(ch) + lcd_context.spacing;
    h = lcd_context.font->font_height;
    fp = lcd_context.font->font_table + (ch * h);
    for (r = 0; r < h; r++, fp++)
    {
        uint16_t t = 0x8000;
        for (c = 0; c < w; c ++, t >>= 1)
        {
            lcd_set_pixel(xPos + c, yPos + r, (*fp & t) ? lcd_context.fcolor :lcd_context.bgcolor);
        }
    }
    lcd_refresh(xPos, yPos, xPos + w - 1, yPos +  h - 1);

    return (xPos + w) | ((yPos + h) << 16);
}

/* Prints and ASCII string at a given position */
void lcd_put_str_xy(int xPos, int yPos, const char *str)
{
    uint32_t xp = xPos;
    while (str && *str)
    {
        xp = lcd_put_char_xy(xp & 0xFFFF, yPos, *str++);
    }
}

void lcd_put_str_centered_x(const char *str, int y_pos)
{
    int w = 0;

    for(const char *p = str; *p != 0; ++p)
    {
        int c = *p;
        c -= lcd_context.font->first_char;
        w += get_char_width(c) + lcd_context.spacing;
    }

    int x_pos = 1 + (LCD_X_RES - w) / 2;

    lcd_put_str_xy(x_pos, y_pos, str);
}

void lcd_put_str_centered(const char *str)
{
	lcd_put_str_centered_x(str, LCD_Y_RES/2);
}


/* Set column address */
static void set_lcd_ram_address(uint8_t pgAddr, uint8_t colAddr)
{
    uint8_t cmd[3];
    cmd[0] = 0xB0 | pgAddr;
    cmd[1] = colAddr & 0x0F; /* Lower addr */
    cmd[2] = 0x10 | (colAddr >> 4); /* Upper addr */
    platform_lcd_write_cmd(cmd, sizeof(cmd));
}

/* Get the width of a given char index (in pixels) */
static int get_char_width(int index)
{
    if (lcd_context.font->font_width_table != NULL)
    {
        return lcd_context.font->font_width_table[index];
    }
    else
    {
        return lcd_context.font->font_fixed_width;
    }
}

