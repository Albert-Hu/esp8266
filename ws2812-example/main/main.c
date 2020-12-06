#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ws2812.h"

#define NUM_PIXELS (8)

/*
  Pinout of NodeMCU
  I2S: RX0, GPIO3
*/
void app_main(void) {
#if 0
  struct ws2812_pixel pixels[8] = {
    {0xff, 0x00, 0x00},
    {0x00, 0xff, 0x00},
    {0x00, 0x00, 0xff},
    {0xff, 0x00, 0x00},
    {0x00, 0xff, 0x00},
    {0x00, 0x00, 0xff},
    {0xff, 0x00, 0x00},
    {0x00, 0xff, 0x00}
  };
#else
  struct ws2812_pixel pixels[8] = {
    {0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
  };
#endif

  if (ws2812_init() != ESP_OK) {
    printf("Fail to init ws2812.\n");
    while (1) {
      vTaskDelay(1000 / portTICK_RATE_MS);
    }
  }

  while (1) {
    if (ws2812_set_pixels(pixels, 8) != ESP_OK) {
      printf("Fail to set pixels to ws2812.\n");
    } else {
      printf("Set pixels to ws2812 successful.\n");
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}
