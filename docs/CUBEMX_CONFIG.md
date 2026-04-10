# STM32F411CEU6 CubeMX Configuration Guide
## Audio Spectrum Analyzer - Complete Hardware Setup

---

## 📋 Step-by-Step CubeMX Configuration

### 1️⃣ Create New Project

1. Open **STM32CubeIDE**
2. **File → New → STM32 Project**
3. Search: `STM32F411CEU6`
4. Select it → Click **Next**
5. Project Name: `AudioSpectrumAnalyzer`
6. Click **Finish**

---

### 2️⃣ Clock Configuration (RCC)

**Pinout & Configuration → System Core → RCC**

```
High Speed Clock (HSE): Crystal/Ceramic Resonator
Low Speed Clock (LSE): Disable (not needed)
```

**Click "Clock Configuration" tab:**

```
Input frequency: 25 MHz (Black Pill has 25MHz crystal)

PLL Source Mux: HSE
PLLM: /25
PLLN: x200
PLLP: /2

HCLK: 100 MHz
APB1 Prescaler: /2 → APB1 = 50 MHz
APB2 Prescaler: /1 → APB2 = 100 MHz

I2S Clock: ~49.15 MHz (auto-calculated)
```

**Visual Settings:**
```
┌─────────────────────────────────────────────────┐
│ HSE = 25 MHz                                    │
│    ↓                                            │
│ PLL: /25 → x200 → /2 = 100 MHz (SYSCLK)        │
│    ↓                                            │
│ AHB Prescaler: /1 → HCLK = 100 MHz             │
│    ↓                                            │
│ APB1 Prescaler: /2 → APB1 = 50 MHz             │
│ APB2 Prescaler: /1 → APB2 = 100 MHz            │
└─────────────────────────────────────────────────┘
```

---

### 3️⃣ I2S2 Configuration (Microphone)

**Pinout & Configuration → Connectivity → I2S2**

**Mode:**
```
Mode: Master Receive
```

**Parameter Settings:**
```
Transmission Mode:       Master Receive
Communication Standard:  I2S Philips
Data and Frame Format:   24 Bits Data on 32 Bits Frame
Selected Audio Frequency: 44 KHz
Clock Polarity:          Low
```

**DMA Settings (Click "Add" button):**
```
DMA Request: SPI2_RX
Stream:      DMA1 Stream 3
Direction:   Peripheral to Memory
Priority:    High

Mode:        Circular       ← CRITICAL!
Increment Address:
  - Peripheral: No
  - Memory:     Yes
Data Width:
  - Peripheral: Half Word
  - Memory:     Half Word
```

**NVIC Settings:**
```
☑ DMA1 stream3 global interrupt: Enabled
☑ SPI2 global interrupt:         Enabled (optional)
```

**GPIO Settings (Auto-assigned):**
```
PB10 → I2S2_CK   (Clock)
PB12 → I2S2_WS   (Word Select / LRCK)
PB15 → I2S2_SD   (Serial Data - MISO for receive)
```

**⚠️ ALTERNATIVE PINS (if conflicts):**
```
PC3 can be used for I2S2_SD instead of PB15
PA4 can be used for I2S2_WS instead of PB12
```

---

### 4️⃣ SPI1 Configuration (OLED Display)

**Pinout & Configuration → Connectivity → SPI1**

**Mode:**
```
Mode: Full-Duplex Master
Hardware NSS Signal: Disable
```

**Parameter Settings:**
```
Frame Format:        Motorola
Data Size:           8 Bits
First Bit:           MSB First

Prescaler:           4          (gives ~12.5 MHz SPI clock)
Baud Rate:           12.5 MBits/s (calculated automatically)

Clock Polarity (CPOL): Low
Clock Phase (CPHA):    1 Edge

CRC Calculation:     Disabled
NSS Signal Type:     Software
```

**GPIO Settings (Auto-assigned):**
```
PA5 → SPI1_SCK   (Clock)
PA6 → SPI1_MISO  (not used, but assigned)
PA7 → SPI1_MOSI  (Data to display)
```

---

### 5️⃣ GPIO Configuration (OLED Control Pins)

**Pinout & Configuration → System Core → GPIO**

**Configure these pins as GPIO_Output:**

Click on each pin in the pinout view and set to `GPIO_Output`:

```
PA4  → GPIO_Output  (OLED CS  - Chip Select)
PB0  → GPIO_Output  (OLED DC  - Data/Command)
PB1  → GPIO_Output  (OLED RST - Reset)
```

**GPIO Output Settings (for each pin):**
```
GPIO output level:  High (for CS), Low (for others)
GPIO mode:          Output Push Pull
GPIO Pull-up/Pull-down: No pull-up and no pull-down
Maximum output speed: High
User Label:         OLED_CS, OLED_DC, OLED_RST
```

**Or use these alternative pins:**
```
PA15 → OLED_CS
PB3  → OLED_DC
PB4  → OLED_RST
```

---

### 6️⃣ SYS Configuration

**Pinout & Configuration → System Core → SYS**

```
Debug: Serial Wire
Timebase Source: SysTick
```

---

### 7️⃣ NVIC Configuration

**Pinout & Configuration → System Core → NVIC**

```
☑ DMA1 stream3 global interrupt    Priority: 0, 0
☑ SPI2 global interrupt            Priority: 1, 0 (optional)
☐ SysTick                          Priority: 15, 0
```

---

### 8️⃣ Project Manager Settings

**Click "Project Manager" tab**

**Project:**
```
Project Name:        AudioSpectrumAnalyzer
Project Location:    (your preferred path)
Toolchain/IDE:       STM32CubeIDE
```

**Code Generator:**
```
☑ Generate peripheral initialization as a pair of .c/.h files
☑ Keep User Code when re-generating
☐ Delete previously generated files when not re-generated
```

---

### 9️⃣ Generate Code

**Click "Generate Code" button (gear icon)**

Or: **Project → Generate Code**

---

## 📌 Final Pin Mapping Summary

### STM32F411CEU6 Pinout for Audio Spectrum Analyzer

```
┌────────────────────────────────────────────────────────────┐
│                    STM32F411CEU6 (Black Pill)              │
│                                                            │
│  INMP441 Microphone:                                       │
│  ├── VDD  → 3.3V                                          │
│  ├── GND  → GND                                           │
│  ├── WS   → PB12 (I2S2_WS)                                │
│  ├── SCK  → PB10 (I2S2_CK)                                │
│  ├── SD   → PB15 (I2S2_SD)  [or PC3]                      │
│  └── L/R  → GND (Left channel)                            │
│                                                            │
│  SSD1306 OLED Display:                                     │
│  ├── VCC  → 3.3V                                          │
│  ├── GND  → GND                                           │
│  ├── SCK  → PA5 (SPI1_SCK)                                │
│  ├── MOSI → PA7 (SPI1_MOSI)                               │
│  ├── CS   → PA4 (GPIO)                                    │
│  ├── DC   → PB0 (GPIO)                                    │
│  └── RST  → PB1 (GPIO)                                    │
│                                                            │
│  Debug:                                                    │
│  ├── SWDIO → PA13                                         │
│  └── SWCLK → PA14                                         │
│                                                            │
│  Crystal:                                                  │
│  ├── HSE_IN  → PH0 (25 MHz)                               │
│  └── HSE_OUT → PH1                                        │
│                                                            │
│  Onboard LED: PC13 (optional for beat detection)          │
└────────────────────────────────────────────────────────────┘
```

---

## 🔧 Update ssd1306_conf.h to Match

After generating code, update `ssd1306_conf.h`:

```c
/* Select SPI mode */
#define SSD1306_USE_SPI

/* SPI Handle */
extern SPI_HandleTypeDef hspi1;
#define SSD1306_SPI_PORT        hspi1

/* Control Pins - MATCH YOUR CubeMX CONFIG! */
#define SSD1306_CS_Port         GPIOA
#define SSD1306_CS_Pin          GPIO_PIN_4

#define SSD1306_DC_Port         GPIOB
#define SSD1306_DC_Pin          GPIO_PIN_0

#define SSD1306_Reset_Port      GPIOB
#define SSD1306_Reset_Pin       GPIO_PIN_1

/* Display dimensions */
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

/* Set this if using DMA for SPI */
// #define SSD1306_USE_DMA
```

---

## ⚡ Quick Reference - CubeMX Settings Table

| Peripheral | Setting | Value |
|------------|---------|-------|
| **RCC** | HSE | Crystal 25 MHz |
| **RCC** | SYSCLK | 100 MHz |
| **RCC** | APB1 | 50 MHz |
| **RCC** | APB2 | 100 MHz |
| **I2S2** | Mode | Master Receive |
| **I2S2** | Standard | Philips |
| **I2S2** | Data Format | 24-bit on 32-bit frame |
| **I2S2** | Audio Freq | 44 KHz |
| **I2S2** | DMA Mode | **Circular** |
| **I2S2** | DMA Stream | DMA1 Stream 3 |
| **SPI1** | Mode | Full-Duplex Master |
| **SPI1** | Prescaler | 4 (~12.5 MHz) |
| **SPI1** | Data Size | 8 bits |
| **SPI1** | CPOL/CPHA | Low / 1 Edge |
| **GPIO** | PA4 | Output (OLED_CS) |
| **GPIO** | PB0 | Output (OLED_DC) |
| **GPIO** | PB1 | Output (OLED_RST) |

---

## 🔌 Physical Wiring Diagram

```
                    STM32F411CEU6 (Black Pill)
                    ┌─────────────────────┐
                    │                     │
    INMP441         │                     │         SSD1306
   ┌───────┐        │                     │        ┌───────┐
   │ VDD ──┼── 3.3V ┤                     ├─ 3.3V ─┼── VCC │
   │ GND ──┼── GND ─┤                     ├─ GND ──┼── GND │
   │ WS  ──┼────────┤ PB12 (I2S2_WS)      │        │       │
   │ SCK ──┼────────┤ PB10 (I2S2_CK)      │        │       │
   │ SD  ──┼────────┤ PB15 (I2S2_SD)      │        │       │
   │ L/R ──┼── GND ─┤                     │        │       │
   └───────┘        │                     │        │       │
                    │         PA5 (SCK) ──┼────────┼── SCK │
                    │         PA7 (MOSI)──┼────────┼── MOSI│
                    │         PA4 (CS) ───┼────────┼── CS  │
                    │         PB0 (DC) ───┼────────┼── DC  │
                    │         PB1 (RST) ──┼────────┼── RST │
                    │                     │        └───────┘
                    │       PC13 (LED) ◄──┤ (onboard LED)
                    └─────────────────────┘
```

---

## 📝 After Code Generation Checklist

```
[ ] Generated code successfully (no CubeMX errors)
[ ] Verified main.c has all MX_xxx_Init() calls
[ ] Copied FFT/, SPECTRUM/, DISPLAY/ folders to Core/
[ ] Added include paths in project settings
[ ] Enabled CMSIS-DSP library
[ ] Added ARM_MATH_CM4 and __FPU_PRESENT=1 defines
[ ] Updated ssd1306_conf.h with correct GPIO pins
[ ] Added integration code to main.c
[ ] Added I2S callbacks to main.c
[ ] Clean build successful
```

---

## 🚨 Common CubeMX Mistakes to Avoid

1. **I2S DMA not set to Circular mode** → Audio stops after one buffer
2. **Wrong pins for I2S2_SD** → No audio data (use PB15 or PC3)
3. **SPI prescaler too low** → Display corruption
4. **Forgot to enable DMA interrupt** → Callbacks never fire
5. **HSE not configured** → Clock errors, I2S timing wrong

---

## ✅ Verification

After flashing:
1. **Title screen appears** → Display working ✓
2. **Clap near mic → bars spike** → Audio working ✓
3. **Play music → bars dance** → Full system working ✓

---

**Save this file for reference during CubeMX setup!**
