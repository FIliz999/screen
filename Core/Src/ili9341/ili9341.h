/*
 * ili9341.h
 *
 *  Created on: 2019/12/26
 *      Author: Kotetsu Yamamoto
 *      Copyright [Kotetsu Yamamoto]

MIT License

Copyright (c) 2020 Kotestu Yamamoto

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#ifndef INC_ILI9341_H_
#define INC_ILI9341_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "main.h" // For STM32F4
#include "fonts.h"
#include "def.h"

#define GUI_WIDTH 320
#define GUI_HEIGHT 240

void ILI9341_Init(SPI_HandleTypeDef hspi3);
void ILI9341_SetWindow(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, SPI_HandleTypeDef hspi3);
void ILI9341_DrawBitmap(uint16_t w, uint16_t h, uint8_t *s, SPI_HandleTypeDef hspi3);
void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color, SPI_HandleTypeDef hspi3);
void ILI9341_EndOfDrawBitmap(void);
void ILI9341_InitWindowsWithFont(SPI_HandleTypeDef hspi3, uint16_t color);
void ILI9341_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, SPI_HandleTypeDef hspi3);
void ILI9341_InitDrawString(const char *str, uint16_t color, uint16_t bgcolor, SPI_HandleTypeDef hspi3);
void LCD_WR_REG(uint8_t data, SPI_HandleTypeDef hspi3);
void LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size, SPI_HandleTypeDef hspi3);

#ifdef __cplusplus
}
#endif

#endif /* INC_ILI9341_H_ */
