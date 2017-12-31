#include "ws2811.h"

#include "stm32f4xx.h"
#include <math.h>

#define WS2811_CLK_HZ       800000
#define WS2811_LED_NUM      (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define TIM_PERIOD          (((180000000 / 2 / WS2811_CLK_HZ) - 1))
#define LED_BUFFER_LEN      (WS2811_LED_NUM)
#define BITBUFFER_PAD       50
#define BITBUFFER_LEN       (24 * LED_BUFFER_LEN + BITBUFFER_PAD)
#define WS2811_ZERO         (TIM_PERIOD * 0.2)
#define WS2811_ONE          (TIM_PERIOD * 0.8)

static uint32_t bitbuffer[BITBUFFER_LEN];
static uint8_t gamma_table[256];
static uint32_t brightness;
static TIM_HandleTypeDef htim2;
static DMA_HandleTypeDef hdma_tim2_ch1;

static uint32_t rgb_to_local(uint32_t color);

void ws2811_init()
{
    brightness = 100;

    // generate gamma table
    for(int i = 0; i < 256; i++)
        gamma_table[i] = (int)roundf(powf((float)i / 255.0, 1.0 / 0.45) * 255.0);

    // update PWM DMA buffer based on framebuffer
    for(int i = 0; i < LED_BUFFER_LEN; i++)
    {
        uint32_t tmp_color = rgb_to_local(0x00000000);

        for(int bit = 0; bit < 24; bit++)
        {
            if(tmp_color & (1 << 23))
                bitbuffer[bit + i * 24] = WS2811_ONE;
            else
                bitbuffer[bit + i * 24] = WS2811_ZERO;

            tmp_color <<= 1;
        }
    }

    // zero padding buffer for the LEDs to recognize the end of a transfer
    for(int i = 0; i < BITBUFFER_PAD; i++)
        bitbuffer[BITBUFFER_LEN - BITBUFFER_PAD - 1 + i] = 0;

    // Enable clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    // Register DMA interrupt handler
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    // Initialize base timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = TIM_PERIOD;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim2);

    // Setup PWM mode
    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);

    // Setup the DMA engine
    hdma_tim2_ch1.Instance = DMA1_Stream5;
    hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;
    hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_tim2_ch1);
    __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);

    // Setup output comare pin on PA5
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Start DMA
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)bitbuffer, BITBUFFER_LEN);
}

void ws2811_set_led_color(int led, uint32_t color)
{
    if(led >= 0 && led < WS2811_LED_NUM)
    {
        color = rgb_to_local(color);

        for(int bit = 0; bit < 24; bit++) {
            if(color & (1 << 23))
                bitbuffer[bit + led * 24] = WS2811_ONE;
            else
                bitbuffer[bit + led * 24] = WS2811_ZERO;

            color <<= 1;
        }
    }
}

void ws2811_all_off()
{
    for(int i = 0; i < (WS2811_LED_NUM * 24); i++)
        bitbuffer[i] = WS2811_ZERO;
}

void ws2811_set_all(uint32_t color)
{
    for(int i = 0; i < WS2811_LED_NUM; i++)
    {
        uint32_t tmp_color = rgb_to_local(color);

        for(int bit = 0; bit < 24; bit++)
        {
            if(tmp_color & (1 << 23))
                bitbuffer[bit + i * 24] = WS2811_ONE;
            else
                bitbuffer[bit + i * 24] = WS2811_ZERO;

            tmp_color <<= 1;
        }
    }
}

void ws2811_set_brightness(uint32_t br)
{
    brightness = br;
}

uint32_t ws2811_get_brightness()
{
    return brightness;
}

void ws2811_set_pixel(int x, int y, int color)
{
    // handle odd colums in the "snake pattern"
    if(x % 2 != 0)
        y = (DISPLAY_HEIGHT-1) - y;

    int lednum = y + x * DISPLAY_HEIGHT;

    ws2811_set_led_color(lednum, color);
}

static uint32_t rgb_to_local(uint32_t color)
{
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t b = color & 0xFF;

    r = (r * brightness) / 100;
    g = (g * brightness) / 100;
    b = (b * brightness) / 100;

    r = gamma_table[r];
    g = gamma_table[g];
    b = gamma_table[b];

    return (g << 16) | (r << 8) | b;
}

void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_tim2_ch1);
}
