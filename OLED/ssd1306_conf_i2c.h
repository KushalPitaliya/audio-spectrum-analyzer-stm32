/**
 * I2C Configuration for SSD1306 OLED Display
 * STM32F411CEU6 (Black Pill)
 *
 * HOW TO USE:
 *   1. Copy this file's contents into ssd1306_conf.h
 *      (replacing the SPI settings), OR
 *   2. Rename this file to ssd1306_conf.h after backing up the SPI version
 *
 * WIRING:
 *   SSD1306 SCL → PB8 (I2C1_SCL)
 *   SSD1306 SDA → PB9 (I2C1_SDA)
 *   SSD1306 VCC → 3.3V
 *   SSD1306 GND → GND
 */

#ifndef SSD1306_CONF_I2C_H
#define SSD1306_CONF_I2C_H

/* ============================================ */
/* MCU Family                                   */
/* ============================================ */
#define STM32F4

/* ============================================ */
/* I2C MODE CONFIGURATION (NOT SPI)            */
/* ============================================ */

/* Comment out SPI, enable I2C */
// #define SSD1306_USE_SPI
#define SSD1306_USE_I2C

/* ============================================ */
/* I2C SETTINGS                                */
/* ============================================ */

/* I2C Handle — must match CubeMX configuration */
#define SSD1306_I2C_PORT        hi2c2

/* I2C Address for SSD1306
 * 7-bit address = 0x3C (most common) or 0x3D
 * The HAL functions expect the 8-bit (left-shifted) format: 0x3C << 1 = 0x78
 * The ssd1306.h default shifts it for us, so we pass the shifted value. */
#define SSD1306_I2C_ADDR        (0x3C << 1)

/* ============================================ */
/* DISPLAY DIMENSIONS                          */
/* ============================================ */

#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

/* Uncomment if you have a 128x32 display */
// #define SSD1306_HEIGHT       32

/* ============================================ */
/* Fonts                                        */
/* ============================================ */
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26
#define SSD1306_INCLUDE_FONT_16x24
#define SSD1306_INCLUDE_FONT_16x15

/* ============================================ */
/* Mirror / Invert (uncomment if needed)        */
/* ============================================ */
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ
// #define SSD1306_INVERSE_COLOR

/* ============================================ */
/* OPTIONAL: DMA FOR I2C (ADVANCED)            */
/* ============================================ */

/* Uncomment to enable DMA for I2C transfers (faster updates) */
// #define SSD1306_USE_DMA

#endif /* SSD1306_CONF_I2C_H */
