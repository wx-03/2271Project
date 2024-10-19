#include "constants.c"

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#include "led.h"
#include "uart.h"
#define RLED 29 //portE pin 29
#define CLOCK_SETUP 1
#define BUZZ 12
#define isMoving 1


const int c = 523;
const int d = 587;
const int e = 659;
const int f = 698;
const int g = 784;
const int a = 880;
const int b = 988;
const int C = 1047;

uint8_t greenPins[] = {8, 9, 10, 11, 2, 3, 4, 5, 20, 21};

uint8_t pinsB[] = {8, 9, 10, 11};
uint8_t pinsE[] = {2, 3, 4, 5, 20, 21};
uint32_t frequencies_mod[] = {1000};

// Set up the sequence of notes and their durations in milliseconds
const int melody[] = { c, c, g, g, a, a, g, f, f, e, e, d, d, c, g, g, f, f, e, e, d, g, g, f, f, e, e, d, c, c, g, g, a, a, g, f, f, e, e, d, d, c };
int noteDurations[] = { 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000 };
int melodySize = sizeof(melody) / sizeof(melody[0]);

void initGPIO() {
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
	

	// Set Data Direction Registers for PortB and PortE
	
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
	
	// Clear all pins
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

void initPWM(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  PORTA->PCR [BUZZ] &= ~PORT_PCR_MUX_MASK; 
	PORTA->PCR [BUZZ] |= PORT_PCR_MUX (3);
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; 
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  TPM1->MOD = 7499;
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK)); 
	TPM1->SC |= (TPM_SC_CMOD (1) | TPM_SC_PS (7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); 
	TPM1_C0SC |= (TPM_CnSC_ELSB (1) | TPM_CnSC_MSB (1));
  TPM1_C0V = 7499/16;
}

void change_frequency(int frequency)
{
  int mod_value = 375000/frequency - 1;
  TPM1->MOD = mod_value;
  TPM1_C0V = mod_value / 16;
}

void red_blinky_main(void *argument)
{
	for (;;) {
		PTE->PTOR |= MASK(RLED);
		if (isMoving)
		{
			osDelay(500U);
		}
		else
		{
			osDelay(250U);
		}
	}
}

void green_blinky_main(void *argument)
{
	for (;;) {
		if (isMoving)
		{
			// Running Mode
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PTOR |= MASK(greenPins[i]);
				} else {
					PTE->PTOR |= MASK(greenPins[i]);
				}
				osDelay(250U);	
				if (0 <= i && i <= 3) {
					PTB->PTOR |= MASK(greenPins[i]);
				} else {
					PTE->PTOR |= MASK(greenPins[i]);
				}
			}	
		}
		else
		{
			// Light up all
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PSOR |= MASK(greenPins[i]);
				} else {
					PTE->PSOR |= MASK(greenPins[i]);
				}
			}			
		}
	}
}



void buzz_main(void *argument)
{
	int i = 0;
	
	int major[] = {c, d ,e ,f ,g, a, b, C};
	for (;;) {
		change_frequency(melody[i]);
		osDelay(noteDurations[i]);
		// change_frequency(0);
		// osDelay(5);
		i = (i + 1) % (sizeof(melody) / sizeof(melody[0]));
	}
}

int main(void)
{
  SystemCoreClockUpdate();
	initGPIO();
	initPWM();
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(red_blinky_main, NULL, NULL);    // Create application main thread
	osThreadNew(green_blinky_main, NULL, NULL);    // Create application main thread
	osThreadNew(buzz_main, NULL, NULL);
  osKernelStart();                      // Start thread execution
 	for (;;) {}
}
