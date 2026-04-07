# STM32F411CEU6 CubeMX Configuration - I2C OLED VERSION
## Audio Spectrum Analyzer - I2C Display Configuration

---

## 🔄 Changes from SPI Version

**Display Interface Changed:**
- ❌ SPI1 (removed)
- ✅ I2C1 (new - 2 wires instead of 5)

**Benefits:**
- Fewer wires (4 pins total vs 7 pins)
- Simpler wiring
- Multiple I2C devices can share the bus

---

## 📋 Complete CubeMX Configuration

### 1️⃣ Create New Project

1. Open **STM32CubeIDE**
2. **File → New → STM32 Project**
3. Search: `STM32F411CEU6`
4. Select → Click **Next**
5. Project Name: `AudioSpectrumAnalyzer_I2C`
6. Click **Finish**

---

### 2️⃣ Clock Configuration (Same as before)

**Pinout & Configuration → System Core → RCC**

```
High Speed Clock (HSE): Crystal/Ceramic Resonator
```

**Clock Configuration Tab:**

```
Input frequency: 25 MHz

PLL Configuration:
├── PLLM: /25
├── PLLN: x200
└── PLLP: /2

Result:
├── SYSCLK: 100 MHz
├── HCLK:   100 MHz
├── APB1:   50 MHz  (prescaler /2)
└── APB2:   100 MHz (prescaler /1)
```

---

### 3️⃣ I2S2 Configuration (SAME - No changes)

**Pinout & Configuration → Connectivity → I2S2**

**Mode:**
```
Mode: Master Receive
```

**Parameters:**
```
Transmission Mode:       Master Receive
Communication Standard:  I2S Philips
Data and Frame Format:   24 Bits Data on 32 Bits Frame
Selected Audio Frequency: 44 KHz
Clock Polarity:          Low
```

**DMA Settings:**
```
DMA Request: SPI2_RX
Stream:      DMA1 Stream 3
Direction:   Peripheral to Memory
Priority:    High
Mode:        Circular ← CRITICAL!
Data Width:  Half Word (both)
```

**NVIC:**
```
☑ DMA1 stream3 global interrupt: Enabled
```

**Pins (Auto-assigned):**
```
PB10 → I2S2_CK
PB12 → I2S2_WS
PB15 → I2S2_SD (or PC3)
```

---

### 4️⃣ I2C1 Configuration (NEW - For OLED Display)

**Pinout & Configuration → Connectivity → I2C1**

**Mode:**
```
I2C: I2C
```

**Configuration:**

**Parameter Settings:**
```
I2C Speed Mode:     Standard Mode (100 KHz)
                    or Fast Mode (400 KHz) ← Recommended for faster updates

Clock Speed:        400000 (400 KHz)

Primary Address:    0 (not used as slave)

Addressing Mode:    7-bit

Dual Address:       Disabled

General Call:       Disabled

No Stretch Mode:    Disabled
```

**GPIO Settings (Auto-assigned):**
```
Option 1 (Recommended):
├── PB8 → I2C1_SCL
└── PB9 → I2C1_SDA

Option 2 (Alternative):
├── PB6 → I2C1_SCL
└── PB7 → I2C1_SDA
```

**DMA Settings (OPTIONAL - for faster updates):**
```
Leave empty for basic operation
OR add for better performance:
├── I2C1_TX: DMA1 Stream 6
└── I2C1_RX: DMA1 Stream 5
```

**NVIC Settings:**
```
☑ I2C1 event interrupt: Enabled
☑ I2C1 error interrupt: Enabled
```

---

### 5️⃣ GPIO Configuration

**No extra GPIO pins needed!** (I2C only uses SCL/SDA)

Optional - keep PC13 for onboard LED:
```
PC13 → GPIO_Output (LED, optional for beat detection)
```

---

### 6️⃣ Project Manager Settings

**Code Generator:**
```
☑ Generate peripheral initialization as a pair of .c/.h files
☑ Keep User Code when re-generating
```

---

### 7️⃣ Generate Code

Click **Generate Code** or **Project → Generate Code**

---

## 📌 Updated Pin Mapping Summary

### STM32F411CEU6 Pinout - I2C Version

```
┌────────────────────────────────────────────────────────────┐
│                    STM32F411CEU6 (Black Pill)              │
│                                                            │
│  INMP441 Microphone:                                       │
│  ├── VDD  → 3.3V                                          │
│  ├── GND  → GND                                           │
│  ├── WS   → PB12 (I2S2_WS)                                │
│  ├── SCK  → PB10 (I2S2_CK)                                │
│  ├── SD   → PB15 (I2S2_SD)                                │
│  └── L/R  → GND (Left channel)                            │
│                                                            │
│  SSD1306 OLED Display (I2C):                               │
│  ├── VCC  → 3.3V                                          │
│  ├── GND  → GND                                           │
│  ├── SCL  → PB8 (I2C1_SCL)  [or PB6]                      │
│  └── SDA  → PB9 (I2C1_SDA)  [or PB7]                      │
│                                                            │
│  Debug:                                                    │
│  ├── SWDIO → PA13                                         │
│  └── SWCLK → PA14                                         │
│                                                            │
│  Optional LED: PC13 (onboard)                              │
└────────────────────────────────────────────────────────────┘
```

---

## 🔌 Physical Wiring Diagram - I2C Version

```
                    STM32F411CEU6 (Black Pill)
                    ┌─────────────────────┐
                    │                     │
    INMP441         │                     │      SSD1306 (I2C)
   ┌───────┐        │                     │        ┌───────┐
   │ VDD ──┼── 3.3V ┤                     ├─ 3.3V ─┼── VCC │
   │ GND ──┼── GND ─┤                     ├─ GND ──┼── GND │
   │ WS  ──┼────────┤ PB12 (I2S2_WS)      │        │       │
   │ SCK ──┼────────┤ PB10 (I2S2_CK)      │        │       │
   │ SD  ──┼────────┤ PB15 (I2S2_SD)      │        │       │
   │ L/R ──┼── GND ─┤                     │        │       │
   └───────┘        │                     │        │       │
                    │         PB8 (SCL)───┼────────┼── SCL │
                    │         PB9 (SDA)───┼────────┼── SDA │
                    │                     │        └───────┘
                    │       PC13 (LED) ◄──┤
                    └─────────────────────┘

Note: Only 4 wires for OLED! (VCC, GND, SCL, SDA)
```

---

## 🔧 Update ssd1306_conf.h

Replace `OLED/ssd1306_conf.h` with this configuration:

```c
#ifndef SSD1306_CONF_H
#define SSD1306_CONF_H

/* ============================================ */
/* I2C MODE - NOT SPI!                         */
/* ============================================ */
#define SSD1306_USE_I2C
// #define SSD1306_USE_SPI  ← Comment this out!

/* ============================================ */
/* I2C CONFIGURATION                           */
/* ============================================ */
extern I2C_HandleTypeDef hi2c1;
#define SSD1306_I2C_PORT        hi2c1
#define SSD1306_I2C_ADDR        0x3C    // 7-bit address
#define SSD1306_I2C_TIMEOUT     1000    // 1 second timeout

/* ============================================ */
/* DISPLAY SIZE                                */
/* ============================================ */
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

#endif
```

---

## 📝 Update main.c

Same as before, but ensure you have:

```c
/* Includes */
#include "main.h"
#include "i2s_mic.h"
#include "ssd1306.h"         // I2C version will be used
#include "fft_processor.h"
#include "spectrum.h"
#include "spectrum_display.h"

/* External handles */
extern I2S_HandleTypeDef hi2s2;
extern I2C_HandleTypeDef hi2c1;  // I2C handle for OLED

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();          // Initialize I2C for OLED
    MX_I2S2_Init();

    /* Initialize OLED via I2C */
    ssd1306_Init();
    DISPLAY_Init();
    DISPLAY_ShowTitle();
    HAL_Delay(2000);
    
    /* Initialize audio processing */
    FFT_Init();
    SPECTRUM_Init();
    MIC_Init(&hi2s2);
    MIC_Start();
    
    /* Main loop */
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

/* I2S Callbacks */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) MIC_HalfTransferCallback();
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) MIC_FullTransferCallback();
}
```

---

## ⚡ Quick Reference - I2C CubeMX Settings

| Peripheral | Setting | Value |
|------------|---------|-------|
| **RCC** | HSE | Crystal 25 MHz |
| **RCC** | SYSCLK | 100 MHz |
| **I2C1** | Mode | I2C |
| **I2C1** | Speed | 400 KHz (Fast Mode) |
| **I2C1** | Pins | PB8 (SCL), PB9 (SDA) |
| **I2S2** | Mode | Master Receive |
| **I2S2** | Standard | Philips, 24-bit, 44 KHz |
| **I2S2** | DMA Mode | **Circular** |
| **I2S2** | Pins | PB10 (CK), PB12 (WS), PB15 (SD) |

---

## 🎯 Benefits of I2C Version

✅ **Simpler Wiring** - Only 4 wires (vs 7 for SPI)
✅ **Fewer GPIO pins used** - No CS, DC, RST pins needed
✅ **Standard I2C addressing** - Easy to add more I2C devices
✅ **No pin conflicts** - Frees up PA4, PA5, PA7, PB0, PB1

---

## 🔍 I2C Address Detection

If display doesn't work, your SSD1306 might use address **0x3D** instead of 0x3C.

**Test with I2C scanner code:**
```c
// Add to main() before ssd1306_Init()
for (uint8_t addr = 0; addr < 128; addr++) {
    if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) == HAL_OK) {
        printf("I2C device found at address: 0x%02X\n", addr);
    }
}
```

Then update `ssd1306_conf.h`:
```c
#define SSD1306_I2C_ADDR  0x3D  // If your display uses this
```

---

## 🐛 Troubleshooting I2C

### Display shows nothing:
```
[ ] Check I2C wiring (SCL, SDA, VCC, GND)
[ ] Verify I2C address (0x3C or 0x3D)
[ ] Check pull-up resistors on SCL/SDA (some displays have built-in)
[ ] Try lower I2C speed (100 KHz instead of 400 KHz)
[ ] Verify hi2c1 is initialized in main.c
```

### I2C Timeout errors:
```
[ ] Add external pull-up resistors (4.7kΩ on SCL and SDA to 3.3V)
[ ] Reduce I2C clock speed in CubeMX
[ ] Check for loose wiring
[ ] Verify 3.3V power supply is stable
```

---

## ✅ After Code Generation Checklist

```
[ ] Generated code successfully
[ ] Updated ssd1306_conf.h to use I2C (not SPI)
[ ] Verified I2C1 handle (hi2c1) exists
[ ] Copied FFT/, SPECTRUM/, DISPLAY/ folders
[ ] Added include paths
[ ] Enabled CMSIS-DSP
[ ] Added ARM_MATH_CM4 define
[ ] Updated main.c with I2C version
[ ] Added I2S callbacks
[ ] Clean build successful
```

---

## 🚀 Testing

1. Flash to STM32F411CEU6
2. Power on → See "Audio Spectrum Analyzer" title
3. Play music → Bars animate
4. If display blank → check I2C address (try 0x3D)

---

**This is the RECOMMENDED configuration - simpler wiring with I2C!**
