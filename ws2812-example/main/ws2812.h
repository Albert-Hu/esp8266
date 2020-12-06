#ifndef _WS2812_H_
#define _WS2812_H_

#include <stdint.h>

struct ws2812_pixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

int ws2812_set_pixels(struct ws2812_pixel *pixels, size_t num);
int ws2812_init();

#endif /* _WS2812_H_ */
