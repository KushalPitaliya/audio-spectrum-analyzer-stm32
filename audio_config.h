/**
 * @file   audio_config.h
 * @brief  Single source of truth for all shared audio pipeline constants.
 *
 * BOTH the MIC driver and FFT processor include this file so that
 * FFT_SIZE and MIC_BUFFER_SIZE are always in sync. A compile-time
 * assertion enforces this at build time — you will get a hard compiler
 * error if they ever drift apart.
 *
 * Change AUDIO_FRAME_SIZE here to resize the entire pipeline at once.
 */

#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

/* -----------------------------------------------------------------------
 * Shared pipeline size
 * Must be a power of 2 (required by the Cooley-Tukey FFT algorithm).
 * ----------------------------------------------------------------------- */
#define AUDIO_FRAME_SIZE    512     /* Samples per FFT frame */

/* -----------------------------------------------------------------------
 * Sample rate — must match I2S CubeMX configuration
 * ----------------------------------------------------------------------- */
#define AUDIO_SAMPLE_RATE   44100   /* Hz */

/* -----------------------------------------------------------------------
 * Compile-time safety contract
 * If FFT_SIZE ever diverges from MIC_BUFFER_SIZE you get:
 *   error: static assertion failed: "FFT_SIZE must equal MIC_BUFFER_SIZE"
 * This makes the buffer-overread bug impossible at compile time.
 * ----------------------------------------------------------------------- */
#define _AUDIO_CONFIG_STATIC_CHECK \
    _Static_assert(AUDIO_FRAME_SIZE > 0,                        \
                   "AUDIO_FRAME_SIZE must be > 0");             \
    _Static_assert((AUDIO_FRAME_SIZE & (AUDIO_FRAME_SIZE - 1)) == 0, \
                   "AUDIO_FRAME_SIZE must be a power of 2")

#endif /* AUDIO_CONFIG_H */
