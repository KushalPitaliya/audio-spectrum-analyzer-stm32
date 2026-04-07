# Audio Spectrum Analyzer - Setup & Integration Guide

## 📁 Project Structure

```
Sound_analyzer/
├── MIC/                    # I2S Microphone Driver
│   ├── i2s_mic.h
│   └── i2s_mic.c
├── OLED/                   # SSD1306 Display Driver  
│   ├── ssd1306.h/.c
│   ├── ssd1306_fonts.h/.c
│   └── ssd1306_tests.h/.c
├── FFT/                    # FFT Processing Module
│   ├── fft_processor.h
│   └── fft_processor.c
├── SPECTRUM/               # Frequency Band Analyzer
│   ├── spectrum.h
│   └── spectrum.c
├── DISPLAY/                # Spectrum Visualization
│   ├── spectrum_display.h
│   └── spectrum_display.c
└── MAIN_EXAMPLE/           # Integration Example
    └── main_integration_example.c
```

---

## 🚀 Quick Start - Integration Steps

### Step 1: Copy Files to STM32 Project

Copy all the module folders into your STM32CubeIDE project's `Core/` directory:

```
YourProject/
└── Core/
    ├── Inc/
    ├── Src/
    │   └── main.c        ← Replace or modify this
    ├── MIC/              ← Copy entire folder
    ├── OLED/             ← Copy entire folder
    ├── FFT/              ← Copy entire folder
    ├── SPECTRUM/         ← Copy entire folder
    └── DISPLAY/          ← Copy entire folder
```

---

### Step 2: Configure Include Paths

**In STM32CubeIDE:**

1. Right-click project → **Properties**
2. **C/C++ Build → Settings → Tool Settings**
3. **MCU GCC Compiler → Include paths**
4. Click **Add...** and add these paths:

```
../Core/MIC
../Core/OLED
../Core/FFT
../Core/SPECTRUM
../Core/DISPLAY
```

---

### Step 3: Enable CMSIS-DSP Library

**Critical for FFT functionality!**

#### Method 1: Using STM32CubeIDE (Recommended)

1. Right-click project → **Properties**
2. **C/C++ Build → Settings → MCU Settings**
   - ✅ **Enable "Use float with printf"** (optional but helpful)
   
3. **Project → Manage Middleware and Software Packs**
   - Expand **CMSIS**
   - ✅ Check **DSP**
   - Click **OK**

4. **C/C++ Build → Settings → MCU GCC Compiler → Preprocessor**
   - Add defines:
     - `ARM_MATH_CM4`
     - `__FPU_PRESENT=1`

5. **MCU Settings** (same settings page)
   - ✅ Enable **Floating Point Hardware**
   - Set to: **"FPv4-SP-D16"** (for STM32F4)

#### Method 2: Manual Makefile Edit (Advanced)

If using Makefile, add:

```makefile
# C defines
C_DEFS += -DARM_MATH_CM4 -D__FPU_PRESENT=1

# FPU flags
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard

# Link CMSIS-DSP library
LIBS = -larm_cortexM4lf_math
LIBDIR = -LDrivers/CMSIS/Lib/GCC
```

---

### Step 4: STM32CubeMX Configuration

#### I2S2 Configuration (for INMP441 Microphone)

Open `.ioc` file in CubeMX:

**Connectivity → I2S2:**
- Mode: **Master Receive**
- Standard: **Philips I2S**
- Data Format: **24 bits**
- Audio Frequency: **44 kHz**
- Clock Polarity: **Low**
- MCLK Output: **Disable**

**DMA Settings:**
- Add DMA Request: **SPI2_RX**
- Mode: **Circular**
- Priority: **High**
- Data Width: **Half Word** (both source and destination)

**NVIC Settings:**
- ✅ Enable **DMA1 Stream3 global interrupt**

#### SPI1 Configuration (for SSD1306 OLED)

**Connectivity → SPI1:**
- Mode: **Full-Duplex Master**
- Hardware NSS Signal: **Disable** (use software CS)
- Data Size: **8 Bits**
- First Bit: **MSB First**
- Prescaler: **4** (for ~8 MHz SPI clock)
- CPOL: **Low**
- CPHA: **1 Edge**

**GPIO Pins for SSD1306:**
- **CS (Chip Select):** e.g., PA15 → GPIO_Output
- **DC (Data/Command):** e.g., PB3 → GPIO_Output  
- **RST (Reset):** e.g., PB4 → GPIO_Output

---

### Step 5: Modify Your main.c

Replace the main loop in your `main.c` with this code:

```c
/* Includes */
#include "main.h"
#include "i2s_mic.h"
#include "ssd1306.h"
#include "fft_processor.h"
#include "spectrum.h"
#include "spectrum_display.h"

/* In main() function, after peripheral init: */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2S2_Init();
    MX_SPI1_Init();

    /* Initialize modules */
    ssd1306_Init();           // OLED display
    DISPLAY_Init();
    DISPLAY_ShowTitle();       // Show "Audio Spectrum Analyzer"
    HAL_Delay(2000);
    
    FFT_Init();               // FFT processor
    SPECTRUM_Init();          // Spectrum analyzer
    
    MIC_Init(&hi2s2);         // I2S microphone
    MIC_Start();
    
    /* Main loop */
    while (1)
    {
        if (mic_data_ready)
        {
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

/* Add these callbacks (outside main): */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2) {
        MIC_HalfTransferCallback();
    }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2) {
        MIC_FullTransferCallback();
    }
}
```

---

### Step 6: Update ssd1306_conf.h

Make sure your OLED driver is configured correctly.

Edit `OLED/ssd1306_conf.h`:

```c
/* I2C or SPI mode selection */
#define SSD1306_USE_SPI  // Use SPI mode

/* SPI handle (match your CubeMX config) */
extern SPI_HandleTypeDef hspi1;
#define SSD1306_SPI_PORT hspi1

/* GPIO pins for SSD1306 control */
#define SSD1306_CS_Port  GPIOA
#define SSD1306_CS_Pin   GPIO_PIN_15

#define SSD1306_DC_Port  GPIOB
#define SSD1306_DC_Pin   GPIO_PIN_3

#define SSD1306_Reset_Port GPIOB
#define SSD1306_Reset_Pin  GPIO_PIN_4
```

---

## 🔌 Hardware Wiring

### INMP441 Microphone → STM32F411

| INMP441 Pin | STM32 Pin | Function |
|------------|-----------|----------|
| VDD        | 3.3V      | Power |
| GND        | GND       | Ground |
| WS         | PA4       | I2S2_WS (Word Select) |
| SCK        | PB10      | I2S2_CK (Clock) |
| SD         | PC3       | I2S2_SD (Serial Data) |
| L/R        | GND       | Left channel select |

### SSD1306 OLED → STM32F411

| SSD1306 Pin | STM32 Pin | Function |
|------------|-----------|----------|
| VCC        | 3.3V      | Power |
| GND        | GND       | Ground |
| SCK/SCL    | PA5       | SPI1_SCK |
| MOSI/SDA   | PA7       | SPI1_MOSI |
| CS         | PA15      | Chip Select (GPIO) |
| DC         | PB3       | Data/Command (GPIO) |
| RST        | PB4       | Reset (GPIO) |

---

## 🏗️ Build & Flash

1. **Clean Build:**
   - Project → Clean Project
   - Project → Build All (Ctrl+B)

2. **Check for Errors:**
   - ✅ No "undefined reference to `arm_cfft_f32`" → CMSIS-DSP linked
   - ✅ No include errors → Paths configured correctly

3. **Flash to STM32:**
   - Run → Debug (F11) or Run (Ctrl+F11)
   - Or use ST-LINK Utility / STM32CubeProgrammer

---

## 🎵 Testing

1. **Power on STM32**
   - Should see "Audio Spectrum Analyzer" title for 2 seconds

2. **Play music near microphone**
   - Bass-heavy music → Bars 0-2 should be high
   - Vocals/speech → Bars 3-5 should be active
   - High frequencies (cymbals) → Bars 6-7 respond

3. **Test with tone generator app:**
   - 100 Hz → Bar 0 lights up
   - 440 Hz → Bar 3 lights up  
   - 4000 Hz → Bar 7 lights up

---

## 🐛 Troubleshooting

### Problem: Display shows nothing

**Check:**
- [ ] SPI configured correctly in CubeMX?
- [ ] GPIO pins match `ssd1306_conf.h`?
- [ ] Called `ssd1306_Init()` before use?
- [ ] Test with `ssd1306_TestAll()` function

### Problem: "Undefined reference to arm_cfft_f32"

**Fix:**
- Enable CMSIS-DSP in project settings (see Step 3)
- Add ARM_MATH_CM4 define
- Check that DSP library is linked

### Problem: All bars stuck at zero

**Check:**
- [ ] I2S DMA receiving data? (check `mic_data_ready` flag)
- [ ] DMA circular mode enabled?
- [ ] Callbacks registered in main.c?
- [ ] Microphone VDD is 3.3V (measure with multimeter)

### Problem: Bars jumping erratically

**Adjust:**
- Increase `SMOOTHING_FACTOR` in spectrum.c (try 0.8)
- Adjust `SCALE_FACTOR` in spectrum.c (try 2.0-5.0)
- Check microphone gain/sensitivity

### Problem: Low frame rate (<10 fps)

**Optimize:**
- Enable FPU hardware: `-mfloat-abi=hard -mfpu=fpv4-sp-d16`
- Use Release build with `-O2` optimization
- Increase SPI speed for display (try 16 MHz)

---

## 📊 Expected Performance

| Metric | Target | Typical |
|--------|--------|---------|
| Frame Rate | 20+ FPS | 25-30 FPS |
| FFT Time | <2 ms | ~1.5 ms |
| Display Update | <20 ms | ~15 ms |
| Total Latency | <50 ms | ~40 ms |

---

## 🎨 Customization Options

### Adjust Frequency Ranges

Edit `SPECTRUM/spectrum.c`, modify `frequency_bands[]` array:

```c
static const FrequencyBand frequency_bands[NUM_BARS] = {
    {1,   2,   "Your custom range"},
    // ... customize bin ranges
};
```

### Change Visual Style

In main loop, try different styles:

```c
DISPLAY_DrawBars(bars);                     // Simple solid bars
DISPLAY_DrawBarsWithPeaks(bars, peaks);     // With peak hold
DISPLAY_DrawSpectrum(bars, peaks, STYLE_OUTLINED_BARS);
```

### Adjust Sensitivity

Edit `SPECTRUM/spectrum.c`:

```c
#define SCALE_FACTOR    3.5f    // Increase for taller bars
#define MIN_THRESHOLD   0.01f   // Lower for more sensitivity
```

---

## 📚 Code Flow Diagram

```
┌─────────────────┐
│  INMP441 MIC    │ (I2S DMA)
└────────┬────────┘
         │ 512 samples (24-bit)
         ▼
┌─────────────────┐
│  MIC Driver     │ mic_audio_buf[256] (int32_t)
└────────┬────────┘
         │ mic_data_ready flag
         ▼
┌─────────────────┐
│  FFT Processor  │ CMSIS-DSP arm_cfft_f32()
└────────┬────────┘
         │ fft_magnitude[256] (float32_t)
         ▼
┌─────────────────┐
│  Spectrum       │ Map 256 bins → 8 bars
│  Analyzer       │
└────────┬────────┘
         │ bar_heights[8] (0-64 pixels)
         ▼
┌─────────────────┐
│  Display        │ Draw bars on SSD1306
│  Renderer       │
└────────┬────────┘
         │ SPI
         ▼
┌─────────────────┐
│  OLED Screen    │ Visual output!
└─────────────────┘
```

---

## 🎓 Learning Resources

- **CMSIS-DSP Documentation:** https://arm-software.github.io/CMSIS_5/DSP/
- **STM32F4 I2S:** Reference Manual RM0383, Chapter 28
- **FFT Theory:** 3Blue1Brown Fourier Transform video
- **I2S Protocol:** NXP I2S Bus Specification

---

## ✅ Pre-Flight Checklist

Before asking for help, verify:

- [ ] All folders copied to project
- [ ] Include paths added in project settings
- [ ] CMSIS-DSP enabled and linked
- [ ] ARM_MATH_CM4 and __FPU_PRESENT=1 defined
- [ ] FPU hardware enabled
- [ ] I2S2 configured: 44.1kHz, 24-bit, DMA circular
- [ ] SPI1 configured for SSD1306
- [ ] GPIO pins match your wiring
- [ ] ssd1306_conf.h updated with your pins
- [ ] Callbacks added to main.c
- [ ] Clean build successful (no errors)

---

**Ready to build? Follow Step 1! 🚀**
