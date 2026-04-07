# Audio Spectrum Analyzer - Quick Reference Card

## 🎯 One-Page Integration Checklist

### ✅ Pre-Build Checklist

```
[ ] All module folders copied to Core/
[ ] Include paths added (MIC, OLED, FFT, SPECTRUM, DISPLAY)
[ ] CMSIS-DSP enabled in project
[ ] Defines added: ARM_MATH_CM4, __FPU_PRESENT=1
[ ] FPU hardware enabled (FPv4-SP-D16)
[ ] ssd1306_conf.h updated with your GPIO pins
```

### 📝 Minimal main.c Code

```c
#include "i2s_mic.h"
#include "ssd1306.h"
#include "fft_processor.h"
#include "spectrum.h"
#include "spectrum_display.h"

extern I2S_HandleTypeDef hi2s2;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2S2_Init();
    MX_SPI1_Init();

    /* Init */
    ssd1306_Init();
    DISPLAY_Init();
    DISPLAY_ShowTitle();
    HAL_Delay(2000);
    
    FFT_Init();
    SPECTRUM_Init();
    MIC_Init(&hi2s2);
    MIC_Start();
    
    /* Main Loop */
    while (1) {
        if (mic_data_ready) {
            FFT_Process(mic_audio_buf);
            mic_data_ready = 0;
            SPECTRUM_Update(fft_magnitude);
            DISPLAY_DrawBarsWithPeaks(
                SPECTRUM_GetBarHeights(), 
                SPECTRUM_GetPeaks()
            );
        }
    }
}

/* Callbacks */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) MIC_HalfTransferCallback();
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) MIC_FullTransferCallback();
}
```

---

## ⚙️ CubeMX Settings Summary

### I2S2 (Microphone)
```
Mode: Master Receive
Standard: Philips I2S
Data: 24 bits
Freq: 44.1 kHz
DMA: Circular, High Priority
```

### SPI1 (Display)
```
Mode: Full-Duplex Master
Speed: Prescaler 4 (8 MHz)
Data: 8 bits
NSS: Software
```

---

## 🔧 Tuning Parameters

### Sensitivity
```c
// spectrum.c
#define SCALE_FACTOR    3.5f    // ↑ = taller bars
#define MIN_THRESHOLD   0.01f   // ↓ = more sensitive
```

### Smoothing
```c
// spectrum.c
#define SMOOTHING_FACTOR  0.7f   // ↑ = smoother (0.0-0.9)
#define PEAK_DECAY_RATE   0.95f  // ↑ = slower decay
```

---

## 🐛 Top 3 Errors & Fixes

### 1. "undefined reference to arm_cfft_f32"
**Fix:** Enable CMSIS-DSP library
- Project → Manage Middleware → CMSIS → DSP ✅
- Add defines: `ARM_MATH_CM4`, `__FPU_PRESENT=1`

### 2. Display blank/garbage
**Fix:** Check ssd1306_conf.h GPIO pins
```c
#define SSD1306_CS_Port  GPIOA
#define SSD1306_CS_Pin   GPIO_PIN_15
#define SSD1306_DC_Port  GPIOB
#define SSD1306_DC_Pin   GPIO_PIN_3
#define SSD1306_Reset_Port GPIOB
#define SSD1306_Reset_Pin  GPIO_PIN_4
```

### 3. All bars at zero
**Fix:** Check I2S DMA
- DMA mode: Circular ✅
- Callbacks registered in main.c ✅
- Microphone VDD = 3.3V (measure!)

---

## 📊 Pin Reference

### INMP441 → STM32
```
VDD → 3.3V
GND → GND
WS  → PA4  (I2S2_WS)
SCK → PB10 (I2S2_CK)
SD  → PC3  (I2S2_SD)
L/R → GND
```

### SSD1306 → STM32
```
VCC  → 3.3V
GND  → GND
SCK  → PA5  (SPI1_SCK)
MOSI → PA7  (SPI1_MOSI)
CS   → PA15 (GPIO)
DC   → PB3  (GPIO)
RST  → PB4  (GPIO)
```

---

## 🎵 Testing Guide

### Test Tones
- **100 Hz**  → Bar 0 (bass)
- **440 Hz**  → Bar 3 (mid)
- **4000 Hz** → Bar 7 (treble)

### Music Types
- **EDM/Bass** → Bars 0-2 high
- **Vocals**   → Bars 3-5 active
- **Cymbals**  → Bars 6-7 respond

---

## 📈 Performance Targets

```
Frame Rate: 30+ FPS
FFT Time:   ~1.5 ms
Latency:    <50 ms
CPU Usage:  ~60%
```

---

## 🚀 Build Commands

```bash
# Clean build
Project → Clean
Project → Build All (Ctrl+B)

# Flash
Run → Debug (F11)

# Release build (faster)
Build Configuration → Release
Enable -O2 optimization
```

---

## 📞 Need Help?

1. **Check:** README_SETUP.md (detailed guide)
2. **Search:** Stack Overflow "[stm32] I2S FFT"
3. **Forum:** STM32 Community forums
4. **Debug:** Use serial printf() to trace data flow

---

**Print this page and keep it next to your workbench!**
