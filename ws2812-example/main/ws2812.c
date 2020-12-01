#include <stdlib.h>
#include <string.h>

#include "esp8266/spi_struct.h"
#include "driver/spi.h"

#include "ws2812.h"

#define R(bitmap) (((bitmap) & 0xff0000) >> 16)
#define G(bitmap) (((bitmap) & 0xff00) >> 8)
#define B(bitmap) ((bitmap) & 0xff)

static void _reverse_byte_order(uint32_t *data, size_t len) {
  while (len-- > 0) {
    *data = ((*data & 0xff000000) >> 24)
          + ((*data & 0x00ff0000) >> 8)
          + ((*data & 0x0000ff00) << 8)
          + ((*data & 0x000000ff) << 24);
    data++;
  }
}

static void _convert(uint8_t *bytes, uint8_t bitmap) {
  uint8_t mask = 0x80;
  
  while (mask) {
    /*
      0xfc = b11111100, set bit to 1
      0xc0 = b11000000, set bit to 0
    */
    *bytes++ = (bitmap & mask) ? 0xfc : 0xc0;
    mask >>= 1;
  }
}

int _set_pixel(uint32_t pixel) {
  uint8_t pixel_bytes[24];
  spi_trans_t trans;

  memset(&trans, 0, sizeof(spi_trans_t));

  _convert(pixel_bytes, G(pixel));
  _convert(pixel_bytes + 8, R(pixel));
  _convert(pixel_bytes + 16, B(pixel));

  _reverse_byte_order((uint32_t*) pixel_bytes, 6);

  trans.mosi = (uint32_t*) pixel_bytes;
  trans.bits.mosi = 24 * 8;

  return spi_trans(HSPI_HOST, trans);
}

int ws2812_reset() {
  uint32_t empty = 0;
  spi_trans_t trans;

  memset(&trans, 0, sizeof(spi_trans_t));
  trans.mosi = &empty;
  trans.bits.mosi = 8;

  return spi_trans(HSPI_HOST, trans);
}

int ws2812_set_pixels(uint32_t *pixels, uint32_t len) {
  uint32_t i, offset;
  uint8_t *pixel_bytes = NULL;
  spi_trans_t trans;

  if (!(pixel_bytes = (uint8_t*) malloc(len * 24))) {
    return -1;
  }

  memset(&trans, 0, sizeof(spi_trans_t));
  trans.bits.mosi = 8 * 24;

  for (i = 0; i < len; i++) {
    offset = i * 24;
    _convert(pixel_bytes + offset, G(pixels[i]));
    _convert(pixel_bytes + offset + 8, R(pixels[i]));
    _convert(pixel_bytes + offset + 16, B(pixels[i]));
    _reverse_byte_order((uint32_t*) (pixel_bytes + offset), 6);
    trans.mosi = (uint32_t*) (pixel_bytes + offset);
    spi_trans(HSPI_HOST, trans);
  }

  free(pixel_bytes);

  return 0;
}

int ws2812_init() {
  spi_config_t spi_config;

  // Load default interface parameters
  // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1,
  // BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
  spi_config.interface.val = SPI_DEFAULT_INTERFACE;

  // Load default interrupt enable
  // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER:
  // false, READ_BUFFER: false
  spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;

  // Cancel hardware cs
  spi_config.interface.cs_en = 0;

  // MISO pin is used for DC
  spi_config.interface.miso_en = 0;

  // CPOL: 1, CPHA: 1
  spi_config.interface.cpol = 1;
  spi_config.interface.cpha = 1;

  // Set SPI to master mode
  // 8266 Only support half-duplex
  spi_config.mode = SPI_MASTER_MODE;

  // Set the SPI clock frequency division factor
  spi_config.clk_div = SPI_8MHz_DIV;

  // Register SPI event callback function
  spi_config.event_cb = NULL;
  
  return spi_init(HSPI_HOST, &spi_config);
}
