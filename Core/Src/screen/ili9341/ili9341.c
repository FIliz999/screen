#include "screen/ili9341/ili9341.h"
#include "stm32f4xx_hal.h"

// Déclaration des handles externes pour STM32CubeMX
extern SPI_HandleTypeDef hspi3;
extern DMA_HandleTypeDef hdma_spi3_tx;
#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#define BUFFER_WIDTH  40   // juste pour les chiffres
#define BUFFER_HEIGHT 16   // hauteur de la police
#define FONT_WIDTH  5
#define FONT_HEIGHT 8
// ---------- Frame buffer ----------
static uint16_t frame_buffer[BUFFER_HEIGHT][BUFFER_WIDTH];
// Flag pour savoir quand le DMA a fini
static volatile uint8_t spiDmaTransferComplete = 0;

// DMA callback
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI3) {
        spiDmaTransferComplete = 1;
    }
}

// Transmission SPI via DMA, bloquante
static void SPI_DMA_Transmit(uint8_t *data, uint16_t size)
{
    spiDmaTransferComplete = 0;
    if(HAL_SPI_Transmit_DMA(&hspi3, data, size) != HAL_OK){
        Error_Handler();
    }
    while(spiDmaTransferComplete == 0); // attendre la fin
}

// Commande LCD
static void LCD_WR_REG(uint8_t reg)
{
    HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_RESET);
    SPI_DMA_Transmit(&reg, 1);
}

static void LCD_WR_DATA(uint8_t data)
{
    HAL_GPIO_WritePin(DC_LCD_GPIO_Port, DC_LCD_Pin, GPIO_PIN_SET);
    SPI_DMA_Transmit(&data, 1);
}

// Reset LCD
void ILI9341_Reset(void)
{
    HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(RESET_LCD_GPIO_Port, RESET_LCD_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
}

// ---------- Pixel drawing ----------
static void ILI9341_WritePixelToBuffer(uint16_t x, uint16_t y, uint16_t color)
{
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    frame_buffer[y][x] = color;
}

// ---------- Optimized time update ----------
void ILI9341_ChangeTime(uint8_t hours, uint8_t minutes)
{
    char str[6];
    sprintf(str,"%02u:%02u", hours, minutes);

    // Ici, seulement redessiner les chiffres qui changent
    ILI9341_DrawString(20, 20, str, 0xFFFF, 0x0000);
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

// Initialisation minimale ILI9341
void ILI9341_Init(void)
{
    ILI9341_Reset();

    LCD_WR_REG(0x01); // Software reset
    HAL_Delay(100);

    LCD_WR_REG(0x11); // Sleep out
    HAL_Delay(120);

    LCD_WR_REG(0x3A); // Pixel format
    LCD_WR_DATA(0x55); // 16 bits

    LCD_WR_REG(0x29); // Display ON
    HAL_Delay(20);
}

// Définir une zone d’écriture
void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    LCD_WR_REG(0x2A); // Column
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    SPI_DMA_Transmit(data, 4);

    LCD_WR_REG(0x2B); // Page
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    SPI_DMA_Transmit(data, 4);

    LCD_WR_REG(0x2C); // Memory write
}

// Dessine un pixel
void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
    ILI9341_SetWindow(x, y, x, y);
    uint8_t data[2] = { color >> 8, color & 0xFF };
    SPI_DMA_Transmit(data, 2);
}

// Test : dessine un rectangle plein blanc
void ILI9341_TestScreen(void)
{
    for(uint16_t x=0; x<10; x++){
        for(uint16_t y=0; y<10; y++){
            ILI9341_WritePixel(x, y, 0xFFFF);
        }
    }
}
