#ifndef _WS2811_H_
#define _WS2811_H_

#include <stdint.h>

#define DISPLAY_WIDTH           16
#define DISPLAY_HEIGHT          16

void ws2811_init(void);
void ws2811_set_led_color(int led, uint32_t color);
void ws2811_all_off(void);
void ws2811_set_all(uint32_t color);
void ws2811_set_brightness(uint32_t br);
uint32_t ws2811_get_brightness(void);
void ws2811_set_pixel(int x, int y, int color);

#endif
