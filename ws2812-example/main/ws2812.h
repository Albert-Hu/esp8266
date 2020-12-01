#ifndef _WS2812_H_
#define _WS2812_H_

#include <stdint.h>

#define COLOR(r, g, b) (((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff))

int ws2812_init();
int ws2812_reset();
int ws2812_set_pixels(uint32_t *pixels, uint32_t len);

#endif /* _WS2812_H_ */
