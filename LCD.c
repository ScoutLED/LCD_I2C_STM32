/**
  ******************************************************************************
  * @file           : LCD.c
  * @brief          : Driver untuk LCD 16x2 via I2C
  * @author         : Iwan Edogawa - ScoutLED
  * @date           : 2025
  * @target         : STM32C0 series
  ******************************************************************************
  */

#include "LCD.h"
#include <stm32c0xx_hal.h>
#include <string.h>
#include <stdlib.h>

/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef* hi2c_lcd = NULL;
static uint8_t lcd_addr = 0x27 << 1; // Default I2C address untuk PCF8574

/* Private function prototypes -----------------------------------------------*/
static void LCD_WriteNibble(uint8_t data, uint8_t rs);
static void LCD_WriteByte(uint8_t data, uint8_t rs);
static void LCD_WriteCommand(uint8_t cmd);
static void LCD_WriteData(uint8_t data);
static void LCD_PulseEnable(uint8_t data);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes LCD with associated I2C handle
  * @param  hi2c: Pointer to I2C handle
  * @retval LCD_StatusTypeDef: Status of initialization
  */
LCD_StatusTypeDef LCD_Init(I2C_HandleTypeDef* hi2c)
{
    if (hi2c == NULL) {
        return LCD_ERROR;
    }
    
    hi2c_lcd = hi2c;
    
    // Delay untuk inisialisasi LCD
    HAL_Delay(50);
    
    // Initial sequence untuk 4-bit mode
    LCD_WriteNibble(0x03 << 4, 0);  // Function set (8-bit)
    HAL_Delay(5);
    LCD_WriteNibble(0x03 << 4, 0);  // Function set (8-bit)
    HAL_Delay(1);
    LCD_WriteNibble(0x03 << 4, 0);  // Function set (8-bit)
    HAL_Delay(1);
    LCD_WriteNibble(0x02 << 4, 0);  // Function set (4-bit)
    HAL_Delay(1);
    
    // Function set: 4-bit, 2 lines, 5x8 font
    LCD_WriteCommand(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS);
    HAL_Delay(1);
    
    // Display control: Display off
    LCD_WriteCommand(LCD_DISPLAY_CONTROL);
    HAL_Delay(1);
    
    // Clear display
    LCD_WriteCommand(LCD_CLEAR_DISPLAY);
    HAL_Delay(2);
    
    // Entry mode set: Increment, no shift
    LCD_WriteCommand(LCD_ENTRY_MODE_SET | LCD_ENTRY_LEFT);
    HAL_Delay(1);
    
    // Display control: Display on, cursor off, blink off
    LCD_WriteCommand(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON);
    HAL_Delay(1);
    
    return LCD_OK;
}

/**
  * @brief  Clears LCD display
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Clear(void)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    LCD_WriteCommand(LCD_CLEAR_DISPLAY);
    HAL_Delay(2); // Clear command needs extra time
    return LCD_OK;
}

/**
  * @brief  Sets cursor position
  * @param  row: Row number (0-1)
  * @param  col: Column number (0-15)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_SetCursor(uint8_t row, uint8_t col)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    // Pastikan dalam batas
    if (row >= 4) row = 3;
    if (col >= 20) col = 19;
    
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};  // Untuk LCD 20x4
    LCD_WriteCommand(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
    
    return LCD_OK;
}

/**
  * @brief  Prints string to LCD
  * @param  str: Null-terminated string
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintString(const char* str)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    if (str == NULL) {
        return LCD_ERROR;
    }
    
    while (*str) {
        LCD_WriteData(*str++);
    }
    
    return LCD_OK;
}

/**
  * @brief  Prints integer to LCD
  * @param  num: Integer number
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintInt(int32_t num)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    char buffer[12];
    itoa(num, buffer, 10);
    return LCD_PrintString(buffer);
}

/**
  * @brief  Prints float to LCD with specified precision
  * @param  num: Float number
  * @param  decimals: Number of decimal places (0-6)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintFloat(float num, uint8_t decimals)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    if (decimals > 6) decimals = 6;
    
    char buffer[20];
    char format[10];
    
    sprintf(format, "%%.%df", decimals);
    sprintf(buffer, format, num);
    
    return LCD_PrintString(buffer);
}

/**
  * @brief  Creates custom character
  * @param  location: CGRAM location (0-7)
  * @param  charmap: 8-byte array for character pattern
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_CreateChar(uint8_t location, uint8_t charmap[])
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    if (location > 7) {
        return LCD_ERROR;
    }
    
    LCD_WriteCommand(LCD_SET_CGRAM_ADDR | (location << 3));
    
    for (int i = 0; i < 8; i++) {
        LCD_WriteData(charmap[i]);
    }
    
    return LCD_OK;
}

/**
  * @brief  Displays custom character
  * @param  location: CGRAM location (0-7)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_WriteChar(uint8_t location)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    if (location > 7) {
        return LCD_ERROR;
    }
    
    LCD_WriteData(location);
    return LCD_OK;
}

/**
  * @brief  Turns display on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Display(uint8_t state)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    if (state) {
        LCD_WriteCommand(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON);
    } else {
        LCD_WriteCommand(LCD_DISPLAY_CONTROL);
    }
    
    return LCD_OK;
}

/**
  * @brief  Turns cursor on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Cursor(uint8_t state)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    uint8_t command = LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON;
    
    if (state) {
        command |= LCD_CURSOR_ON;
    }
    
    LCD_WriteCommand(command);
    return LCD_OK;
}

/**
  * @brief  Turns cursor blink on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Blink(uint8_t state)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    uint8_t command = LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON;
    
    if (state) {
        command |= LCD_BLINK_ON;
    }
    
    LCD_WriteCommand(command);
    return LCD_OK;
}

/**
  * @brief  Sets LCD I2C address
  * @param  address: I2C address (shifted left by 1 bit)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_SetAddress(uint8_t address)
{
    lcd_addr = address;
    return LCD_OK;
}

/**
  * @brief  Scrolls display left
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_ScrollLeft(void)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    LCD_WriteCommand(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT);
    return LCD_OK;
}

/**
  * @brief  Scrolls display right
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_ScrollRight(void)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    LCD_WriteCommand(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT);
    return LCD_OK;
}

/**
  * @brief  Returns cursor to home position
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Home(void)
{
    if (hi2c_lcd == NULL) {
        return LCD_NOT_INITIALIZED;
    }
    
    LCD_WriteCommand(LCD_RETURN_HOME);
    HAL_Delay(2); // Home command needs extra time
    return LCD_OK;
}

/* Private helper functions --------------------------------------------------*/

/**
  * @brief  Writes nibble to LCD
  * @param  data: 4-bit data
  * @param  rs: Register select (0 for command, 1 for data)
  */
static void LCD_WriteNibble(uint8_t data, uint8_t rs)
{
    uint8_t packet = data;
    
    if (rs) {
        packet |= LCD_RS;
    }
    
    // Add backlight bit
    packet |= LCD_BACKLIGHT;
    
    // Set enable bit high
    packet |= LCD_EN;
    HAL_I2C_Master_Transmit(hi2c_lcd, lcd_addr, &packet, 1, 10);
    HAL_Delay(1);
    
    // Set enable bit low
    packet &= ~LCD_EN;
    HAL_I2C_Master_Transmit(hi2c_lcd, lcd_addr, &packet, 1, 10);
    HAL_Delay(1);
}

/**
  * @brief  Writes byte to LCD
  * @param  data: 8-bit data
  * @param  rs: Register select (0 for command, 1 for data)
  */
static void LCD_WriteByte(uint8_t data, uint8_t rs)
{
    // Send high nibble
    LCD_WriteNibble(data & 0xF0, rs);
    // Send low nibble
    LCD_WriteNibble((data << 4) & 0xF0, rs);
}

/**
  * @brief  Writes command to LCD
  * @param  cmd: Command byte
  */
static void LCD_WriteCommand(uint8_t cmd)
{
    LCD_WriteByte(cmd, 0);
}

/**
  * @brief  Writes data to LCD
  * @param  data: Data byte
  */
static void LCD_WriteData(uint8_t data)
{
    LCD_WriteByte(data, 1);
}

/**
  * @brief  Sends pulse to enable pin
  * @param  data: Data to send
  */
static void LCD_PulseEnable(uint8_t data)
{
    HAL_I2C_Master_Transmit(hi2c_lcd, lcd_addr, &data, 1, 10);
    HAL_Delay(1);
    
    data |= LCD_EN;
    HAL_I2C_Master_Transmit(hi2c_lcd, lcd_addr, &data, 1, 10);
    HAL_Delay(1);
    
    data &= ~LCD_EN;
    HAL_I2C_Master_Transmit(hi2c_lcd, lcd_addr, &data, 1, 10);
    HAL_Delay(1);
}