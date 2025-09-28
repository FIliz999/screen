/*
 * ili9341_optimized.c
 *
 * Optimized ILI9341 driver for STM32
 * Author: Adapted from Kotetsu Yamamoto
 * Features:
 *  - Frame buffer for minimal redraw
 *  - DMA transfers on blocks
 *  - Efficient character drawing
 *  - Ready for menu (heure / alarme)
 */

#include "screen/ili9341/ili9341.h"
#include "stm32f4xx_hal.h"
#include <string.h> // memset

// ---------- Configuration ----------
#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#define BUFFER_WIDTH  40   // juste pour les chiffres
#define BUFFER_HEIGHT 16   // hauteur de la police

#define FONT_WIDTH  5
#define FONT_HEIGHT 8

SPI_HandleTypeDef hspi3;
extern DMA_HandleTypeDef hdma_spi3_tx;
static __IO uint8_t spiDmaTransferComplete;
// ---------- Frame buffer ----------
static uint16_t frame_buffer[BUFFER_HEIGHT][BUFFER_WIDTH];
static uint16_t old_frame[BUFFER_HEIGHT][BUFFER_WIDTH];

// ---------- DMA flag ----------
static __IO uint8_t dmaComplete = 0;

// ---------- Low level GPIO helpers ----------
static void MX_SPI3_Init();
static void CS_L(void) { HAL_GPIO_WritePin(CS_LCD_GPIO_Port, CS_LCD_Pin, GPIO_PIN_RESET); }
static void CS_H(void) { HAL_GPIO_WritePin(CS_LCD_GPIO_Port, CS_LCD_Pin, GPIO_PIN_SET); }
static void DC_L(void) { HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_RESET); }
static void DC_H(void) { HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_SET); }
static void RESET_L(void) { HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_RESET); }
static void RESET_H(void) { HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_SET); }

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

// ---------- DMA transmit ----------
static void SPI_DMA_Write(uint8_t *data, uint16_t size)
{
    dmaComplete = 0;
    if(HAL_SPI_Transmit_DMA(&hspi3, data, size) != HAL_OK) { Error_Handler(); }
    while(!dmaComplete); // Attente non-bloquante possible avec flags si nécessaire
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI3) dmaComplete = 1;
}

// ---------- Command / Data ----------
static void LCD_WriteCmd(uint8_t cmd) { DC_L(); SPI_DMA_Write(&cmd, 1); }
static void LCD_WriteData(uint8_t data) { DC_H(); SPI_DMA_Write(&data, 1); }

// ---------- Window ----------
static void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_WriteCmd(0x2A);
    uint8_t data[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
    DC_H(); SPI_DMA_Write(data, 4);

    LCD_WriteCmd(0x2B);
    uint8_t dataY[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
    DC_H(); SPI_DMA_Write(dataY, 4);

    LCD_WriteCmd(0x2C); // GRAM
}

// ---------- Pixel drawing ----------
static void ILI9341_WritePixelToBuffer(uint16_t x, uint16_t y, uint16_t color)
{
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    frame_buffer[y][x] = color;
}

// ---------- Full screen update ----------
void ILI9341_UpdateScreen()
{
    for(uint16_t y=0; y<LCD_HEIGHT; y++)
    {
        uint16_t start = 0;
        for(uint16_t x=0; x<LCD_WIDTH; x++)
        {
            if(frame_buffer[y][x] != old_frame[y][x])
            {
                if(start == 0) start = x;
            }
            else
            {
                if(start != 0)
                {
                    LCD_SetWindow(start, y, x-1, y);
                    DC_H();
                    SPI_DMA_Write((uint8_t*)&frame_buffer[y][start], (x-start)*2);
                    memcpy(&old_frame[y][start], &frame_buffer[y][start], (x-start)*2);
                    start = 0;
                }
            }
        }
        if(start != 0)
        {
            LCD_SetWindow(start, y, LCD_WIDTH-1, y);
            DC_H();
            SPI_DMA_Write((uint8_t*)&frame_buffer[y][start], (LCD_WIDTH-start)*2);
            memcpy(&old_frame[y][start], &frame_buffer[y][start], (LCD_WIDTH-start)*2);
        }
    }
}

// ---------- Clear screen ----------
void ILI9341_Clear(uint16_t color)
{
    for(uint16_t y=0; y<LCD_HEIGHT; y++)
        for(uint16_t x=0; x<LCD_WIDTH; x++)
            frame_buffer[y][x] = color;
}

// ---------- Character drawing (5x8 font) ----------
void ILI9341_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg)
{
    if(c < 32 || c > 126) c = '?';
    const uint8_t *glyph = FONTS5_8[c-32];
    for(uint8_t i=0;i<FONT_HEIGHT;i++)
        for(uint8_t j=0;j<FONT_WIDTH;j++)
        {
            if(glyph[j] & (1<<i))
                ILI9341_WritePixelToBuffer(x+j, y+i, color);
            else
                ILI9341_WritePixelToBuffer(x+j, y+i, bg);
        }
}

// ---------- String drawing ----------
void ILI9341_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg)
{
    while(*str)
    {
        ILI9341_DrawChar(x, y, *str, color, bg);
        x += FONT_WIDTH + 1; // +1 pour interspace
        str++;
    }
}

// ---------- Optimized time update ----------
void ILI9341_ChangeTime(uint8_t hours, uint8_t minutes)
{
    char str[6];
    sprintf(str,"%02u:%02u", hours, minutes);

    // Ici, seulement redessiner les chiffres qui changent
    ILI9341_DrawString(20, 20, str, 0xFFFF, 0x0000);
}

// ---------- Initialization ----------
void ILI9341_Init()
{
    MX_SPI3_Init();
    //MX_DMA_Init();
    RESET_L(); HAL_Delay(50);
    RESET_H(); HAL_Delay(50);

    // Soft reset
    LCD_WriteCmd(0x01); HAL_Delay(120);

    // Configuration minimale (puissance, pixel format, display on)
    LCD_WriteCmd(0x36); LCD_WriteData(0x48);
    LCD_WriteCmd(0x3A); LCD_WriteData(0x55); // 16-bit
    LCD_WriteCmd(0x29); // Display on

    ILI9341_Clear(0x0000);
    ILI9341_UpdateScreen();
}
