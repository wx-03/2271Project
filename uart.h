#include "constants.h"

// UART2 Initialisation
void initUART2(void)
{
    uint32_t divisor, bus_clock;
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
    bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
    divisor = bus_clock / (BAUD_RATE * 16);
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL;
    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;
    UART2->C2 |= UART_C2_RE_MASK;
}

// UART2 Receive Poll
uint8_t UART2_Receive_Poll(void)
{
    while (!(UART2->S1 & UART_S1_RDRF_MASK))
        ;
    return (UART2->D);
}