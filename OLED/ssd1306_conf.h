/**
 * Private configuration file for the SSD1306 library.
 * Configured for STM32F411CEU6 (Black Pill), I2C mode using I2C2.
 *
 * WIRING (I2C):
 *   SSD1306 SCL → PB10 (I2C2_SCL)
 *   SSD1306 SDA → PB3  (I2C2_SDA)  — or PB9 depending on AF remap
 *   SSD1306 VCC → 3.3V
 *   SSD1306 GND → GND
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

/* ============================================ */
/* Choose a microcontroller family              */
/* ============================================ */
// #define STM32F0
// #define STM32F1
#define STM32F4
// #define STM32L0
// #define STM32L1
// #define STM32L4
// #define STM32F3
// #define STM32H7
// #define STM32F7
// #define STM32G0
// #define STM32C0
// #define STM32U5

/* ============================================ */
/* Choose a bus: I2C (active) or SPI            */
/* ============================================ */
#define SSD1306_USE_I2C
// #define SSD1306_USE_SPI

/* ============================================ */
/* I2C Configuration (active)                   */
/* ============================================ */
#define SSD1306_I2C_PORT        hi2c2
#define SSD1306_I2C_ADDR        (0x3C << 1)

/* ============================================ */
/* SPI Configuration (inactive — for reference) */
/* ============================================ */
// #define SSD1306_SPI_PORT        hspi1
// #define SSD1306_CS_Port         GPIOA
// #define SSD1306_CS_Pin          GPIO_PIN_15
// #define SSD1306_DC_Port         GPIOB
// #define SSD1306_DC_Pin          GPIO_PIN_3
// #define SSD1306_Reset_Port      GPIOB
// #define SSD1306_Reset_Pin       GPIO_PIN_4

/* ============================================ */
/* Display Dimensions                           */
/* ============================================ */
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

/* ============================================ */
/* Mirror / Invert (uncomment if needed)        */
/* ============================================ */
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ
// #define SSD1306_INVERSE_COLOR

/* ============================================ */
/* Fonts — include only what you need           */
/* ============================================ */
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26
#define SSD1306_INCLUDE_FONT_16x24
#define SSD1306_INCLUDE_FONT_16x15

#endif /* __SSD1306_CONF_H__ */
