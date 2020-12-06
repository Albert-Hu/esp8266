#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp8266/pin_mux_register.h"
#include "ws2812.h"

#define SAMPLE_RATE (100000)
#define I2S_NUM (0)
#define I2S_BCK_EN (0)
#define I2S_WS_EN (0)
#define I2S_DO_EN (1)
#define I2S_DI_EN (0)

struct __attribute__ ((packed)) pixel_buf {
  uint8_t bytes[12]; /* 1 pixel in 12 bytes */
};

static uint8_t _bit_patterns[4] = {
  0x88, // b1000_1000, b00
  0x8c, // b1000_1100, b01
  0xc8, // b1100_1000, b10
  0xcc  // b1100_1100, b11
};

void _set_buf(uint8_t bitmap, uint8_t *buf) {
  buf[0] = _bit_patterns[(bitmap >> 6) & 0x03];
  buf[1] = _bit_patterns[(bitmap >> 4) & 0x03];
  buf[2] = _bit_patterns[(bitmap >> 2) & 0x03];
  buf[3] = _bit_patterns[bitmap & 0x03];
}

void _set_pixel(struct ws2812_pixel *pixel, struct pixel_buf *buf) {
  /*
    The order of color of ws2812 is GRB
  */
  _set_buf(pixel->g, &buf->bytes[0]);
  _set_buf(pixel->r, &buf->bytes[4]);
  _set_buf(pixel->b, &buf->bytes[8]);
}

int ws2812_set_pixels(struct ws2812_pixel *pixels, size_t num) {
  int result = ESP_OK;
  size_t i, i2s_bytes_write = 0;
  const size_t buf_size = sizeof(struct pixel_buf) * num;
  struct pixel_buf *buf = NULL;

  do {
    if (!(buf = (struct pixel_buf*) malloc(buf_size))) {
      result = -1;
      break;
    }

    memset(buf, 0, buf_size);

    for (i = 0; i < num; i++) {
      _set_pixel(&pixels[i], &buf[i]);
    }

    result = i2s_write(I2S_NUM, buf, buf_size, &i2s_bytes_write, 100);
    if (result != ESP_OK) break;

    printf("%d bytes has written.\n", i2s_bytes_write);

    i2s_zero_dma_buffer(I2S_NUM);

    free(buf);
  } while (0);

  if (buf) free(buf);

  return result;
}

int ws2812_init() {
  /*
    Sample Rate: 200k
    Bits per Sample: 16
    Bit Rate: 3.2 Mbps
  */
  int result = ESP_OK;
  i2s_config_t i2s_config = {
      .mode = I2S_MODE_MASTER | I2S_MODE_TX, // Only TX
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // 2-channels
      .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
      .dma_buf_count = 4,
      .dma_buf_len = 96};
  i2s_pin_config_t pin_config = {.bck_o_en = I2S_BCK_EN,
                                 .ws_o_en = I2S_WS_EN,
                                 .data_out_en = I2S_DO_EN,
                                 .data_in_en = I2S_DI_EN};

  do {
    result = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    if (result != ESP_OK) break;

    result = i2s_set_pin(I2S_NUM, &pin_config);
    if (result != ESP_OK) break;
  } while (0);

  return result;
}
