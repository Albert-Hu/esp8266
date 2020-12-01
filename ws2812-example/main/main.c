#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ws2812.h"

#define NUM_PIXELS (8)

/*
  Pinout of NodeMCU
  SPI-MOSI: GPIO13, D7
*/
void app_main(void) {
  int i, next, start = 0, enabled_pixels = 1;
  uint32_t colors[3] = {
    COLOR(0x20, 0x00, 0x00),
    COLOR(0x00, 0x20, 0x00),
    COLOR(0x00, 0x00, 0x20)
  };
  uint32_t pixels[NUM_PIXELS];

  ws2812_init();

  vTaskDelay(1000 / portTICK_RATE_MS);

  while (1) {
    /*
    printf("Output %d pixels.\n", enabled_pixels);
    for (i = 0; i < enabled_pixels; i++) {
      next = (start + i) % 3;
      pixels[i] = colors[next];
      printf("Set Pixel: 0X%X\n", pixels[i]);
    }
    printf("\n");

    ws2812_set_pixels(pixels, enabled_pixels);

    if (enabled_pixels < NUM_PIXELS) {
      enabled_pixels++;
    } else {
      start = (start + 1) % 3;
    }
    */
    ws2812_reset();
    vTaskDelay(1000 / portTICK_RATE_MS);

    pixels[0] = 0x101010;
    ws2812_set_pixels(pixels, 1);
    ws2812_set_pixels(pixels, 1);
    ws2812_set_pixels(pixels, 1);
    ws2812_set_pixels(pixels, 1);

    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
