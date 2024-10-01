#include "MKL25Z4.h"

#define RED_LED 18   // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1   // PortD Pin 1
#define BAUD_RATE 9600
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

#define MASK(x) (1 << (x))

static void delayy(volatile uint32_t nof)
{
    while (nof != 0)
    {
        __asm("NOP");
        nof--;
    }
}
