#include "fft_processor.h"
#include <math.h>
#include <string.h>

/* -----------------------------------------------------------------------
 * Private Constants
 * ----------------------------------------------------------------------- */
#define AUDIO_NORMALIZE_FACTOR  (1.0f / 8388608.0f)  /* 2^23 for 24-bit audio */
#define PI                      3.14159265358979f

/* -----------------------------------------------------------------------
 * FFT Buffers
 * ----------------------------------------------------------------------- */
float32_t fft_input[FFT_SIZE * 2];      /* Complex buffer (real, imag pairs) */
float32_t fft_magnitude[FFT_BINS];      /* Magnitude spectrum output */
float32_t hanning_window[FFT_SIZE];     /* Pre-computed window coefficients */

/* -----------------------------------------------------------------------
 * CMSIS-DSP FFT Instance
 * ----------------------------------------------------------------------- */
static arm_cfft_instance_f32 fft_instance;

/* -----------------------------------------------------------------------
 * FFT_Init
 * Pre-compute Hanning window and initialize FFT instance
 * ----------------------------------------------------------------------- */
void FFT_Init(void)
{
    /* Initialize CMSIS-DSP FFT instance for 512-point complex FFT */
    arm_cfft_init_f32(&fft_instance, FFT_SIZE);
    
    /* Pre-compute Hanning window: w[n] = 0.5 * (1 - cos(2*pi*n/N)) */
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        hanning_window[i] = 0.5f * (1.0f - arm_cos_f32(2.0f * PI * i / FFT_SIZE));
    }
    
    /* Clear buffers */
    memset(fft_input, 0, sizeof(fft_input));
    memset(fft_magnitude, 0, sizeof(fft_magnitude));
}

/* -----------------------------------------------------------------------
 * FFT_Process
 * Complete FFT pipeline: prepare → transform → magnitude
 * ----------------------------------------------------------------------- */
void FFT_Process(int32_t *audio_samples)
{
    if (audio_samples == NULL) return;
    
    /* Step 1: Convert to float and apply Hanning window */
    FFT_PrepareInput(audio_samples, fft_input);
    
    /* Step 2: Execute FFT */
    FFT_Transform(fft_input);
    
    /* Step 3: Calculate magnitude spectrum */
    FFT_CalculateMagnitude(fft_input, fft_magnitude);
}

/* -----------------------------------------------------------------------
 * FFT_PrepareInput
 * Convert int32_t → float, normalize, apply window, create complex buffer
 * ----------------------------------------------------------------------- */
void FFT_PrepareInput(int32_t *audio_samples, float32_t *output)
{
    /* Complex FFT requires interleaved [real, imag, real, imag, ...] format
     * For real-only input, imaginary parts are zero */
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        /* Normalize 24-bit signed audio to -1.0 to +1.0 range */
        float32_t normalized = (float32_t)audio_samples[i] * AUDIO_NORMALIZE_FACTOR;
        
        /* Apply Hanning window to reduce spectral leakage */
        float32_t windowed = normalized * hanning_window[i];
        
        /* Interleave: real at even index, imaginary (0) at odd index */
        output[2 * i]     = windowed;  /* Real part */
        output[2 * i + 1] = 0.0f;      /* Imaginary part = 0 */
    }
}

/* -----------------------------------------------------------------------
 * FFT_Transform
 * Execute CMSIS-DSP optimized FFT (in-place)
 * ----------------------------------------------------------------------- */
void FFT_Transform(float32_t *input)
{
    /* arm_cfft_f32: Complex FFT
     * Parameters:
     *   - Instance: pre-initialized FFT configuration
     *   - Input: complex buffer [real, imag, ...] (modified in-place)
     *   - ifftFlag: 0 = forward FFT, 1 = inverse FFT
     *   - bitReverseFlag: 1 = apply bit-reversal (required for correct output)
     */
    arm_cfft_f32(&fft_instance, input, 0, 1);
}

/* -----------------------------------------------------------------------
 * FFT_CalculateMagnitude
 * Convert complex FFT output to magnitude spectrum
 * ----------------------------------------------------------------------- */
void FFT_CalculateMagnitude(float32_t *complex_output, float32_t *magnitude)
{
    /* arm_cmplx_mag_f32: Calculate magnitude from complex pairs
     * magnitude[i] = sqrt(real[i]^2 + imag[i]^2)
     * 
     * Parameters:
     *   - complex_output: interleaved [real, imag, ...] (size = FFT_SIZE * 2)
     *   - magnitude: output array (size = FFT_BINS = FFT_SIZE / 2)
     *   - FFT_BINS: number of output magnitude values
     * 
     * Note: Only first FFT_SIZE/2 bins are useful (Nyquist limit)
     */
    arm_cmplx_mag_f32(complex_output, magnitude, FFT_BINS);
}
