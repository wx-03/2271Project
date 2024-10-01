#include "constants.c"
#include "led.h"
#include "uart.h"

int main(void)
{
    SystemCoreClockUpdate();
    initLED();
    initUART2();
    while (1)
    {
        flashLed();
        // /* Rx and Tx*/
        // uint8_t rx_data = UART2_Receive_Poll();
        // if (rx_data == 0x30)
        //     PTB->PCOR |= MASK(RED_LED);
		
        // else if (rx_data == 0x31)
        // { // On Red LED
        //     PTB->PSOR |= MASK(RED_LED);
        // }
        // else if (rx_data == 0x32)
        // { // Off Green LED
        //     PTB->PCOR |= MASK(GREEN_LED);
        // }
        // else if (rx_data == 0x33)
        // { // On Green LED
        //     PTB->PSOR |= MASK(GREEN_LED);
        // }
        // else if (rx_data == 0x34)
        // { // Off Blue LED
        //     PTD->PCOR |= MASK(BLUE_LED);
        // }
        // else if (rx_data == 0x35)
        // { // On Blue LED
        //     PTD->PSOR |= MASK(BLUE_LED);
        // }
    }
}

static void delay(volatile uint32_t nof)
{
    while (nof != 0)
    {
        __asm("NOP");
        nof--;
    }
}