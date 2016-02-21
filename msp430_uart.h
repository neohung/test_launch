#include "msp430g2553.h"
#include "stdbool.h"
static volatile char data;

void uart_init(void (*callback_fun)(char ch));
void set_uart_rx_cb(void (*callback_fun)(char ch));

void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength);
