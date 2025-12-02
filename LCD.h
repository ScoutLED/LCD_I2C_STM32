/**
  ******************************************************************************
  * @file           : LCD.h
  * @brief          : Header file untuk LCD 16x2 via I2C
  * @author         : Iwan Edogawa - ScoutLED
  * @date           : 2025
  * @target         : STM32C0 series
  ******************************************************************************
  */

#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stm32c0xx_hal.h>

/* Public defines ------------------------------------------------------------*/

// LCD commands
#define LCD_CLEAR_DISPLAY       0x01
#define LCD_RETURN_HOME         0x02
#define LCD_ENTRY_MODE_SET      0x04
#define LCD_DISPLAY_CONTROL     0x08
#define LCD_CURSOR_SHIFT        0x10
#define LCD_FUNCTION_SET        0x20
#define LCD_SET_CGRAM_ADDR      0x40
#define LCD_SET_DDRAM_ADDR      0x80

// Flags for display entry mode
#define LCD_ENTRY_RIGHT         0x00
#define LCD_ENTRY_LEFT          0x02
#define LCD_ENTRY_SHIFT_INC     0x01
#define LCD_ENTRY_SHIFT_DEC     0x00

// Flags for display on/off control
#define LCD_DISPLAY_ON          0x04
#define LCD_DISPLAY_OFF         0x00
#define LCD_CURSOR_ON           0x02
#define LCD_CURSOR_OFF          0x00
#define LCD_BLINK_ON            0x01
#define LCD_BLINK_OFF           0x00

// Flags for display/cursor shift
#define LCD_DISPLAY_MOVE        0x08
#define LCD_CURSOR_MOVE         0x00
#define LCD_MOVE_RIGHT          0x04
#define LCD_MOVE_LEFT           0x00

// Flags for function set
#define LCD_8BIT_MODE           0x10
#define LCD_4BIT_MODE           0x00
#define LCD_2LINE               0x08
#define LCD_4LINE               0x09 
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

// Control pins pada PCF8574
#define LCD_RS                  0x01  // Register Select
#define LCD_RW                  0x02  // Read/Write
#define LCD_EN                  0x04  // Enable
#define LCD_BACKLIGHT           0x08  // Backlight

/* Public types --------------------------------------------------------------*/
typedef enum {
    LCD_OK = 0,
    LCD_ERROR,
    LCD_NOT_INITIALIZED,
    LCD_BUSY,
    LCD_TIMEOUT
} LCD_StatusTypeDef;

/* Public function prototypes ------------------------------------------------*/

/**
  * @brief  Initializes LCD with associated I2C handle
  * @param  hi2c: Pointer to I2C handle
  * @retval LCD_StatusTypeDef: Status of initialization
  */
LCD_StatusTypeDef LCD_Init(I2C_HandleTypeDef* hi2c);

/**
  * @brief  Clears LCD display
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Clear(void);

/**
  * @brief  Sets cursor position
  * @param  row: Row number (0-1)
  * @param  col: Column number (0-15)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_SetCursor(uint8_t row, uint8_t col);

/**
  * @brief  Prints string to LCD
  * @param  str: Null-terminated string
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintString(const char* str);

/**
  * @brief  Prints integer to LCD
  * @param  num: Integer number
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintInt(int32_t num);

/**
  * @brief  Prints float to LCD with specified precision
  * @param  num: Float number
  * @param  decimals: Number of decimal places (0-6)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_PrintFloat(float num, uint8_t decimals);

/**
  * @brief  Creates custom character
  * @param  location: CGRAM location (0-7)
  * @param  charmap: 8-byte array for character pattern
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_CreateChar(uint8_t location, uint8_t charmap[]);

/**
  * @brief  Displays custom character
  * @param  location: CGRAM location (0-7)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_WriteChar(uint8_t location);

/**
  * @brief  Turns display on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Display(uint8_t state);

/**
  * @brief  Turns cursor on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Cursor(uint8_t state);

/**
  * @brief  Turns cursor blink on/off
  * @param  state: 1 for on, 0 for off
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Blink(uint8_t state);

/**
  * @brief  Sets LCD I2C address
  * @param  address: I2C address (shifted left by 1 bit)
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_SetAddress(uint8_t address);

/**
  * @brief  Scrolls display left
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_ScrollLeft(void);

/**
  * @brief  Scrolls display right
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_ScrollRight(void);

/**
  * @brief  Returns cursor to home position
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Home(void);

/**
  * @brief  Prints formatted string (sprintf style)
  * @param  format: Format string
  * @param  ...: Variable arguments
  * @retval LCD_StatusTypeDef: Status of operation
  */
LCD_StatusTypeDef LCD_Printf(const char* format, ...);

/* Public macros -------------------------------------------------------------*/
/**
  * @brief  Prints string at specific position
  * @param  row: Row (0-1)
  * @param  col: Column (0-15)
  * @param  str: String to print
  */
#define LCD_PRINT_AT(row, col, str) do { \
    LCD_SetCursor(row, col); \
    LCD_PrintString(str); \
} while(0)

/**
  * @brief  Prints integer at specific position
  * @param  row: Row (0-1)
  * @param  col: Column (0-15)
  * @param  num: Integer to print
  */
#define LCD_PRINT_INT_AT(row, col, num) do { \
    LCD_SetCursor(row, col); \
    LCD_PrintInt(num); \
} while(0)

/**
  * @brief  Prints float at specific position
  * @param  row: Row (0-1)
  * @param  col: Column (0-15)
  * @param  num: Float to print
  * @param  dec: Decimal places
  */
#define LCD_PRINT_FLOAT_AT(row, col, num, dec) do { \
    LCD_SetCursor(row, col); \
    LCD_PrintFloat(num, dec); \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */