/* TEMPORARY WORKAROUND: Basic FFT without CMSIS-DSP
 * 
 * This is a simplified version that uses basic math library instead of arm_math.h
 * 
 * IMPORTANT: This is SLOWER than CMSIS-DSP version!
 * Use this ONLY while troubleshooting CMSIS-DSP installation.
 * 
 * Performance: ~10-15ms per FFT (vs 1.5ms with CMSIS-DSP)
 * 
 * Replace fft_processor.h with this file temporarily.
 */

#ifndef FFT_PROCESSOR_SIMPLE_H
#define FFT_PROCESSOR_SIMPLE_H

#include "main.h"
#include <stdint.h>
#include <math.h>

/* -----------------------------------------------------------------------
 * Configuration
 * ----------------------------------------------------------------------- */
#define FFT_SIZE            512
#define FFT_BINS            (FFT_SIZE / 2)
#define SAMPLE_RATE         44100

/* -----------------------------------------------------------------------
 * FFT Buffers
 * ----------------------------------------------------------------------- */
extern float fft_input_real[FFT_SIZE];
extern float fft_input_imag[FFT_SIZE];
extern float fft_magnitude[FFT_BINS];
extern float hanning_window[FFT_SIZE];

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief Initialize FFT processor (pre-compute Hanning window)
 */
void FFT_Init(void);

/**
 * @brief Process audio samples through FFT
 * @param audio_samples Input audio buffer (int32_t)
 */
void FFT_Process(int32_t *audio_samples);

/**
 * @brief Simple FFT implementation (Cooley-Tukey radix-2)
 * @param real Real part array
 * @param imag Imaginary part array
 * @param n FFT size (must be power of 2)
 */
void FFT_Compute(float *real, float *imag, int n);

/**
 * @brief Calculate magnitude from complex values
 */
void FFT_CalculateMagnitude(void);

/**
 * @brief Get frequency for bin index
 */
static inline float FFT_BinToFrequency(uint16_t bin_index)
{
    return (float)bin_index * SAMPLE_RATE / FFT_SIZE;
}

/**
 * @brief Get bin index for frequency
 */
static inline uint16_t FFT_FrequencyToBin(float frequency)
{
    return (uint16_t)(frequency * FFT_SIZE / SAMPLE_RATE);
}

#endif /* FFT_PROCESSOR_SIMPLE_H */
