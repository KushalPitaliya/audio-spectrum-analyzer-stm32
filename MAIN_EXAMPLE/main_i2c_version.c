/**
 ******************************************************************************
 * @file           : main.c (I2C OLED VERSION)
 * @brief          : Audio Spectrum Analyzer — I2C Display Integration
 * @author         : Kushal Pitaliya
 * @date           : 2026-04-08
 ******************************************************************************
 *
 * HARDWARE:
 *   - STM32F411CEU6 (Black Pill) @ 100 MHz
 *   - INMP441 MEMS Microphone (I2S2)
 *   - SSD1306 OLED Display 128x64 (I2C1 — simpler wiring!)
 *
 * WIRING:
 *   INMP441:
 *     VDD → 3.3V,  GND → GND
 *     WS  → PB12 (I2S2_WS)
 *     SCK → PB10 (I2S2_CK)
 *     SD  → PB15 (I2S2_SD)
 *     L/R → GND
 *
 *   SSD1306 (I2C2):
 *     VCC → 3.3V,  GND → GND
 *     SCL → PB10 (I2C2_SCL)
 *     SDA → PB3  (I2C2_SDA)
 *
 * IMPORTANT:
 *   To use this I2C version, you must replace ssd1306_conf.h with
 *   ssd1306_conf_i2c.h (or copy its contents into ssd1306_conf.h).
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

/* User includes */
#include "i2s_mic.h"
#include "ssd1306.h"           /* Will use I2C from ssd1306_conf.h */
#include "fft_processor.h"
#include "spectrum.h"
#include "spectrum_display.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;       /* I2C2 for OLED display */
I2S_HandleTypeDef hi2s2;       /* I2S for microphone */
DMA_HandleTypeDef hdma_spi2_rx;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2S2_Init(void);

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================ */
int main(void)
{
    /* HAL Init */
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();     /* Must be before I2S init */
    MX_I2C2_Init();    /* I2C2 for OLED */
    MX_I2S2_Init();    /* I2S for microphone */

    /* -----------------------------------------------------------------------
     * Initialize All Modules
     * ----------------------------------------------------------------------- */
    
    /* 1. Initialize OLED Display (I2C) */
    ssd1306_Init();
    DISPLAY_Init();
    
    /* 2. Show startup title for 2 seconds */
    DISPLAY_ShowTitle();
    HAL_Delay(2000);
    
    /* 3. Initialize FFT processor */
    FFT_Init();
    
    /* 4. Initialize spectrum analyzer */
    SPECTRUM_Init();
    
    /* 5. Initialize and start I2S microphone */
    MIC_Init(&hi2s2);
    MIC_Start();
    
    /* -----------------------------------------------------------------------
     * Main Loop — Audio Processing Pipeline
     * ----------------------------------------------------------------------- */
    while (1)
    {
        /* Check if new audio data is ready from DMA */
        if (mic_data_ready)
        {
            /* Clear flag FIRST to avoid missing next interrupt */
            mic_data_ready = 0;
            
            /* STEP 1: Process audio through FFT */
            FFT_Process(mic_audio_buf);
            
            /* STEP 2: Map FFT bins to 8 frequency bands */
            SPECTRUM_Update(fft_magnitude);
            
            /* STEP 3: Draw spectrum on OLED via I2C */
            DISPLAY_DrawBarsWithPeaks(
                SPECTRUM_GetBarHeights(), 
                SPECTRUM_GetPeaks()
            );
        }
    }
}

/* ============================================================================
 * HAL I2S DMA CALLBACKS
 * ============================================================================ */

/**
 * @brief I2S DMA Half Transfer Complete Callback
 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2) {
        MIC_HalfTransferCallback();
    }
}

/**
 * @brief I2S DMA Full Transfer Complete Callback
 */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2) {
        MIC_FullTransferCallback();
    }
}

/* ============================================================================
 * PERIPHERAL INITIALIZATION
 * ============================================================================ */

/**
 * @brief I2C2 Initialization Function (for OLED)
 */
static void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 400000;              /* 400 KHz Fast Mode */
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief I2S2 Initialization Function (for Microphone)
 */
static void MX_I2S2_Init(void)
{
    hi2s2.Instance = SPI2;
    hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
    hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s2.Init.DataFormat = I2S_DATAFORMAT_24B;
    hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    hi2s2.Init.CPOL = I2S_CPOL_LOW;
    hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
    
    if (HAL_I2S_Init(&hi2s2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief DMA Initialization
 */
static void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}

/**
 * @brief GPIO Initialization
 */
static void MX_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

/**
 * @brief System Clock Configuration (100 MHz)
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;              /* 25 MHz / 25 = 1 MHz */
    RCC_OscInitStruct.PLL.PLLN = 200;             /* 1 MHz * 200 = 200 MHz */
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;   /* 200 MHz / 2 = 100 MHz */
    RCC_OscInitStruct.PLL.PLLQ = 4;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   /* 50 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   /* 100 MHz */

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure I2S PLL */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    PeriphClkInitStruct.PLLI2S.PLLI2SN = 86;
    PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
    
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* Error — stay here */
    }
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number */
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
