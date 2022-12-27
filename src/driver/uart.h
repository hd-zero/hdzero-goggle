#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>
#define UART_PORTS  4

int     uart_open(int port);
int     uart_set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
int 	uart_read_byte(int fd, uint8_t *data);
int     uart_write_byte(int fd, uint8_t data);
void	uart_close(int fd);
int     uart_read(int fd, uint8_t* data, int len);
int     uart_write(int fd, uint8_t* data, int len);
#endif // __UART_H_
