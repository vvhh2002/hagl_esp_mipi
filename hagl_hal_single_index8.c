/*

MIT License

Copyright (c) 2019-2020 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

This file is part of the MIPI DCS HAL for HAGL graphics library:
https://github.com/tuupola/hagl_esp_mipi/

SPDX-License-Identifier: MIT

*/

#include "sdkconfig.h"
#include "hagl_hal.h"

#if defined (CONFIG_HAGL_HAL_NO_BUFFERING) && defined (HAGL_HAL_FORMAT_INDEX8)

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_heap_caps.h>
#include <string.h>
#include <mipi_display.h>
#include <bitmap.h>
#include <hagl.h>

#include "rgb332.h"
#include "hagl_hal.h"

static spi_device_handle_t spi;
static const char *TAG = "hagl_esp_mipi";

/*
 * Initializes the MIPI display
 */
bitmap_t *hagl_hal_init(void)
{
    ESP_LOGI(TAG, "INDEX8 mode without buffering.\n");
    mipi_display_init(&spi);
    return NULL;
}

void hagl_hal_put_pixel(int16_t x0, int16_t y0, color_t color)
{
    uint16_t rgb565 = rgb332_to_rgb565(color);
    mipi_display_write(spi, x0, y0, 1, 1, (uint8_t *) &rgb565);
}

/*
 * Accelerated horizontal line drawing
 */
void hagl_hal_hline(int16_t x0, int16_t y0, uint16_t width, color_t color)
{
    static uint16_t line[DISPLAY_WIDTH];
    uint16_t *ptr = line;
    uint16_t height = 1;

    for (uint16_t x = 0; x < width; x++) {
        *(ptr++) = rgb332_to_rgb565(color);
    }

    mipi_display_write(spi, x0, y0, width, height, (uint8_t *) line);
}

/*
 * Accelerated vertical line drawing
 */
void hagl_hal_vline(int16_t x0, int16_t y0, uint16_t height, color_t color)
{
    uint16_t line[DISPLAY_HEIGHT];
    uint16_t *ptr = line;
    uint16_t width = 1;

    for (uint16_t x = 0; x < height; x++) {
        *(ptr++) = rgb332_to_rgb565(color);
    }

    mipi_display_write(spi, x0, y0, width, height, (uint8_t *) line);
}

#endif /* CONFIG_HAGL_HAL_NO_BUFFERING */