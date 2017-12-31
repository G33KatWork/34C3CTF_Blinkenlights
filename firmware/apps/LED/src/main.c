#include "stm32f4xx.h"

#include "clocks.h"
#include "uart.h"
#include "ws2811.h"
#include "font.h"
#include "hsv.h"

#include <string.h>

//#define RAINBOW_BRIGHTNESS 100.0
//#define DISPLAY_BRIGHTNESS 100

#define RAINBOW_BRIGHTNESS 50.0
#define DISPLAY_BRIGHTNESS 100

int main(int argc, char const *argv[])
{
    HAL_Init();
    SystemClock_Config();

    ws2811_init();
    ws2811_set_brightness(DISPLAY_BRIGHTNESS);
    ws2811_all_off();

    uart_init();
    iprintf("Welcome to flag printer 9000\n");
    iprintf("Enter flag to display and press enter: \n");

    char string[129] = {0};
    iscanf("%128s", string);
    //const char* string = "34C3_relaxen_und_watchen_das_blinkenlichten";

    iprintf("Displaying text: %s\n", string);

    int strwidth = font_get_width(string);
    int offset = -DISPLAY_WIDTH;

    int tick = 0;
    while(1)
    {
        for(int x = 0; x < DISPLAY_WIDTH; x++)
        {
            for(int y = 0; y < DISPLAY_HEIGHT; y++)
            {
                uint32_t rgb;
                float h = (5*tick + 10*y + 10*x) % 360;

                hsv_to_rgb(h, 100.0, RAINBOW_BRIGHTNESS, &rgb);
                ws2811_set_pixel(x, y, rgb);
            }
        }

        draw_string(string, 4, offset, 0xFFFFFF);

        if(tick % 5 == 0)
        {
            offset++;

            if(offset >= strwidth)
                offset = -DISPLAY_WIDTH;
        }

        tick++;
        HAL_Delay(10);
    }

    return 0;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
    iprintf("Assertion failed at %s:%d\n", file, line);
    while (1)
    {
    }
}
#endif
