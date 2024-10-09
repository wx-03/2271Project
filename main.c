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
#define blinkyDelay 2000000

int initGPIO() {
	// Enable Clock to PORTB and PORTD C

	SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK));
	// Configure MUX settings to make all 3 pins GPIO

	PORTC->PCR[G1] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G1] |= PORT_PCR_MUX(1);
	PORTC->PCR[G2] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G2] |= PORT_PCR_MUX(1);
	PORTC->PCR[G3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G3] |= PORT_PCR_MUX(1);
	PORTC->PCR[G4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G4] |= PORT_PCR_MUX(1);
	PORTC->PCR[G5] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G5] |= PORT_PCR_MUX(1);
	PORTD->PCR[G6] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[G6] |= PORT_PCR_MUX(1);
	PORTA->PCR[G7] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[G7] |= PORT_PCR_MUX(1);
	PORTA->PCR[G8] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[G8] |= PORT_PCR_MUX(1);
	PORTA->PCR[G9] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[G9] |= PORT_PCR_MUX(1);
	PORTC->PCR[G10] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[G10] |= PORT_PCR_MUX(1);
	PORTA->PCR[RLED] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RLED] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for A PortC and PortD
	PTD->PDDR |= MASK(G6);
	PTC->PDDR |= MASK(G1) | MASK(G2) | MASK(G3) | MASK(G4) | MASK(G5) | MASK(G10);
	PTA->PDDR |= MASK(G7) | MASK(G8) | MASK(G9) | MASK(RLED);
	//sets
	//PTD->PSOR |= MASK(G1);
	PTC->PCOR |= MASK(G1);
	PTA->PDOR |= MASK(RLED);
	PTC->PDOR |= MASK(G10);
}

void tG1()
{
	PTD->PTOR |= MASK(G1);
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
	PTD->PTOR |= MASK(G10);
}

int main(void)
{
	

  SystemCoreClockUpdate();
  initLED();
	initUART2();
	initGPIO();
  while (1)
    {
			delay(blinkyDelay);
			PTC->PTOR |= MASK(G1);
			
			/*delay(blinkyDelay);
			tG2();
			delay(blinkyDelay);
			tG3();
			delay(blinkyDelay);
			tG4();
			delay(blinkyDelay);
			tG5();
			delay(blinkyDelay);
			tG6();
			delay(blinkyDelay);
			tG7();
			delay(blinkyDelay);
			tG8();
			delay(blinkyDelay);
			tG9();
			delay(blinkyDelay);
			tG10();
			delay(blinkyDelay);
			*/
		
    }
}
