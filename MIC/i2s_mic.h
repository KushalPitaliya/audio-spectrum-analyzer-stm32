#ifndef I2S_MIC_H
#define I2S_MIC_H

#include "main.h"
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Configuration
 * ----------------------------------------------------------------------- */
#define MIC_BUFFER_SIZE     512     /* Total DMA buffer size (in 32-bit words).
                                     * Half-buffer = 256 samples processed per
                                     * callback. Adjust based on your RAM budget
                                     * and processing latency requirements.     */

/* Number of samples that make up one FFT frame.
 * Must equal MIC_BUFFER_SIZE so the full DMA buffer feeds one FFT.       */
#define MIC_SAMPLES_PER_FRAME  MIC_BUFFER_SIZE

/* -----------------------------------------------------------------------
 * Audio sample buffer — accessible from your application layer
 * ----------------------------------------------------------------------- */

/* Raw DMA buffer (32-bit words from I2S peripheral).
 * Each word contains: [31:8] = 24-bit audio, [7:0] = 0x00              */
extern uint32_t i2s_dma_buf[MIC_BUFFER_SIZE];

/* Processed audio: 24-bit value sign-extended into int32_t.
 * Full-size buffer that holds a complete FFT frame (512 samples).
 * Filled by ping-ponging the two DMA halves into each half.             */
extern int32_t  mic_audio_buf[MIC_BUFFER_SIZE];

/* Flag set by DMA callbacks — poll this in your main loop.
 * 0 = not ready, 1 = first half updated, 2 = second half updated.
 * A full FFT frame is ready when BOTH halves have been filled at
 * least once, but for continuous operation you can process after
 * each callback since the other half is always the previous data.       */
extern volatile uint8_t mic_data_ready;

/* -----------------------------------------------------------------------
 * API
 * ----------------------------------------------------------------------- */
void MIC_Init(I2S_HandleTypeDef *hi2s);
void MIC_Start(void);
void MIC_Stop(void);

/* Call these from HAL_I2S_RxHalfCpltCallback and HAL_I2S_RxCpltCallback */
void MIC_HalfTransferCallback(void);
void MIC_FullTransferCallback(void);

/* Extracts and sign-extends 24-bit audio from raw 32-bit I2S word       */
static inline int32_t MIC_ExtractSample(uint32_t raw_word)
{
    /* Audio occupies bits [31:8]. Shift right by 8 to align to LSB,
     * then sign-extend from bit 23 by casting to int32_t after
     * preserving the sign bit.                                           */
    int32_t sample = (int32_t)(raw_word) >> 8;
    return sample;
}

#endif /* I2S_MIC_H */