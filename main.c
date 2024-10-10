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
#define PTB0_Pin 0
#define isMoving 0


uint8_t greenPins[] = {8, 9, 10, 11, 2, 3, 4, 5, 20, 21};

uint8_t pinsB[] = {8, 9, 10, 11};
uint8_t pinsE[] = {2, 3, 4, 5, 20, 21};
// uint8_t pinsC[] = {9};
uint32_t frequencies_mod[] = {1000};

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

void initPWM(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
  PORTB->PCR [PTB0_Pin] &= ~PORT_PCR_MUX_MASK; 
	PORTB->PCR [PTB0_Pin] |= PORT_PCR_MUX (3);
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; 
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  TPM1->MOD = 7499;
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK)); 
	TPM1->SC |= (TPM_SC_CMOD (1) | TPM_SC_PS (7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); 
	TPM1_C0SC |= (TPM_CnSC_ELSB (1) | TPM_CnSC_MSB (1));
  TPM1_C0V = 7499/2;
}

void change_frequency(int frequency)
{
  int mod_value = 375000/frequency - 1;
  TPM1->MOD = mod_value;
  TPM1_C0V = mod_value / 2;
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
		if (!isMoving)
		{
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PTOR |= MASK(greenPins[i]);
				} else {
					PTE->PTOR |= MASK(greenPins[i]);
				}
				osDelay(250U);
			}
		}
		else
		{
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PTOR |= MASK(greenPins[i]);
				} else {
					PTE->PTOR |= MASK(greenPins[i]);
				}
				osDelay(250U);
			}
		}
	}
}

void buzz_main(void *argument)
{
	for (;;) {
		change_frequency(442);
		osDelay(1000U);
		change_frequency(800);
		osDelay(1000U);
	}
}

int main(void)
{
  SystemCoreClockUpdate();
	initGPIO();
	initPWM();
	// int i = 0;
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(red_blinky_main, NULL, NULL);    // Create application main thread
	osThreadNew(green_blinky_main, NULL, NULL);    // Create application main thread
	osThreadNew(buzz_main, NULL, NULL);
  osKernelStart();                      // Start thread execution
 	for (;;) {}
}
