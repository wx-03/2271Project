#include "constants.c"

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#include "led.h"
#include "uart.h"
#define RLED 29 // portE pin 29
#define CLOCK_SETUP 1
#define BUZZER_PIN 0
#define LEFT_MOTOR_PIN 0
#define RIGHT_MOTOR_PIN 1
#define isMoving 0
#define TIMER_THRESHOLD 7499

osSemaphoreId_t brainSem;
osSemaphoreId_t motorSem;
float leftDc = 0;
float rightDc = 0;

uint8_t greenPins[] = {8, 9, 10, 11, 2, 3, 4, 5, 20, 21};

uint8_t pinsB[] = {8, 9, 10, 11};
uint8_t pinsE[] = {2, 3, 4, 5, 20, 21};
// uint8_t pinsC[] = {9};
uint32_t frequencies_mod[] = {1000};

void initGPIO()
{
	// Enable Clock to PORTB and PORTD C

	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTE_MASK));
	// Configure MUX settings to make all 3 pins GPIO

	uint8_t i = 0;
	for (; i < 4; i++)
	{
		int pinNo = greenPins[i];
		PORTB->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[pinNo] |= PORT_PCR_MUX(1);
	}
	for (; i < 10; i++)
	{
		int pinNo = greenPins[i];
		PORTE->PCR[pinNo] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[pinNo] |= PORT_PCR_MUX(1);
	}

	PORTE->PCR[RLED] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RLED] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for A PortC and PortD

	i = 0;
	for (; i < 4; i++)
	{
		int pinNo = greenPins[i];
		PTB->PDDR |= MASK(pinNo);
	}
	for (; i < 10; i++)
	{
		int pinNo = greenPins[i];
		PTE->PDDR |= MASK(pinNo);
	}

	PTE->PDDR |= MASK(RLED);
	// sets
	for (int j = 0; j < 10; j++)
	{
		int pinNo = greenPins[i];
		if (0 <= j && j <= 3)
		{
			PTB->PCOR |= MASK(pinNo);
		}
		else
		{
			PTE->PCOR |= MASK(pinNo);
		}
	}
	PTE->PCOR |= MASK(RLED);
}

void initMotorPWM(void)
{
	PORTB->PCR[LEFT_MOTOR_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_MOTOR_PIN] |= PORT_PCR_MUX(3);
	PORTB->PCR[RIGHT_MOTOR_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_MOTOR_PIN] |= PORT_PCR_MUX(3);
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	// SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	// SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	TPM2->MOD = TIMER_THRESHOLD;
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM2_C0V = 0;
	TPM2_C1V = 0;
}

void initBuzzerPWM(void)
{
	PORTB->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[BUZZER_PIN] |= PORT_PCR_MUX(3);
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	TPM1->MOD = TIMER_THRESHOLD;
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM1_C0V = TIMER_THRESHOLD / 2;
}

void change_frequency(int frequency)
{
	int mod_value = 375000 / frequency - 1;
	TPM1->MOD = mod_value;
	TPM1_C0V = mod_value / 2;
}

void red_blinky_main(void *argument)
{
	for (;;)
	{
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
	for (;;)
	{
		if (!isMoving)
		{
			for (int i = 0; i < 10; i++)
			{
				if (0 <= i && i <= 3)
				{
					PTB->PTOR |= MASK(greenPins[i]);
				}
				else
				{
					PTE->PTOR |= MASK(greenPins[i]);
				}
				osDelay(250U);
			}
		}
		else
		{
			for (int i = 0; i < 10; i++)
			{
				if (0 <= i && i <= 3)
				{
					PTB->PTOR |= MASK(greenPins[i]);
				}
				else
				{
					PTE->PTOR |= MASK(greenPins[i]);
				}
				osDelay(250U);
			}
		}
	}
}

void buzz_main(void *argument)
{
	for (;;)
	{
		change_frequency(442);
		osDelay(1000U);
		change_frequency(800);
		osDelay(1000U);
	}
}

void brain_main(void *argument)
{
	for (;;)
	{
		osSemaphoreAcquire(brainSem, osWaitForever);
		leftDc = ((uartData >> 4) - 7) / 8;
		rightDc = ((uartData &= 00001111) - 7) / 8;
		osSempaphoreRelease(motorSem)
	}
}

void motor_main(void *argument)
{
	for (;;)
	{
		osSemaphoreAcquire(motorSem, osWaitFoerver);
		TPM2_C0V = TIMER_THRESHOLD * leftDc;
		TPM2_C1V = TIMER_THRESHOLD * rightDc;
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	initGPIO();
	initBuzzerPWM;
	initMotorPWM();
	brainSem = osSemaphoreNew(1, 0, NULL);
	// int i = 0;

	osKernelInitialize();						// Initialize CMSIS-RTOS
	osThreadNew(red_blinky_main, NULL, NULL);	// Create application main thread
	osThreadNew(green_blinky_main, NULL, NULL); // Create application main thread
	osThreadNew(buzz_main, NULL, NULL);
	osKernelStart(); // Start thread execution
	for (;;)
	{
	}
}
