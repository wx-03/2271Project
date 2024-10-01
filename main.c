#include "constants.h"

int main(void)
{
    SystemCoreClockUpdate();
    initLED();
    initUART2();
    while (1)
    {
        /* Rx and Tx*/
        uint8_t rx_data = UART2_Receive_Poll();
        if (rx_data == 0x30)
        { // Off Red LED
            PTB->PCOR |= MASK(RED_LED);
        }
        else if (rx_data == 0x31)
        { // On Red LED
            PTB->PSOR |= MASK(RED_LED);
        }
        else if (rx_data == 0x32)
        { // Off Green LED
            PTB->PCOR |= MASK(GREEN_LED);
        }
        else if (rx_data == 0x33)
        { // On Green LED
            PTB->PSOR |= MASK(GREEN_LED);
        }
        else if (rx_data == 0x34)
        { // Off Blue LED
            PTD->PCOR |= MASK(BLUE_LED);
        }
        else if (rx_data == 0x35)
        { // On Blue LED
            PTD->PSOR |= MASK(BLUE_LED);
        }
    }
}