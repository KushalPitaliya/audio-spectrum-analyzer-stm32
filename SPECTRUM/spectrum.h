#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "main.h"
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Configuration
 * ----------------------------------------------------------------------- */
#ifndef NUM_BARS
#define NUM_BARS            8       /* Number of frequency bars on display */
#endif

#define DISPLAY_HEIGHT_PX   64      /* OLED display height in pixels */
#define BAR_MAX_HEIGHT      64      /* Maximum bar height in pixels */

/* -----------------------------------------------------------------------
 * Frequency Band Definitions (for 44.1 kHz sampling, 512-point FFT)
 * Logarithmic spacing for better musical representation
 * ----------------------------------------------------------------------- */
typedef struct {
    uint16_t bin_start;     /* Starting FFT bin */
    uint16_t bin_end;       /* Ending FFT bin */
    const char* label;      /* Frequency range label */
} FrequencyBand;

/* -----------------------------------------------------------------------
 * Bar Heights - accessible from application
 * ----------------------------------------------------------------------- */

/* Current bar heights (0-64 pixels) for display */
extern uint8_t bar_heights[NUM_BARS];

/* Peak hold values for visual effect (slowly decay) */
extern uint8_t bar_peaks[NUM_BARS];

/* Previous bar heights for smoothing */
extern float bar_heights_smooth[NUM_BARS];

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief Initialize spectrum analyzer
 * @details Clears bar heights and peak values
 * @note Call once during system initialization
 */
void SPECTRUM_Init(void);

/**
 * @brief Process FFT magnitude data and update bar heights
 * @param magnitude FFT magnitude array (256 bins) — uses float (float32_t)
 * @details Maps 256 FFT bins → 8 logarithmically-spaced bars
 *          Applies smoothing and peak detection
 */
void SPECTRUM_Update(float *magnitude);

/**
 * @brief Get current bar heights for display
 * @return Pointer to bar_heights[NUM_BARS] array
 */
uint8_t* SPECTRUM_GetBarHeights(void);

/**
 * @brief Get peak hold values for display
 * @return Pointer to bar_peaks[NUM_BARS] array
 */
uint8_t* SPECTRUM_GetPeaks(void);

/**
 * @brief Reset all bars and peaks to zero
 */
void SPECTRUM_Reset(void);

/**
 * @brief Get frequency band information
 * @param bar_index Bar number (0-7)
 * @return Pointer to FrequencyBand structure
 */
const FrequencyBand* SPECTRUM_GetBandInfo(uint8_t bar_index);

#endif /* SPECTRUM_H */
