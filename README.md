**1. Overview**
A lightweight I2C LCD library for STM32 microcontrollers, compatible with HD44780-based LCD displays (16x2, 20x4, etc.) with PCF8574 I2C backpack.

**2. Configure I2C in CubeMX:**
Enable I2C1 (or other I2C instance)
Set I2C speed to 100kHz (Standard Mode)
Configure GPIO pins as Alternate Function Open-Drain
Enable I2C interrupt if using DMA (optional)

**3. Quick Start**


#include "main.h"
#include "LCD.h"

I2C_HandleTypeDef hi2c1;  // I2C handle

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    
    // Initialize I2C
    MX_I2C1_Init();
    
    // Initialize LCD (default: 16x2)
    if (LCD_Init(&hi2c1) == LCD_OK) {
        printf("LCD initialized successfully!\r\n");
    } else {
        printf("LCD initialization failed!\r\n");
    }
    
    // Basic operations
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_PrintString("Hello, STM32!");
    LCD_SetCursor(1, 0);
    LCD_PrintString("LCD Library v1.0");
    
    while (1) {
        // Your main code here
        HAL_Delay(100);
    }
}
