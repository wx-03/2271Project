#include "constants.c"
#include "led.h"
#include "uart.h"
#define G1 3 //portC pin 3
#define G2 4 //portC pin 4
#define G3 5 //portC pin 5
#define G4 6 //portC pin 6
#define G5 10 //portC pin 10
#define G6 4 //portD pin 4
#define G7 12 //portA pin 12
#define G8 4 //portA pin 4
#define G9 5 //portA pin 5
#define G10 8 //portC pin 8
#define RLED 13 //portA pin 13

uint8_t greenPins[] = {3, 4, 5, 6, 10, 4, 12, 4, 5, 8};
uint8_t GreenPortC1[] = {3, 4, 5, 6, 10};
uint8_t GreenPortD[] = {4};
uint8_t GreenPortA[] = {12, 4, 5};
uint8_t GreenPortC2[] = {8};

int initGPIO() {
	// Enable Clock to PORTB and PORTD C

	SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK));
	// Configure MUX settings to make all 3 pins GPIO
	
	int i = 0;
	for (; i < 5; i++) {
		int pinNo = greenPins[i];
		PORTC->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	for (; i < 1; i++) {
		int pinNo = greenPins[i];
		PORTD->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTD->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	for (; i < 3; i++) {
		int pinNo = greenPins[i];
		PORTA->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTA->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	for (; i < 1; i++) {
		int pinNo = greenPins[i];
		PORTC->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	
	PORTA->PCR[RLED] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RLED] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for A PortC and PortD
	PTD->PDDR |= MASK(G6) | MASK(RLED);
	PTC->PDDR |= MASK(G1) | MASK(G2) | MASK(G3) | MASK(G4) | MASK(G5) | MASK(G10);
	PTA->PDDR |= MASK(G7) | MASK(G8) | MASK(G9);
	//sets
	//PTD->PSOR |= MASK(G1);
	PTC->PCOR |= MASK(G1);
	PTD->PSOR |= MASK(RLED);
}

void tG1()
{
	PTC->PTOR |= MASK(G1);
}

void tG2()
{
	PTC->PTOR |= MASK(G2);
}

void tG3()
{
	PTC->PTOR |= MASK(G3);
}

void tG4()
{
	PTC->PTOR |= MASK(G4);
}

void tG5()
{
	PTC->PTOR |= MASK(G5);
}

void tG6()
{
	PTD->PTOR |= MASK(G6);
}

void tG7()
{
	PTA->PTOR |= MASK(G7);
}

void tG8()
{
	PTA->PTOR |= MASK(G8);
}

void tG9()
{
	PTA->PTOR |= MASK(G9);
}

void tG10()
{
	PTC->PTOR |= MASK(G10);
}

int main(void)
{
	

  SystemCoreClockUpdate();
  initLED();
	initUART2();
	initGPIO();
  while (1)
    {
			delay(200000);
			PTC->PTOR |= MASK(G1);
			
			delay(200000);
			tG2();
			delay(200000);
			tG3();
			delay(200000);
			tG4();
			/*
			delay(2000);
			tG5();
			delay(2000);
			tG6();
			delay(2000);
			tG7();
			delay(2000);
			tG8();
			delay(2000);
			tG9();
			delay(2000);
			tG10();
			delay(2000);
			PTD->PTOR |= MASK(RLED);
			*/
				
    }
}
