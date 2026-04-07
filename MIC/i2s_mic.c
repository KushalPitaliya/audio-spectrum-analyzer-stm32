#include "i2s_mic.h"
#include <string.h>

/* -----------------------------------------------------------------------
 * Buffers
 * ----------------------------------------------------------------------- */
uint32_t i2s_dma_buf[MIC_BUFFER_SIZE] __attribute__((aligned(4)));
int32_t  mic_audio_buf[MIC_BUFFER_SIZE];
volatile uint8_t mic_data_ready = 0;

/* -----------------------------------------------------------------------
 * Private state
 * ----------------------------------------------------------------------- */
static I2S_HandleTypeDef *_hi2s = NULL;

/* DC offset estimator — leaky integrator, updated per sample.
 * Tracks the long-term mean of the raw signal so it can be subtracted,
 * eliminating the INMP441's DC bias and preventing a large bin-0 FFT
 * spike that would otherwise push bar-0 permanently high.              */
static int32_t _dc_estimate = 0;

/* -----------------------------------------------------------------------
 * Private helper — process one DMA half into mic_audio_buf
 * src_offset : first index in i2s_dma_buf to read
 * dst_offset : first index in mic_audio_buf to write
 * ----------------------------------------------------------------------- */
static void _process_half(uint32_t src_offset, uint32_t dst_offset)
{
    for (uint32_t i = 0; i < MIC_HALF_SIZE; i++)
    {
        /* 1. Extract 24-bit signed sample */
        int32_t raw = MIC_ExtractSample(i2s_dma_buf[src_offset + i]);

#if MIC_DC_FILTER_SHIFT > 0
        /* 2. DC removal — leaky integrator:
         *    estimate += (raw - estimate) >> SHIFT
         * This is a first-order IIR high-pass with fc ≈ fs / (2π * 2^SHIFT)
         * At 44.1 kHz and shift=10: fc ≈ 6.8 Hz  (inaudible)            */
        _dc_estimate += (raw - _dc_estimate) >> MIC_DC_FILTER_SHIFT;
        raw -= _dc_estimate;
#endif

        /* 3. Soft clip: clamp to 24-bit range to prevent overflow in FFT  */
        if      (raw >  8388607)  raw =  8388607;   /*  2^23 - 1 */
        else if (raw < -8388608)  raw = -8388608;   /* -2^23     */

        mic_audio_buf[dst_offset + i] = raw;
    }
}

/* -----------------------------------------------------------------------
 * MIC_Init
 * ----------------------------------------------------------------------- */
void MIC_Init(I2S_HandleTypeDef *hi2s)
{
    _hi2s = hi2s;
    _dc_estimate = 0;
    mic_data_ready = 0;
    memset(i2s_dma_buf,   0, sizeof(i2s_dma_buf));
    memset(mic_audio_buf, 0, sizeof(mic_audio_buf));
}

/* -----------------------------------------------------------------------
 * MIC_Start
 * HAL_I2S_Receive_DMA size parameter is in half-words (uint16_t).
 * Each 32-bit I2S word = 2 half-words → size = MIC_BUFFER_SIZE * 2.
 * ----------------------------------------------------------------------- */
void MIC_Start(void)
{
    if (_hi2s == NULL) return;
    HAL_I2S_Receive_DMA(_hi2s,
                        (uint16_t *)i2s_dma_buf,
                        MIC_BUFFER_SIZE * 2);
}

/* -----------------------------------------------------------------------
 * MIC_Stop
 * ----------------------------------------------------------------------- */
void MIC_Stop(void)
{
    if (_hi2s == NULL) return;
    HAL_I2S_DMAStop(_hi2s);
}

/* -----------------------------------------------------------------------
 * MIC_HalfTransferCallback
 * DMA filled i2s_dma_buf[0 .. HALF-1].
 * Extract + DC-correct into mic_audio_buf[0 .. HALF-1].
 * mic_audio_buf[HALF .. END] still holds previous cycle's data, so
 * the full 512-sample frame is always a valid, continuous recording.
 * ----------------------------------------------------------------------- */
void MIC_HalfTransferCallback(void)
{
    _process_half(0, 0);
    mic_data_ready = 1;
}

/* -----------------------------------------------------------------------
 * MIC_FullTransferCallback
 * DMA filled i2s_dma_buf[HALF .. END].
 * Extract + DC-correct into mic_audio_buf[HALF .. END].
 * Now both halves are fresh — 512-sample frame is complete.
 * ----------------------------------------------------------------------- */
void MIC_FullTransferCallback(void)
{
    _process_half(MIC_HALF_SIZE, MIC_HALF_SIZE);
    mic_data_ready = 2;
}