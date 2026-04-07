#ifndef I2S_MIC_H
#define I2S_MIC_H

#include "main.h"
#include <stdint.h>
#include "audio_config.h"   /* Shared AUDIO_FRAME_SIZE — single source of truth */

/* -----------------------------------------------------------------------
 * Configuration — derived from audio_config.h
 * DO NOT redefine these here; change AUDIO_FRAME_SIZE in audio_config.h
 * ----------------------------------------------------------------------- */
#define MIC_BUFFER_SIZE        AUDIO_FRAME_SIZE        /* Total DMA buffer (32-bit words)  */
#define MIC_SAMPLES_PER_FRAME  AUDIO_FRAME_SIZE        /* Samples handed to FFT per frame  */
#define MIC_HALF_SIZE          (MIC_BUFFER_SIZE / 2)   /* DMA interrupt stride             */

/* Compile-time contract: enforce power-of-2 and positivity */
_AUDIO_CONFIG_STATIC_CHECK;

/* -----------------------------------------------------------------------
 * DC offset removal
 * The INMP441 outputs a small DC bias that shows up as a large bin-0
 * spike in the FFT if not removed. We use a leaky integrator (IIR
 * high-pass) with a very long time constant to estimate and subtract it.
 *
 * Shift = 10  →  time constant ≈ 1024 samples ≈ 23 ms @ 44.1 kHz
 * Set MIC_DC_FILTER_SHIFT to 0 to disable DC removal.
 * ----------------------------------------------------------------------- */
#define MIC_DC_FILTER_SHIFT    10   /* IIR time constant: 2^10 = 1024 samples */

/* -----------------------------------------------------------------------
 * Audio sample buffers — accessible from application layer
 * ----------------------------------------------------------------------- */

/* Raw DMA buffer (32-bit words, aligned for DMA efficiency)             */
extern uint32_t i2s_dma_buf[MIC_BUFFER_SIZE];

/* DC-corrected, sign-extended int32_t samples ready for FFT.
 * Full 512-element frame — always holds the last complete frame.        */
extern int32_t  mic_audio_buf[MIC_BUFFER_SIZE];

/* Flag: 0 = idle, 1 = first half updated, 2 = second half updated.
 * Written in ISR, read in main loop. Declared volatile for correct
 * visibility; clear it BEFORE calling FFT_Process() to avoid missing
 * the next interrupt.                                                   */
extern volatile uint8_t mic_data_ready;

/* -----------------------------------------------------------------------
 * API
 * ----------------------------------------------------------------------- */
void MIC_Init(I2S_HandleTypeDef *hi2s);
void MIC_Start(void);
void MIC_Stop(void);

/* Call from HAL_I2S_RxHalfCpltCallback / HAL_I2S_RxCpltCallback       */
void MIC_HalfTransferCallback(void);
void MIC_FullTransferCallback(void);

/* -----------------------------------------------------------------------
 * MIC_ExtractSample (inline)
 * Sign-extend the 24-bit audio value packed in bits [31:8] of the
 * 32-bit I2S word.
 * ----------------------------------------------------------------------- */
static inline int32_t MIC_ExtractSample(uint32_t raw_word)
{
    /* Arithmetic right-shift preserves the sign bit */
    return (int32_t)(raw_word) >> 8;
}

#endif /* I2S_MIC_H */