/* TEMPORARY WORKAROUND: Basic FFT without CMSIS-DSP
 * 
 * This implements a simple FFT without requiring arm_math.h
 * 
 * IMPORTANT: This is SLOWER than CMSIS-DSP!
 * Use only while fixing CMSIS-DSP installation.
 * 
 * To use:
 * 1. Rename fft_processor.h → fft_processor_cmsis.h
 * 2. Rename fft_processor.c → fft_processor_cmsis.c
 * 3. Rename this file → fft_processor.c
 * 4. Rename fft_processor_simple.h → fft_processor.h
 */

#include "fft_processor.h"
#include <math.h>
#include <string.h>

/* Constants */
#define PI 3.14159265358979f
#define AUDIO_NORMALIZE_FACTOR (1.0f / 8388608.0f)  /* 2^23 for 24-bit */

/* Buffers */
float fft_input_real[FFT_SIZE];
float fft_input_imag[FFT_SIZE];
float fft_magnitude[FFT_BINS];
float hanning_window[FFT_SIZE];

/* -----------------------------------------------------------------------
 * FFT_Init
 * Pre-compute Hanning window
 * ----------------------------------------------------------------------- */
void FFT_Init(void)
{
    /* Pre-compute Hanning window */
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        hanning_window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / FFT_SIZE));
    }
    
    /* Clear buffers */
    memset(fft_input_real, 0, sizeof(fft_input_real));
    memset(fft_input_imag, 0, sizeof(fft_input_imag));
    memset(fft_magnitude, 0, sizeof(fft_magnitude));
}

/* -----------------------------------------------------------------------
 * FFT_Process
 * Complete FFT pipeline
 * ----------------------------------------------------------------------- */
void FFT_Process(int32_t *audio_samples)
{
    if (audio_samples == NULL) return;
    
    /* Convert to float and apply window */
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        float normalized = (float)audio_samples[i] * AUDIO_NORMALIZE_FACTOR;
        fft_input_real[i] = normalized * hanning_window[i];
        fft_input_imag[i] = 0.0f;
    }
    
    /* Perform FFT */
    FFT_Compute(fft_input_real, fft_input_imag, FFT_SIZE);
    
    /* Calculate magnitude */
    FFT_CalculateMagnitude();
}

/* -----------------------------------------------------------------------
 * FFT_Compute
 * Cooley-Tukey radix-2 FFT algorithm
 * ----------------------------------------------------------------------- */
void FFT_Compute(float *real, float *imag, int n)
{
    int i, j, k, step;
    float temp_real, temp_imag;
    float w_real, w_imag;
    float angle;
    
    /* Bit-reverse ordering */
    j = 0;
    for (i = 0; i < n - 1; i++)
    {
        if (i < j)
        {
            /* Swap real parts */
            temp_real = real[i];
            real[i] = real[j];
            real[j] = temp_real;
            
            /* Swap imaginary parts */
            temp_imag = imag[i];
            imag[i] = imag[j];
            imag[j] = temp_imag;
        }
        
        /* Bit-reverse increment */
        k = n >> 1;
        while (k <= j)
        {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
    
    /* FFT computation */
    for (step = 1; step < n; step <<= 1)
    {
        int step2 = step << 1;
        
        for (k = 0; k < step; k++)
        {
            /* Twiddle factor */
            angle = -PI * k / step;
            w_real = cosf(angle);
            w_imag = sinf(angle);
            
            for (i = k; i < n; i += step2)
            {
                j = i + step;
                
                /* Butterfly operation */
                temp_real = w_real * real[j] - w_imag * imag[j];
                temp_imag = w_real * imag[j] + w_imag * real[j];
                
                real[j] = real[i] - temp_real;
                imag[j] = imag[i] - temp_imag;
                
                real[i] += temp_real;
                imag[i] += temp_imag;
            }
        }
    }
}

/* -----------------------------------------------------------------------
 * FFT_CalculateMagnitude
 * Calculate magnitude spectrum
 * ----------------------------------------------------------------------- */
void FFT_CalculateMagnitude(void)
{
    for (uint16_t i = 0; i < FFT_BINS; i++)
    {
        float re = fft_input_real[i];
        float im = fft_input_imag[i];
        fft_magnitude[i] = sqrtf(re * re + im * im);
    }
}

/* -----------------------------------------------------------------------
 * NOTE: This implementation is about 10x slower than CMSIS-DSP
 * 
 * Expected performance:
 * - CMSIS-DSP: ~1.5ms per 512-point FFT
 * - This version: ~15ms per 512-point FFT
 * 
 * This will reduce frame rate from 30+ FPS to about 10-15 FPS.
 * 
 * FIX CMSIS-DSP INSTALLATION AS SOON AS POSSIBLE!
 * See: FIX_ARM_MATH_ERROR.md
 * ----------------------------------------------------------------------- */
