#include "spectrum_display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h>
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Private Variables
 * ----------------------------------------------------------------------- */
static BarStyle current_style = STYLE_WITH_PEAKS;

/* -----------------------------------------------------------------------
 * DISPLAY_Init
 * Initialize display system
 * ----------------------------------------------------------------------- */
void DISPLAY_Init(void)
{
    /* SSD1306 should already be initialized in main */
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
}

/* -----------------------------------------------------------------------
 * DISPLAY_DrawSpectrum
 * Main rendering function with style selection
 * ----------------------------------------------------------------------- */
void DISPLAY_DrawSpectrum(uint8_t *bar_heights, uint8_t *peaks, BarStyle style)
{
    current_style = style;
    
    switch (style) {
        case STYLE_SOLID_BARS:
            DISPLAY_DrawBars(bar_heights);
            break;
            
        case STYLE_WITH_PEAKS:
            DISPLAY_DrawBarsWithPeaks(bar_heights, peaks);
            break;
            
        case STYLE_OUTLINED_BARS:
            /* Draw outlined bars (rectangle border only, no fill) */
            if (bar_heights == NULL) return;
            ssd1306_Fill(Black);
            for (uint8_t i = 0; i < NUM_BARS; i++)
            {
                uint8_t x = i * (BAR_WIDTH + BAR_SPACING);
                uint8_t height = bar_heights[i];
                if (height > DISPLAY_HEIGHT) height = DISPLAY_HEIGHT;
                if (height < 2) continue;
                uint8_t y = DISPLAY_HEIGHT - height;
                /* Draw rectangle outline: (x1, y1) to (x2, y2) */
                ssd1306_DrawRectangle(x, y,
                                      x + BAR_WIDTH - 1,
                                      DISPLAY_HEIGHT - 1,
                                      White);
            }
            ssd1306_UpdateScreen();
            break;
            
        case STYLE_DOTTED:
            /* Dotted bar style — fill every other pixel row */
            if (bar_heights == NULL) return;
            ssd1306_Fill(Black);
            for (uint8_t i = 0; i < NUM_BARS; i++)
            {
                uint8_t x = i * (BAR_WIDTH + BAR_SPACING);
                uint8_t height = bar_heights[i];
                if (height > DISPLAY_HEIGHT) height = DISPLAY_HEIGHT;
                if (height < 2) continue;
                uint8_t y_start = DISPLAY_HEIGHT - height;
                for (uint8_t row = y_start; row < DISPLAY_HEIGHT; row += 2)
                {
                    ssd1306_Line(x, row,
                                 x + BAR_WIDTH - 1, row,
                                 White);
                }
            }
            ssd1306_UpdateScreen();
            break;
            
        default:
            DISPLAY_DrawBars(bar_heights);
            break;
    }
}

/* -----------------------------------------------------------------------
 * DISPLAY_DrawBars
 * Simple solid bar rendering (fastest method)
 *
 * IMPORTANT: ssd1306_FillRectangle expects CORNER coordinates:
 *   ssd1306_FillRectangle(x1, y1, x2, y2, color)
 * NOT (x, y, width, height).
 * ----------------------------------------------------------------------- */
void DISPLAY_DrawBars(uint8_t *bar_heights)
{
    if (bar_heights == NULL) return;
    
    /* Clear display */
    ssd1306_Fill(Black);
    
    /* Draw each bar */
    for (uint8_t i = 0; i < NUM_BARS; i++)
    {
        /* Calculate bar position */
        uint8_t x = i * (BAR_WIDTH + BAR_SPACING);
        uint8_t height = bar_heights[i];
        
        /* Clamp height to display range */
        if (height > DISPLAY_HEIGHT) {
            height = DISPLAY_HEIGHT;
        }
        
        /* Skip if bar is too small */
        if (height < 2) continue;
        
        /* Calculate Y position (bars grow upward from bottom) */
        uint8_t y_top = DISPLAY_HEIGHT - height;
        
        /* Draw filled rectangle: top-left corner to bottom-right corner */
        ssd1306_FillRectangle(x, y_top,
                              x + BAR_WIDTH - 1,
                              DISPLAY_HEIGHT - 1,
                              White);
    }
    
    /* Update display */
    ssd1306_UpdateScreen();
}

/* -----------------------------------------------------------------------
 * DISPLAY_DrawBarsWithPeaks
 * Draw bars with peak hold indicators on top
 * ----------------------------------------------------------------------- */
void DISPLAY_DrawBarsWithPeaks(uint8_t *bar_heights, uint8_t *peaks)
{
    if (bar_heights == NULL) return;
    
    /* Clear display */
    ssd1306_Fill(Black);
    
    /* Draw each bar with peak indicator */
    for (uint8_t i = 0; i < NUM_BARS; i++)
    {
        /* Calculate bar position */
        uint8_t x = i * (BAR_WIDTH + BAR_SPACING);
        uint8_t height = bar_heights[i];
        
        /* Clamp height to display range */
        if (height > DISPLAY_HEIGHT) {
            height = DISPLAY_HEIGHT;
        }
        
        /* Draw the main bar */
        if (height >= 2) {
            uint8_t y_top = DISPLAY_HEIGHT - height;
            /* Filled rectangle: (x1, y1) to (x2, y2) */
            ssd1306_FillRectangle(x, y_top,
                                  x + BAR_WIDTH - 1,
                                  DISPLAY_HEIGHT - 1,
                                  White);
        }
        
        /* Draw peak indicator if peaks array provided */
        if (peaks != NULL && peaks[i] >= 2) {
            uint8_t peak_height = peaks[i];
            
            /* Clamp peak to display range */
            if (peak_height > DISPLAY_HEIGHT) {
                peak_height = DISPLAY_HEIGHT;
            }
            
            /* Draw peak line at the top of the peak value */
            uint8_t peak_y = DISPLAY_HEIGHT - peak_height;
            
            /* Draw horizontal line for peak indicator */
            ssd1306_Line(x, peak_y, x + BAR_WIDTH - 1, peak_y, White);
            
            /* Optional: Make peak thicker for visibility */
            if (PEAK_THICKNESS > 1 && peak_y > 0) {
                ssd1306_Line(x, peak_y - 1, x + BAR_WIDTH - 1, peak_y - 1, White);
            }
        }
    }
    
    /* Update display */
    ssd1306_UpdateScreen();
}

/* -----------------------------------------------------------------------
 * DISPLAY_ShowTitle
 * Show startup title screen
 * ----------------------------------------------------------------------- */
void DISPLAY_ShowTitle(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 20);
    ssd1306_WriteString("Audio Spectrum", Font_11x18, White);
    ssd1306_SetCursor(25, 42);
    ssd1306_WriteString("Analyzer", Font_11x18, White);
    ssd1306_UpdateScreen();
}

/* -----------------------------------------------------------------------
 * DISPLAY_ShowNoSignal
 * Display message when no audio detected
 * ----------------------------------------------------------------------- */
void DISPLAY_ShowNoSignal(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(15, 28);
    ssd1306_WriteString("No Audio", Font_11x18, White);
    ssd1306_UpdateScreen();
}

/* -----------------------------------------------------------------------
 * DISPLAY_ShowFPS
 * Display frame rate for performance monitoring
 * ----------------------------------------------------------------------- */
void DISPLAY_ShowFPS(uint16_t fps)
{
    char fps_str[16];
    snprintf(fps_str, sizeof(fps_str), "FPS:%u", fps);
    
    /* Draw FPS in top-right corner */
    ssd1306_SetCursor(85, 0);
    ssd1306_WriteString(fps_str, Font_7x10, White);
    ssd1306_UpdateScreen();
}
