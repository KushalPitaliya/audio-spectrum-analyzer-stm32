#include "i2s_mic.h"
#include <string.h>

/* -----------------------------------------------------------------------
 * Buffer definitions
 * ----------------------------------------------------------------------- */

/* DMA writes 32-bit words here in circular mode.
 * HAL I2S DMA uses uint16_t internally, so size = MIC_BUFFER_SIZE * 2
 * halfwords. We cast to uint32_t for convenient sample extraction.
 * Aligned to 4 bytes for DMA efficiency.                                */
uint32_t i2s_dma_buf[MIC_BUFFER_SIZE] __attribute__((aligned(4)));

/* Processed, application-ready audio samples.
 * Full 512-sample frame: first 256 from one DMA half, second 256 from
 * the other. This way FFT_Process() always has a valid 512-sample
 * buffer to read.                                                       */
int32_t  mic_audio_buf[MIC_BUFFER_SIZE];

/* Data-ready flag: 1 = first half ready, 2 = second half ready          */
volatile uint8_t mic_data_ready = 0;

/* -----------------------------------------------------------------------
 * Private handle storage
 * ----------------------------------------------------------------------- */
static I2S_HandleTypeDef *_hi2s = NULL;

/* -----------------------------------------------------------------------
 * MIC_Init
 * Stores the I2S handle. Call AFTER MX_I2S2_Init() in main.c           */
void MIC_Init(I2S_HandleTypeDef *hi2s)
{
    _hi2s = hi2s;
    mic_data_ready = 0;
    memset(i2s_dma_buf,   0, sizeof(i2s_dma_buf));
    memset(mic_audio_buf, 0, sizeof(mic_audio_buf));
}

/* -----------------------------------------------------------------------
 * MIC_Start
 * Begins circular DMA reception. Call once after MIC_Init().
 *
 * HAL_I2S_Receive_DMA expects:
 *   pData : pointer to uint16_t buffer
 *   Size  : number of HALF-WORDS (uint16_t) to receive
 *
 * Since each I2S 32-bit word = 2 half-words, Size = MIC_BUFFER_SIZE * 2 */
void MIC_Start(void)
{
    if (_hi2s == NULL) return;

    HAL_I2S_Receive_DMA(_hi2s,
                        (uint16_t *)i2s_dma_buf,
                        MIC_BUFFER_SIZE * 2);  /* Size in half-words */
}

/* -----------------------------------------------------------------------
 * MIC_Stop
 * Halts DMA transfer cleanly                                            */
void MIC_Stop(void)
{
    if (_hi2s == NULL) return;
    HAL_I2S_DMAStop(_hi2s);
}

/* -----------------------------------------------------------------------
 * MIC_HalfTransferCallback
 * Called when DMA fills the FIRST half of i2s_dma_buf[0 .. N/2-1].
 * Extract into the FIRST half of mic_audio_buf[0 .. N/2-1].
 * The second half retains the previous DMA cycle's data, so the
 * full 512-sample buffer is always a valid, overlapping frame.          */
void MIC_HalfTransferCallback(void)
{
    /* Extract 24-bit signed audio from the first half of the DMA buffer */
    for (uint32_t i = 0; i < MIC_BUFFER_SIZE / 2; i++)
    {
        mic_audio_buf[i] = MIC_ExtractSample(i2s_dma_buf[i]);
    }

    mic_data_ready = 1;  /* Signal to main loop: first half updated      */
}

/* -----------------------------------------------------------------------
 * MIC_FullTransferCallback
 * Called when DMA fills the SECOND half of i2s_dma_buf[N/2 .. N-1].
 * Extract into the SECOND half of mic_audio_buf[N/2 .. N-1].
 * Now both halves are fresh and the 512-sample frame is complete.       */
void MIC_FullTransferCallback(void)
{
    /* Extract 24-bit signed audio from the second half of DMA buffer    */
    uint32_t offset = MIC_BUFFER_SIZE / 2;
    for (uint32_t i = 0; i < MIC_BUFFER_SIZE / 2; i++)
    {
        mic_audio_buf[offset + i] = MIC_ExtractSample(i2s_dma_buf[offset + i]);
    }

    mic_data_ready = 2;  /* Signal to main loop: second half updated     */
}