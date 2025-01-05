/*
 * ili9341.c
 *
 *  Created on: 2019/12/26
 *      Author: Kotetsu Yamamoto
 *      Copyright [Kotetsu Yamamoto]

I refer https://github.com/dtnghia2206/TFTLCD/blob/master/TFTLCD/ILI9341/ILI9341_Driver.c

from Original source:

MIT License

Copyright (c) 2019 NghiaDT

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

#include "ili9341.h"

// This function is for compatible HiLetgo ILI9341

typedef enum {
	ROTATE_0,
	ROTATE_90,
	ROTATE_180,
	ROTATE_270
} LCD_Horizontal_t;

extern void Error_Handler(void);
static __IO uint8_t spiDmaTransferComplete;

void ILI9341_Reset(void);
void ILI9341_SoftReset(SPI_HandleTypeDef hspi3);

void LCD_WR_REG(uint8_t data, SPI_HandleTypeDef hspi3);
static void LCD_WR_DATA(uint8_t data, SPI_HandleTypeDef hspi3);
static void LCD_direction(LCD_Horizontal_t direction, SPI_HandleTypeDef hspi3);
static void ILI9341_WritePixelsFullHeight(uint16_t x, uint16_t y, int i, int j, uint16_t scaleY, uint16_t scaleX,uint16_t color, SPI_HandleTypeDef hspi3 );
static void RESET_L(void);
static void RESET_H(void);
static void CS_L(void);
static void DC_L(void);
static void DC_H(void);
static void LED_H(void);
static void changeMinutes(int x, uint8_t minute, SPI_HandleTypeDef hspi3);
static void changeHours();

// Initialization
void ILI9341_Init(SPI_HandleTypeDef hspi3)
{
	ILI9341_Reset();
	ILI9341_SoftReset(hspi3);

	/* Power Control A */
	LCD_WR_REG(0xCB, hspi3);
	LCD_WR_DATA(0x39, hspi3);
	LCD_WR_DATA(0x2C, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x34, hspi3);
	LCD_WR_DATA(0x02, hspi3);
	/* Power Control B */
	LCD_WR_REG(0xCF, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0xC1, hspi3);
	LCD_WR_DATA(0x30, hspi3);
	/* Driver timing control A */
	LCD_WR_REG(0xE8, hspi3);
	LCD_WR_DATA(0x85, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x78, hspi3);
	/* Driver timing control B */
	LCD_WR_REG(0xEA, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	/* Power on Sequence control */
	LCD_WR_REG(0xED, hspi3);
	LCD_WR_DATA(0x64, hspi3);
	LCD_WR_DATA(0x03, hspi3);
	LCD_WR_DATA(0x12, hspi3);
	LCD_WR_DATA(0x81, hspi3);
	/* Pump ratio control */
	LCD_WR_REG(0xF7, hspi3);
	LCD_WR_DATA(0x20, hspi3);
	/* Power Control 1 */
	LCD_WR_REG(0xC0, hspi3);
	LCD_WR_DATA(0x10, hspi3);
	/* Power Control 2 */
	LCD_WR_REG(0xC1, hspi3);
	LCD_WR_DATA(0x10, hspi3);
	/* VCOM Control 1 */
	LCD_WR_REG(0xC5, hspi3);
	LCD_WR_DATA(0x3E, hspi3);
	LCD_WR_DATA(0x28, hspi3);
	/* VCOM Control 2 */
	LCD_WR_REG(0xC7, hspi3);
	LCD_WR_DATA(0x86, hspi3);
	/* VCOM Control 2 */
	LCD_WR_REG(0x36, hspi3);
	LCD_WR_DATA(0x48, hspi3);
	/* Pixel Format Set */
	LCD_WR_REG(0x3A, hspi3);
	LCD_WR_DATA(0x55, hspi3);    //16bit
	LCD_WR_REG(0xB1, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x18, hspi3);
#if 0
	// Little Endian for TouchGFX
	LCD_WR_REG(0xF6, hspi3);
	LCD_WR_DATA(0x01, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x20, hspi3); // Little Endian
#endif
	/* Display Function Control */
	LCD_WR_REG(0xB6, hspi3);
	LCD_WR_DATA(0x08, hspi3);
	LCD_WR_DATA(0x82, hspi3);
	LCD_WR_DATA(0x27, hspi3);
	/* 3GAMMA FUNCTION DISABLE */
	LCD_WR_REG(0xF2, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	/* GAMMA CURVE SELECTED */
	LCD_WR_REG(0x26, hspi3); //Gamma set
	LCD_WR_DATA(0x01, hspi3); 	//Gamma Curve (G2.2)
	//Positive Gamma  Correction
	LCD_WR_REG(0xE0, hspi3);
	LCD_WR_DATA(0x0F, hspi3);
	LCD_WR_DATA(0x31, hspi3);
	LCD_WR_DATA(0x2B, hspi3);
	LCD_WR_DATA(0x0C, hspi3);
	LCD_WR_DATA(0x0E, hspi3);
	LCD_WR_DATA(0x08, hspi3);
	LCD_WR_DATA(0x4E, hspi3);
	LCD_WR_DATA(0xF1, hspi3);
	LCD_WR_DATA(0x37, hspi3);
	LCD_WR_DATA(0x07, hspi3);
	LCD_WR_DATA(0x10, hspi3);
	LCD_WR_DATA(0x03, hspi3);
	LCD_WR_DATA(0x0E, hspi3);
	LCD_WR_DATA(0x09, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	//Negative Gamma  Correction
	LCD_WR_REG(0xE1, hspi3);
	LCD_WR_DATA(0x00, hspi3);
	LCD_WR_DATA(0x0E, hspi3);
	LCD_WR_DATA(0x14, hspi3);
	LCD_WR_DATA(0x03, hspi3);
	LCD_WR_DATA(0x11, hspi3);
	LCD_WR_DATA(0x07, hspi3);
	LCD_WR_DATA(0x31, hspi3);
	LCD_WR_DATA(0xC1, hspi3);
	LCD_WR_DATA(0x48, hspi3);
	LCD_WR_DATA(0x08, hspi3);
	LCD_WR_DATA(0x0F, hspi3);
	LCD_WR_DATA(0x0C, hspi3);
	LCD_WR_DATA(0x31, hspi3);
	LCD_WR_DATA(0x36, hspi3);
	LCD_WR_DATA(0x0F, hspi3);
	//EXIT SLEEP
	LCD_WR_REG(0x11, hspi3);

	HAL_Delay(120);

	//TURN ON DISPLAY
	LCD_WR_REG(0x29, hspi3);
	LCD_WR_DATA(0x2C, hspi3);

	LCD_direction(ROTATE_270, hspi3);

}

void ILI9341_SetWindow(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, SPI_HandleTypeDef hspi3)
{
	// Set Window
	LCD_WR_REG(0x2a, hspi3);
	LCD_WR_DATA(start_x >> 8, hspi3);
	LCD_WR_DATA(0xFF & start_x, hspi3);
	LCD_WR_DATA(end_x >> 8, hspi3);
	LCD_WR_DATA(0xFF & end_x, hspi3);

	LCD_WR_REG(0x2b, hspi3);
	LCD_WR_DATA(start_y >> 8, hspi3);
	LCD_WR_DATA(0xFF & start_y, hspi3);
	LCD_WR_DATA(end_y >> 8, hspi3);
	LCD_WR_DATA(0xFF & end_y, hspi3);

}

void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color, SPI_HandleTypeDef hspi3)
{
	uint8_t data[2];
	data[0] = color >> 8;
	data[1] = color;
	ILI9341_SetWindow(x, y, x, y, hspi3);
	// Enable to access GRAM
	LCD_WR_REG(0x2c, hspi3);
	DC_H();
	if (HAL_SPI_Transmit(&hspi3, data, 2, 1000) != HAL_OK) {
		Error_Handler();
	}
}

static void ConvHL(uint8_t *s, int32_t l)
{
	uint8_t v;
	while (l > 0) {
		v = *(s+1);
		*(s+1) = *s;
		*s = v;
		s += 2;
		l -= 2;
	}
}

// Call this function after ILI9341_SetWindow
// This function is non blocked
// The variable for Callback is open. User should set by himself
void ILI9341_DrawBitmap(uint16_t w, uint16_t h, uint8_t *s, SPI_HandleTypeDef hspi3)
{
	// Enable to access GRAM
	LCD_WR_REG(0x2c, hspi3);

	DC_H();
#if 0
	__HAL_SPI_DISABLE(&hspi1);
	hspi3.Instance->CR2 |= SPI_DATASIZE_16BIT; // Set 16 bit mode
	__HAL_SPI_ENABLE(&hspi1);
#endif
	ConvHL(s, (int32_t)w*h*2);
	HAL_SPI_Transmit_DMA(&hspi3, (uint8_t*)s, w * h *2);
#if 0
	__HAL_SPI_DISABLE(&hspi1);
	hspi3.Instance->CR2 &= ~(SPI_DATASIZE_16BIT); // Set 8 bit mode
	__HAL_SPI_ENABLE(&hspi1);
#endif
}

// User should call it at callback
void ILI9341_EndOfDrawBitmap(void)
{
#if 0
	__HAL_SPI_DISABLE(&hspi1);
	hspi3.Instance->CR2 &= ~(SPI_DATASIZE_16BIT); // Set 8 bit mode
	__HAL_SPI_ENABLE(&hspi3);
#endif
}

void ILI9341_Reset(void)
{
	RESET_L();
	HAL_Delay(100);
	RESET_H();
	HAL_Delay(100);
	CS_L();
	LED_H();
}

void ILI9341_SoftReset(SPI_HandleTypeDef hspi3)
{
	uint8_t cmd;
	cmd = 0x01; //Software reset
	DC_L();
	if (HAL_SPI_Transmit(&hspi3, &cmd, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}


void LCD_WR_REG(uint8_t data, SPI_HandleTypeDef hspi3)
{
	DC_L();
	if (HAL_SPI_Transmit(&hspi3, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}

static void LCD_WR_DATA(uint8_t data, SPI_HandleTypeDef hspi3)
{
	DC_H();
	if (HAL_SPI_Transmit(&hspi3, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}

void LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size, SPI_HandleTypeDef hspi3)
{
	/* Swap endianes */
	ConvHL(pData, (int32_t)Size*2);

	DC_H();
//	HAL_SPI_Transmit(&hspi3, (uint8_t*)pData, Size * 2, HAL_MAX_DELAY);
	spiDmaTransferComplete = 0;
	HAL_SPI_Transmit_DMA(&hspi3, pData, Size*2 );
	//HAL_SPI_Transmit_DMA(&hspi3, (uint8_t*)pData, Size );
	while(spiDmaTransferComplete == 0);
}

void ILI9341_InitWindowsWithFont(SPI_HandleTypeDef hspi3, uint16_t color){
	// Efface l'écran avec une couleur de fond (noir)
	ILI9341_SetWindow(0, 0, H_LCD - 1, W_LCD - 1, hspi3);
	for (uint16_t x = 0; x < W_LCD; x++) {
		for (uint16_t y = 0; y < H_LCD; y++) {
			ILI9341_WritePixel(x, y, color, hspi3);
		}
	}
}

// Dessine un caractère en utilisant une police 8x8
void ILI9341_DrawChar(uint16_t x, char c, SPI_HandleTypeDef hspi3) {
	if (c < 32 || c > 126) {
	        c = '?'; // Si le caractère n'est pas supporté
	}

	// Trouver l'index du caractère dans la table (32 = espace, donc l'index est c - 32)
	const uint8_t *glyph = FONTS5_8[c - 32];  // Font_5x8[caractère - 32]
	const uint16_t scaleX = SIZE_NUMBERS / 8;
	const uint16_t scaleY = H_LCD / 8;

	// Parcourir chaque ligne du caractère (chaque ligne est représentée par un octet)
	for (int i = 0; i < 8; i++) {  // Parcourt les lignes (de 0 à 7)
		for (int j = 0; j < 5; j++) {  // Parcourt les colonnes (de 0 à 4)
			if (glyph[j] & (1 << i)) {  // Vérifie si le bit est 1 (pixel actif)
				ILI9341_WritePixelsFullHeight(x, Y_CHAR, i, j, scaleX, scaleY, 0xFFFF, hspi3);
			} else {
				// Dessiner le fond pour ce bloc
				ILI9341_WritePixelsFullHeight(x, Y_CHAR, i, j, scaleX, scaleY, 0x0000, hspi3);
			}
		}
	}

    // Ajoute un espace entre les caractères
    for (int8_t j = 0; j < 7; j++) {
        ILI9341_WritePixel(x + 5, Y_CHAR + j, 0x0000, hspi3);
    }
}

// Dessine une chaîne de caractères
void ILI9341_InitDrawString(const char *str, SPI_HandleTypeDef hspi3) {
	int cmpt = 0;
	uint16_t x = MARGE_NUMBERS_X;
	if (strlen(str) == 5)
	{
		while (*str)
		{
			if (cmpt != 0){
				x = MARGE_NUMBERS_X + cmpt * SIZE_NUMBERS + INTERVALE_NUMBERS * cmpt;
			}
			ILI9341_DrawChar(x, *str, hspi3);
			str++;
			cmpt += 1;
		}
	}
	else
	{
		printf("Erreur le string est composé de plus de 5 éléments");
	}
}

void changeTime(RTC_TimeTypeDef Time, SPI_HandleTypeDef hspi3)
{
	if (Time.Minutes == 00)
	{
		changeHours();
		//changeMinutes(Time.Minutes, hspi3);
	}
	else
	{
		uint8_t diz = Time.Minutes / 10; // Get the dizaines
		uint8_t unite = Time.Minutes % 10; // get unités
		if (unite != 0)
		{
			// Il faut juste modifier l'unité
			changeMinutes(X_UNIT_MIN, unite, hspi3);
		}
		else
		{
			// Il faut modifier l'unité et la dizaine des minutes
			changeMinutes(X_DIX_MIN, diz, hspi3);
			changeMinutes(X_UNIT_MIN, unite, hspi3);
		}

	}
}

static void changeMinutes(int x, uint8_t minute, SPI_HandleTypeDef hspi3)
{
	char caractere = minute + '0';  // Conversion en caractère
	ILI9341_DrawChar(x, caractere, hspi3);
}

static void changeHours()
{

}

static void ILI9341_WritePixelsFullHeight(uint16_t x, uint16_t y, int i, int j, uint16_t scaleX, uint16_t scaleY, uint16_t color, SPI_HandleTypeDef hspi3 )
{
	for (int dy = 0; dy < scaleY; dy++) {
		for (int dx = 0; dx < scaleX; dx++) {
			ILI9341_WritePixel(x + j * scaleX + dx, y + i * scaleY + dy, color, hspi3);
		}
	}
}

static void LCD_direction(LCD_Horizontal_t direction, SPI_HandleTypeDef hspi3)
{
	switch (direction) {
	case ROTATE_0:
		LCD_WR_REG(0x36, hspi3);
		LCD_WR_DATA(0x48, hspi3);
		break;
	case ROTATE_90:
		LCD_WR_REG(0x36, hspi3);
		LCD_WR_DATA(0x28, hspi3);
		break;
	case ROTATE_180:
		LCD_WR_REG(0x36, hspi3);
		LCD_WR_DATA(0x88, hspi3);
		break;
	case ROTATE_270:
		LCD_WR_REG(0x36, hspi3);
		LCD_WR_DATA(0xE8, hspi3);
		break;
	}
}

static void RESET_L(void)
{
	HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_RESET);
}

static void RESET_H(void)
{
	HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_SET);
}

static void CS_L(void)
{
	HAL_GPIO_WritePin(CS_LCD_GPIO_Port, CS_LCD_Pin, GPIO_PIN_RESET);
}

static void DC_L(void)
{
	HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_RESET);
}

static void DC_H(void)
{
	HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_SET);
}

static void LED_H(void)
{
	//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}
