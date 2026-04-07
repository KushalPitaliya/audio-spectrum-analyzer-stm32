#ifndef FFT_PROCESSOR_H
#define FFT_PROCESSOR_H

#include "main.h"
#include <stdint.h>
#include "arm_math.h"   /* Defines float32_t as 'float' — compatible with float* */

/* -----------------------------------------------------------------------
 * Configuration
 * ----------------------------------------------------------------------- */
#define FFT_SIZE            512     /* Must match MIC_BUFFER_SIZE */
#define FFT_BINS            (FFT_SIZE / 2)
#define SAMPLE_RATE         44100   /* Hz - must match I2S config */

/* -----------------------------------------------------------------------
 * FFT Buffers - accessible from application
 * ----------------------------------------------------------------------- */

/* Input buffer for FFT (interleaved real/imaginary: [real0, imag0, real1, imag1, ...]) */
extern float32_t fft_input[FFT_SIZE * 2];

/* Output magnitude spectrum (only first FFT_SIZE/2 bins are valid) */
extern float32_t fft_magnitude[FFT_BINS];

/* Pre-computed Hanning window coefficients */
extern float32_t hanning_window[FFT_SIZE];

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief Initialize FFT processor
 * @details Pre-computes Hanning window coefficients
 * @note Call once during system initialization
 */
void FFT_Init(void);

/**
 * @brief Process audio samples through FFT pipeline
 * @param audio_samples Pointer to int32_t audio buffer (FFT_SIZE samples)
 * @details Pipeline: Convert to float → Apply window → FFT → Calculate magnitude
 *          Result available in fft_magnitude[FFT_BINS]
 */
void FFT_Process(int32_t *audio_samples);

/**
 * @brief Convert int32_t audio samples to float and apply Hanning window
 * @param audio_samples Input audio buffer (int32_t)
 * @param output Windowed float output (must be size FFT_SIZE*2 for complex FFT)
 * @note Normalizes int32_t range to -1.0 to +1.0
 */
void FFT_PrepareInput(int32_t *audio_samples, float32_t *output);

/**
 * @brief Execute FFT transform
 * @param input Complex input buffer (interleaved real/imag)
 * @note Uses ARM CMSIS-DSP optimized FFT (arm_cfft_f32)
 */
void FFT_Transform(float32_t *input);

/**
 * @brief Calculate magnitude spectrum from complex FFT output
 * @param complex_output Complex FFT result (interleaved real/imag)
 * @param magnitude Output magnitude array (FFT_BINS elements)
 * @note Uses ARM CMSIS-DSP optimized magnitude (arm_cmplx_mag_f32)
 */
void FFT_CalculateMagnitude(float32_t *complex_output, float32_t *magnitude);

/**
 * @brief Get frequency in Hz for a given FFT bin
 * @param bin_index Bin number (0 to FFT_BINS-1)
 * @return Frequency in Hz
 */
static inline float FFT_BinToFrequency(uint16_t bin_index)
{
    return (float)bin_index * SAMPLE_RATE / FFT_SIZE;
}

/**
 * @brief Get FFT bin index for a given frequency
 * @param frequency Frequency in Hz
 * @return Bin index (0 to FFT_BINS-1)
 */
static inline uint16_t FFT_FrequencyToBin(float frequency)
{
    return (uint16_t)(frequency * FFT_SIZE / SAMPLE_RATE);
}

#endif /* FFT_PROCESSOR_H */
