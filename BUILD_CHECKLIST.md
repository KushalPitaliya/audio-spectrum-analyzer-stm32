# ✅ Pre-Build Verification Checklist

**Complete this checklist BEFORE attempting to build your project.**

Print this page and check off each item as you complete it.

---

## 📂 File Structure Verification

```
[ ] Created/copied FFT folder with:
    [ ] fft_processor.h
    [ ] fft_processor.c

[ ] Created/copied SPECTRUM folder with:
    [ ] spectrum.h
    [ ] spectrum.c

[ ] Created/copied DISPLAY folder with:
    [ ] spectrum_display.h
    [ ] spectrum_display.c

[ ] Existing MIC folder has:
    [ ] i2s_mic.h
    [ ] i2s_mic.c

[ ] Existing OLED folder has:
    [ ] ssd1306.h / ssd1306.c
    [ ] ssd1306_fonts.h / ssd1306_fonts.c
    [ ] ssd1306_conf.h
```

---

## ⚙️ STM32CubeIDE Configuration

### Include Paths
```
[ ] Right-click project → Properties → C/C++ Build → Settings
[ ] MCU GCC Compiler → Include paths
[ ] Added: ../Core/MIC
[ ] Added: ../Core/OLED
[ ] Added: ../Core/FFT
[ ] Added: ../Core/SPECTRUM
[ ] Added: ../Core/DISPLAY
[ ] Clicked Apply → OK
```

### CMSIS-DSP Library
```
[ ] Project → Manage Middleware and Software Packs
[ ] Expanded CMSIS node
[ ] Checked ✅ DSP
[ ] Clicked OK
[ ] Verified "CMSIS" folder appeared in project tree
```

### Compiler Defines
```
[ ] C/C++ Build → Settings → MCU GCC Compiler → Preprocessor
[ ] Added define: ARM_MATH_CM4
[ ] Added define: __FPU_PRESENT=1
[ ] Clicked Apply → OK
```

### FPU Settings
```
[ ] C/C++ Build → Settings → MCU Settings
[ ] Floating Point Hardware: FPv4-SP-D16 (or enabled)
[ ] Floating Point ABI: Hardware implementation (-mfloat-abi=hard)
[ ] Clicked Apply → OK
```

### Optional: Printf Float Support
```
[ ] C/C++ Build → Settings → MCU Settings
[ ] Checked ✅ "Use float with printf from newlib-nano"
[ ] Clicked Apply → OK
```

---

## 🔧 CubeMX Hardware Configuration

### I2S2 Configuration
```
[ ] Opened .ioc file in CubeMX
[ ] Connectivity → I2S2
[ ] Mode: Master Receive
[ ] Standard: Philips I2S
[ ] Data Format: 24 Bits Data on 32 Bits Frame
[ ] Selected Audio Frequency: 44 KHz
[ ] Clock Polarity: Low
[ ] MCLK Output: Disable
```

### I2S2 DMA Settings
```
[ ] I2S2 → DMA Settings tab
[ ] Added: SPI2_RX
[ ] Mode: Circular
[ ] Priority: High (or Very High)
[ ] Data Width: Half Word (both)
```

### I2S2 NVIC Settings
```
[ ] I2S2 → NVIC Settings tab
[ ] Enabled ✅: DMA1 stream3 global interrupt
[ ] Enabled ✅: SPI2 global interrupt
```

### SPI1 Configuration (for OLED)
```
[ ] Connectivity → SPI1
[ ] Mode: Full-Duplex Master
[ ] Hardware NSS Signal: Disable
[ ] Data Size: 8 Bits
[ ] First Bit: MSB First
[ ] Prescaler: 4 (adjust for your clock - target ~8MHz)
[ ] Clock Polarity (CPOL): Low
[ ] Clock Phase (CPHA): 1 Edge
```

### GPIO Configuration
```
[ ] Verify I2S2 pins:
    [ ] PA4  = I2S2_WS
    [ ] PB13 = I2S2_CK  ← NOT PB10 (that's I2C2_SCL)
    [ ] PC3  = I2S2_SD

[ ] Verify I2C2 pins:
    [ ] PB10 = I2C2_SCL  ← Exclusively for OLED
    [ ] PB3  = I2C2_SDA
```

### Clock Configuration
```
[ ] Clock Configuration tab
[ ] HCLK: 100 MHz (or your target frequency)
[ ] APB1: 50 MHz
[ ] APB2: 100 MHz
[ ] I2S Clock: 49-50 MHz (verify PLL settings)
```

### Generate Code
```
[ ] Project → Generate Code
[ ] Verify no errors in code generation
```

---

## 📝 Code Integration

### Update ssd1306_conf.h
```
[ ] Opened OLED/ssd1306_conf.h
[ ] Verified SSD1306_USE_SPI is defined (not I2C)
[ ] Verified SPI handle matches: hspi1
[ ] Verified GPIO pins match your CubeMX configuration:
    #define SSD1306_CS_Port     GPIOA
    #define SSD1306_CS_Pin      GPIO_PIN_15
    #define SSD1306_DC_Port     GPIOB
    #define SSD1306_DC_Pin      GPIO_PIN_3
    #define SSD1306_Reset_Port  GPIOB
    #define SSD1306_Reset_Pin   GPIO_PIN_4
```

### Update main.c
```
[ ] Added includes at top:
    #include "i2s_mic.h"
    #include "ssd1306.h"
    #include "fft_processor.h"
    #include "spectrum.h"
    #include "spectrum_display.h"

[ ] Added extern declaration:
    extern I2S_HandleTypeDef hi2s2;

[ ] Added initialization code after peripheral init:
    ssd1306_Init();
    DISPLAY_Init();
    DISPLAY_ShowTitle();
    HAL_Delay(2000);
    FFT_Init();
    SPECTRUM_Init();
    MIC_Init(&hi2s2);
    MIC_Start();

[ ] Replaced while(1) loop with processing code:
    if (mic_data_ready) {
        FFT_Process(mic_audio_buf);
        mic_data_ready = 0;
        SPECTRUM_Update(fft_magnitude);
        DISPLAY_DrawBarsWithPeaks(
            SPECTRUM_GetBarHeights(),
            SPECTRUM_GetPeaks()
        );
    }

[ ] Added callbacks (outside main):
    void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
        if (hi2s->Instance == SPI2) MIC_HalfTransferCallback();
    }
    
    void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
        if (hi2s->Instance == SPI2) MIC_FullTransferCallback();
    }
```

---

## 🔌 Hardware Wiring

### INMP441 Microphone
```
[ ] VDD → STM32 3.3V
[ ] GND → STM32 GND
[ ] WS  → STM32 PA4 (I2S2_WS)
[ ] SCK → STM32 PB13 (I2S2_CK)  ← NOT PB10
[ ] SD  → STM32 PC3 (I2S2_SD)
[ ] L/R → GND (left channel)
```

### SSD1306 OLED Display
```
[ ] VCC → STM32 3.3V
[ ] GND → STM32 GND
[ ] SCL → STM32 PB10 (I2C2_SCL)
[ ] SDA → STM32 PB3  (I2C2_SDA)
```

### Power Supply
```
[ ] Verified 3.3V rail voltage with multimeter
[ ] All GND connections secure
[ ] No loose wires
```

---

## 🏗️ Build Process

### Pre-Build
```
[ ] Project → Clean
[ ] Removed all .o and .elf files
```

### Build Configuration
```
[ ] Build Configuration: Debug (initially)
[ ] Optimization: -O0 or -O1 (for debugging)
[ ] Later switch to Release with -O2
```

### Build
```
[ ] Project → Build All (Ctrl+B)
[ ] Build completed with 0 errors
[ ] Warnings are OK, but check for critical ones
[ ] Binary size < 128KB (should be ~50-60KB)
```

### Common Build Errors - Check These:
```
If "arm_cfft_f32 undefined":
[ ] CMSIS-DSP library enabled?
[ ] ARM_MATH_CM4 defined?

If include errors:
[ ] Include paths added correctly?
[ ] File names match exactly?

If ssd1306 errors:
[ ] SSD1306 driver files in project?
[ ] ssd1306_conf.h configured?
```

---

## 🚀 Flash & Test

### Flash to STM32
```
[ ] Connected ST-LINK debugger
[ ] STM32 powered on
[ ] Run → Debug (F11)
[ ] Or use Run → Run (Ctrl+F11)
[ ] Verified "Download verified successfully" message
```

### Initial Test
```
[ ] STM32 powered on
[ ] OLED displays "Audio Spectrum Analyzer" for 2 seconds
[ ] OLED then shows bars (might be at zero with no audio)
```

### Audio Test
```
[ ] Played music near microphone
[ ] Bass music → Bars 0-2 move
[ ] Vocals → Bars 3-5 active
[ ] High frequencies → Bars 6-7 respond
```

### Tone Generator Test
```
[ ] Used phone app to play:
    [ ] 100 Hz tone → Bar 0 lights up
    [ ] 440 Hz tone → Bar 3 lights up
    [ ] 4000 Hz tone → Bar 7 lights up
```

---

## 🐛 Quick Troubleshooting

### If display is blank:
```
[ ] Power to display verified (3.3V)
[ ] SPI pins wired correctly
[ ] GPIO control pins match ssd1306_conf.h
[ ] Try ssd1306_TestAll() function
```

### If bars don't move:
```
[ ] Microphone VDD = 3.3V (measure!)
[ ] I2S wiring correct (WS, SCK, SD)
[ ] DMA circular mode enabled
[ ] Callbacks registered in main.c
[ ] mic_data_ready flag being set (add printf)
```

### If build fails:
```
[ ] CMSIS-DSP enabled
[ ] Include paths correct
[ ] All files copied to project
[ ] Clean build attempted
```

---

## 📊 Performance Verification

### Expected Results:
```
[ ] Frame rate: 20-30+ FPS (smooth animation)
[ ] No display flicker
[ ] Bars respond quickly to audio changes (<50ms)
[ ] Peak hold lines visible and decay smoothly
[ ] CPU usage < 70% (check in debug mode)
```

---

## ✅ SUCCESS CRITERIA

**Your project is working correctly if:**

✓ OLED displays startup title  
✓ Bars visible on screen  
✓ Bass music makes bars 0-2 tall  
✓ Vocals/speech activates bars 3-5  
✓ Cymbals/high sounds move bars 6-7  
✓ Animation is smooth (no freezing)  
✓ Peak hold indicators visible  

---

## 📞 If You're Stuck

1. **Re-check this entire checklist** - missed steps are #1 cause of issues
2. **Read README_SETUP.md** - detailed troubleshooting section
3. **Check QUICK_REFERENCE.md** - common errors & fixes
4. **Search:** "STM32F4 I2S FFT spectrum analyzer"
5. **Forum:** STM32 Community forums

---

**Date Completed: _______________**

**Notes/Issues Encountered:**

```
_________________________________________________________________

_________________________________________________________________

_________________________________________________________________
```

**Working? YES / NO**

If NO, what's not working:

```
_________________________________________________________________

_________________________________________________________________
```

---

**Print this checklist and work through it systematically!**
**Don't skip steps - each one matters!**
