#include "constants.c"
#include "led.h"
#include "uart.h"
#define RLED 29 //portE pin 29
#define CLOCK_SETUP 1

uint8_t greenPins[] = {8, 9, 10, 11, 2, 3, 4, 5, 20, 21};

uint8_t pinsB[] = {8, 9, 10, 11};
uint8_t pinsE[] = {2, 3, 4, 5, 20, 21};
// uint8_t pinsC[] = {9};

int initGPIO() {
	// Enable Clock to PORTB and PORTD C

	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTE_MASK));
	// Configure MUX settings to make all 3 pins GPIO
	
	
	uint8_t i = 0;
	for (; i < 4; i++) {
		int pinNo = greenPins[i];
		PORTB->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	for (; i < 10; i++) {
		int pinNo = greenPins[i];
		PORTE->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	
	PORTE->PCR[RLED] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RLED] |= PORT_PCR_MUX(1);
	

	// Set Data Direction Registers for A PortC and PortD
	
	i = 0;
	for (; i < 4; i++) {
		int pinNo = greenPins[i];
		PTB->PDDR |= MASK(pinNo);
	}
	for (; i < 10; i++) {
		int pinNo = greenPins[i];
		PTE->PDDR |= MASK(pinNo);
	}
	
	PTE->PDDR |= MASK(RLED);
	//sets
	for (int j = 0; j < 10; j++) {
		int pinNo = greenPins[i];
		if (0 <= j && j <= 3) {
			PTB->PCOR |= MASK(pinNo);
		} else {
			PTE->PCOR |= MASK(pinNo);
		}
	}
	PTE->PCOR |= MASK(RLED);
}


int main(void)
{
	

  SystemCoreClockUpdate();
  initLED();
	initUART2();
	initGPIO();
	// int i = 0;
  while (1)
    {
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PTOR |= MASK(greenPins[i]);
				} else {
					PTE->PTOR |= MASK(greenPins[i]);
				}
				if (i == 1) {
					PTE->PTOR |= MASK(RLED);
				}
				delay(120000);
			}
    }
}
