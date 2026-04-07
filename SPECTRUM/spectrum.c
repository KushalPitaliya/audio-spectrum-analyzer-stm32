#include "spectrum.h"
#include <string.h>
#include <math.h>

/* -----------------------------------------------------------------------
 * Private Constants
 * ----------------------------------------------------------------------- */
#define SMOOTHING_FACTOR    0.7f    /* 0.7 = heavy smoothing, 0.0 = no smoothing */
#define PEAK_DECAY_RATE     0.95f   /* Peak falls at 5% per frame */
#define SCALE_FACTOR        3.5f    /* Adjust for appropriate bar heights */
#define MIN_THRESHOLD       0.01f   /* Ignore very low noise */

/* -----------------------------------------------------------------------
 * Frequency Band Mapping (44.1 kHz sample rate, 512-point FFT)
 * Frequency resolution: 44100 / 512 = 86.13 Hz per bin
 * 
 * Logarithmic spacing for musical perception:
 * - Bass frequencies get more bars (we're more sensitive)
 * - Treble frequencies compressed (less perceptually important)
 * ----------------------------------------------------------------------- */
static const FrequencyBand frequency_bands[NUM_BARS] = {
    {1,   2,   "86-172 Hz (Sub-bass)"},      /* Bar 0: Deep bass */
    {3,   5,   "258-431 Hz (Bass)"},         /* Bar 1: Bass */
    {6,   10,  "517-861 Hz (Low-mid)"},      /* Bar 2: Low midrange */
    {11,  18,  "947-1550 Hz (Mid)"},         /* Bar 3: Midrange */
    {19,  30,  "1636-2584 Hz (Upper-mid)"}, /* Bar 4: Upper midrange */
    {31,  50,  "2670-4307 Hz (Presence)"},   /* Bar 5: Presence */
    {51,  80,  "4393-6890 Hz (Brilliance)"}, /* Bar 6: High frequencies */
    {81,  120, "6977-10336 Hz (Air)"}        /* Bar 7: Very high frequencies */
};

/* -----------------------------------------------------------------------
 * Bar Data
 * ----------------------------------------------------------------------- */
uint8_t bar_heights[NUM_BARS];              /* Current display heights (0-64) */
uint8_t bar_peaks[NUM_BARS];                /* Peak hold values (0-64) */
float bar_heights_smooth[NUM_BARS];         /* Smoothed float values */

/* -----------------------------------------------------------------------
 * SPECTRUM_Init
 * Initialize all bars to zero
 * ----------------------------------------------------------------------- */
void SPECTRUM_Init(void)
{
    SPECTRUM_Reset();
}

/* -----------------------------------------------------------------------
 * SPECTRUM_Reset
 * Clear all bar data
 * ----------------------------------------------------------------------- */
void SPECTRUM_Reset(void)
{
    memset(bar_heights, 0, sizeof(bar_heights));
    memset(bar_peaks, 0, sizeof(bar_peaks));
    memset(bar_heights_smooth, 0, sizeof(bar_heights_smooth));
}

/* -----------------------------------------------------------------------
 * SPECTRUM_Update
 * Process FFT magnitude → frequency bands → bar heights
 * ----------------------------------------------------------------------- */
void SPECTRUM_Update(float *magnitude)
{
    if (magnitude == NULL) return;
    
    /* Process each frequency bar */
    for (uint8_t bar = 0; bar < NUM_BARS; bar++)
    {
        /* Sum magnitude values in this frequency band */
        float sum = 0.0f;
        uint16_t bin_count = 0;
        
        for (uint16_t bin = frequency_bands[bar].bin_start; 
             bin <= frequency_bands[bar].bin_end; 
             bin++)
        {
            sum += magnitude[bin];
            bin_count++;
        }
        
        /* Average the magnitude values */
        float avg_magnitude = sum / bin_count;
        
        /* Apply logarithmic scaling for better visual range
         * log10(x + 1) smoothly maps [0, inf) → [0, inf)
         * Multiply by scale factor to reach display range */
        float scaled = log10f(avg_magnitude + 1.0f) * SCALE_FACTOR;
        
        /* Apply noise gate */
        if (scaled < MIN_THRESHOLD) {
            scaled = 0.0f;
        }
        
        /* Smooth the bar movement (reduces jitter) */
        bar_heights_smooth[bar] = (SMOOTHING_FACTOR * bar_heights_smooth[bar]) + 
                                  ((1.0f - SMOOTHING_FACTOR) * scaled);
        
        /* Convert to pixel height (0-64 range) */
        uint8_t pixel_height = (uint8_t)(bar_heights_smooth[bar]);
        
        /* Clamp to display range */
        if (pixel_height > BAR_MAX_HEIGHT) {
            pixel_height = BAR_MAX_HEIGHT;
        }
        
        bar_heights[bar] = pixel_height;
        
        /* ---------------------------------------------------------------
         * Peak Hold Effect
         * Remember the maximum value, decay slowly over time
         * --------------------------------------------------------------- */
        if (pixel_height > bar_peaks[bar]) {
            /* New peak! */
            bar_peaks[bar] = pixel_height;
        } else {
            /* Decay existing peak */
            bar_peaks[bar] = (uint8_t)(bar_peaks[bar] * PEAK_DECAY_RATE);
        }
    }
}

/* -----------------------------------------------------------------------
 * SPECTRUM_GetBarHeights
 * Return pointer to bar heights array
 * ----------------------------------------------------------------------- */
uint8_t* SPECTRUM_GetBarHeights(void)
{
    return bar_heights;
}

/* -----------------------------------------------------------------------
 * SPECTRUM_GetPeaks
 * Return pointer to peak values array
 * ----------------------------------------------------------------------- */
uint8_t* SPECTRUM_GetPeaks(void)
{
    return bar_peaks;
}

/* -----------------------------------------------------------------------
 * SPECTRUM_GetBandInfo
 * Get frequency range information for a bar
 * ----------------------------------------------------------------------- */
const FrequencyBand* SPECTRUM_GetBandInfo(uint8_t bar_index)
{
    if (bar_index >= NUM_BARS) return NULL;
    return &frequency_bands[bar_index];
}
