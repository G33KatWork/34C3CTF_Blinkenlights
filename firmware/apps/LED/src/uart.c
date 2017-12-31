#include "uart.h"

#include "stm32f4xx.h"

#include <string.h>

static UART_HandleTypeDef uart1;

#define ENABLE_ECHO 1

void uart_init()
{
    uart1.Instance = USART1;
    uart1.Init.BaudRate = 115200;
    uart1.Init.WordLength = UART_WORDLENGTH_8B;
    uart1.Init.StopBits = UART_STOPBITS_1;
    uart1.Init.Parity = UART_PARITY_NONE;
    uart1.Init.Mode = UART_MODE_TX_RX;
    uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&uart1);
}

void uart_send(const char* buf, int nbytes)
{
    HAL_UART_Transmit(&uart1, (char*)buf, nbytes, HAL_MAX_DELAY);
}

int uart_recv(char* buf, int nbytes)
{
    int i;
    for(i = 0; i < nbytes; i++)
    {
        while(HAL_UART_Receive(&uart1, &buf[i], 1, 0) != HAL_OK);

#ifdef ENABLE_ECHO
        uart_send(&buf[i], 1);
#endif

        if(buf[i] == '\n')
            return i + 1;
    }

    return i + 1;
}

void HAL_UART_MspInit(UART_HandleTypeDef *uart)
{
    if(uart->Instance==USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
            .Pin = GPIO_PIN_9 | GPIO_PIN_10,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = GPIO_AF7_USART1
        });
    }
}
