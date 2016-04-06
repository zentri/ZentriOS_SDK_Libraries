/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once




#ifndef LCD_FLIP_Y
#define LCD_FLIP_Y  1  /* If 0 Mirrors Y direction */
#endif
/* Resolution of the LCD */
#ifdef LCD_ORIENT_PORTRAIT
#define LCD_X_RES            64
#define LCD_Y_RES            128
#else
#define LCD_X_RES            128
#define LCD_Y_RES            64
#endif
