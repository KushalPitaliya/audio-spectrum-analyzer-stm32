#ifndef SPECTRUM_DISPLAY_H
#define SPECTRUM_DISPLAY_H

#include "main.h"
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Display Configuration
 * ----------------------------------------------------------------------- */
#define DISPLAY_WIDTH       128     /* SSD1306 width */
#define DISPLAY_HEIGHT      64      /* SSD1306 height */

#ifndef NUM_BARS
#define NUM_BARS            8       /* Number of spectrum bars */
#endif

#define BAR_WIDTH           14      /* Each bar width in pixels */
#define BAR_SPACING         2       /* Gap between bars */
#define PEAK_THICKNESS      2       /* Peak indicator line thickness */

/* -----------------------------------------------------------------------
 * Visual Effects
 * ----------------------------------------------------------------------- */
typedef enum {
    STYLE_SOLID_BARS,       /* Simple filled rectangles */
    STYLE_OUTLINED_BARS,    /* Bars with outline */
    STYLE_WITH_PEAKS,       /* Bars + peak hold indicators */
    STYLE_DOTTED           /* Dotted pattern for retro look */
} BarStyle;

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief Initialize spectrum display
 * @details Initializes SSD1306 display, clears screen
 * @note Call after SSD1306_Init()
 */
void DISPLAY_Init(void);

/**
 * @brief Draw spectrum analyzer bars on OLED
 * @param bar_heights Array of bar heights [0-64] for 8 bars
 * @param peaks Array of peak values [0-64] (can be NULL if no peaks)
 * @param style Visual style for rendering bars
 */
void DISPLAY_DrawSpectrum(uint8_t *bar_heights, uint8_t *peaks, BarStyle style);

/**
 * @brief Draw simple bar graph (fastest rendering)
 * @param bar_heights Array of 8 bar heights (0-64 pixels)
 */
void DISPLAY_DrawBars(uint8_t *bar_heights);

/**
 * @brief Draw bars with peak hold indicators
 * @param bar_heights Array of 8 bar heights (0-64 pixels)
 * @param peaks Array of 8 peak values (0-64 pixels)
 */
void DISPLAY_DrawBarsWithPeaks(uint8_t *bar_heights, uint8_t *peaks);

/**
 * @brief Clear display and show "Audio Spectrum" title
 */
void DISPLAY_ShowTitle(void);

/**
 * @brief Display "No Audio" message
 */
void DISPLAY_ShowNoSignal(void);

/**
 * @brief Show FPS counter on display (for debugging)
 * @param fps Current frame rate
 */
void DISPLAY_ShowFPS(uint16_t fps);

#endif /* SPECTRUM_DISPLAY_H */
