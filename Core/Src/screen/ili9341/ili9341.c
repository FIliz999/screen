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

#include "screen/ili9341/ili9341.h"

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

static void MX_SPI3_Init();
static void LCD_WR_DATA(uint8_t data);
static void LCD_WR_REG(uint8_t data);
static void LCD_direction(LCD_Horizontal_t direction, SPI_HandleTypeDef hspi3);
static void ILI9341_WritePixelsFullHeight(uint16_t x, uint16_t y, int i, int j, uint16_t scaleY, uint16_t scaleX,uint16_t color, SPI_HandleTypeDef hspi3 );
static void RESET_L(void);
static void RESET_H(void);
static void CS_L(void);
static void DC_L(void);
static void DC_H(void);
static void LED_H(void);
static void change(int x, uint8_t time, SPI_HandleTypeDef hspi3);
static void blackFond(SPI_HandleTypeDef hspi3);

SPI_HandleTypeDef hspi3;

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }

}

static void LCD_WR_REG(uint8_t data)
{
	DC_L();
	if (HAL_SPI_Transmit(&hspi3, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}

static void LCD_WR_DATA(uint8_t data)
{
	DC_H();
	if (HAL_SPI_Transmit(&hspi3, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}

static void change(int x, uint8_t time, SPI_HandleTypeDef hspi3)
{
	char caractere = time + '0';  // Conversion en caractère
	ILI9341_DrawChar(x, caractere, hspi3);
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
		LCD_WR_REG(0x36);
		LCD_WR_DATA(0x48);
		break;
	case ROTATE_90:
		LCD_WR_REG(0x36);
		LCD_WR_DATA(0x28);
		break;
	case ROTATE_180:
		LCD_WR_REG(0x36);
		LCD_WR_DATA(0x88);
		break;
	case ROTATE_270:
		LCD_WR_REG(0x36);
		LCD_WR_DATA(0xE8);
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

static void blackFond(SPI_HandleTypeDef hspi3){
	// Efface l'écran avec une couleur de fond (noir)
	ILI9341_SetWindow(0, 0, H_LCD - 1, W_LCD - 1, hspi3);
	for (uint16_t x = 0; x < W_LCD; x++) {
		for (uint16_t y = 0; y < H_LCD; y++) {
			ILI9341_WritePixel(x, y, 0x0000, hspi3);
		}
	}
}

// Initialization
void ILI9341_Init()
{
	MX_SPI3_Init();
	ILI9341_Reset();
	ILI9341_SoftReset(hspi3);

	/* Power Control A */
	LCD_WR_REG(0xCB);
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x02);
	/* Power Control B */
	LCD_WR_REG(0xCF);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xC1);
	LCD_WR_DATA(0x30);
	/* Driver timing control A */
	LCD_WR_REG(0xE8);
	LCD_WR_DATA(0x85);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x78);
	/* Driver timing control B */
	LCD_WR_REG(0xEA);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	/* Power on Sequence control */
	LCD_WR_REG(0xED);
	LCD_WR_DATA(0x64);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x81);
	/* Pump ratio control */
	LCD_WR_REG(0xF7);
	LCD_WR_DATA(0x20);
	/* Power Control 1 */
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x10);
	/* Power Control 2 */
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0x10);
	/* VCOM Control 1 */
	LCD_WR_REG(0xC5);
	LCD_WR_DATA(0x3E);
	LCD_WR_DATA(0x28);
	/* VCOM Control 2 */
	LCD_WR_REG(0xC7);
	LCD_WR_DATA(0x86);
	/* VCOM Control 2 */
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x48);
	/* Pixel Format Set */
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);    //16bit
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x18);
#if 0
	// Little Endian for TouchGFX
	LCD_WR_REG(0xF6);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x20); // Little Endian
#endif
	/* Display Function Control */
	LCD_WR_REG(0xB6);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x82);
	LCD_WR_DATA(0x27);
	/* 3GAMMA FUNCTION DISABLE */
	LCD_WR_REG(0xF2);
	LCD_WR_DATA(0x00);
	/* GAMMA CURVE SELECTED */
	LCD_WR_REG(0x26); //Gamma set
	LCD_WR_DATA(0x01); 	//Gamma Curve (G2.2)
	//Positive Gamma  Correction
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x31);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x4E);
	LCD_WR_DATA(0xF1);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x00);
	//Negative Gamma  Correction
	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x14);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x31);
	LCD_WR_DATA(0xC1);
	LCD_WR_DATA(0x48);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x31);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x0F);
	//EXIT SLEEP
	LCD_WR_REG(0x11);

	HAL_Delay(120);

	//TURN ON DISPLAY
	LCD_WR_REG(0x29);
	LCD_WR_DATA(0x2C);

	LCD_direction(ROTATE_270, hspi3);

	blackFond(hspi3);

}

void ILI9341_SetWindow(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, SPI_HandleTypeDef hspi3)
{
	// Set Window
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(start_x >> 8);
	LCD_WR_DATA(0xFF & start_x);
	LCD_WR_DATA(end_x >> 8);
	LCD_WR_DATA(0xFF & end_x);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(start_y >> 8);
	LCD_WR_DATA(0xFF & start_y);
	LCD_WR_DATA(end_y >> 8);
	LCD_WR_DATA(0xFF & end_y);

}

void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color, SPI_HandleTypeDef hspi3)
{
	uint8_t data[2];
	data[0] = color >> 8;
	data[1] = color;
	ILI9341_SetWindow(x, y, x, y, hspi3);
	// Enable to access GRAM
	LCD_WR_REG(0x2c);
	DC_H();
	if (HAL_SPI_Transmit(&hspi3, data, 2, 1000) != HAL_OK) {
		Error_Handler();
	}
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
void ILI9341_InitDrawString(const char *str) {
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

/*
 * Methode de changement de temps
 * Il y a deux parties, une pour les heures et l'autre pour les minutes.
 */
void ILI9341_ChangeTime(RTC_TimeTypeDef Time)
{
	if (Time.Minutes == 00)
	{
		uint8_t diz = Time.Hours / 10; // Get the dizaines
		uint8_t unite = Time.Hours % 10; // get unités
		if (unite != 0)
		{
			change(X_UNIT_HOUR, unite, hspi3);
		}
		else
		{
			change(X_DIX_HOUR, diz, hspi3);
			change(X_UNIT_HOUR, unite, hspi3);
		}
		// Il faut modifier l'unité et la dizaine des minutes
		change(X_DIX_MIN, 0, hspi3);
		change(X_UNIT_MIN, 0, hspi3);
	}
	else
	{
		uint8_t diz = Time.Minutes / 10; // Get the dizaines
		uint8_t unite = Time.Minutes % 10; // get unités
		if (unite != 0)
		{
			// Il faut juste modifier l'unité
			change(X_UNIT_MIN, unite, hspi3);
		}
		else
		{
			// Il faut modifier l'unité et la dizaine des minutes
			change(X_DIX_MIN, diz, hspi3);
			change(X_UNIT_MIN, unite, hspi3);
		}

	}
}
