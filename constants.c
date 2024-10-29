#include "MKL25Z4.h"

#include "MKL25Z4.h"

#define BAUD_RATE 9600
#define TIMER_THRESHOLD 7499
#define UART2_INT_PRIO 128

// Pins
#define BUZZER_PIN 0                                      // PB0 - TPM1_CH0
#define UART_RX_PIN 23                                    // PE23
#define MOTOR_DRIVER_AIN1 2                               // PB2 - TPM2_CH0
#define MOTOR_DRIVER_AIN2 3                               // PB3 - TPM2_CH1
#define MOTOR_DRIVER_BIN1 1                               // PC1 - TPM0_CH0
#define MOTOR_DRIVER_BIN2 2                               // PC2 - TPM0_CH1
#define RLED 29                                           // PE29

#define MASK(x) (1 << (x))

static void delay(volatile uint32_t nof)
{
    while (nof != 0)
    {
        __asm("NOP");
        nof--;
    }
}
