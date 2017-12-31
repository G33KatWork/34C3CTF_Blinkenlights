#ifndef _UART_H_
#define _UART_H_

void uart_init(void);
void uart_send(const char* buf, int nbytes);
int uart_recv(char* buf, int nbytes);

#endif
