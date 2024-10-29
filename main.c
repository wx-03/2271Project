#include "constants.c"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#define RLED 29 // portE pin 29
#define CLOCK_SETUP 1
#define BUZZ 12


#define c 523
#define d 587
#define e 659
#define f 698
#define g 784
#define a 880
#define b 988
#define C 1047

uint8_t greenPins[] = {8, 9, 10, 11, 2, 3, 4, 5, 20, 21};
osSemaphoreId_t brainSem;
osSemaphoreId_t motorSem;
volatile uint8_t uartData = 0x77;
volatile static float leftDc = 0.0;
volatile static float rightDc = 0.0;
volatile static uint8_t isMoving = 0;
volatile static int counter = 0;
volatile static uint8_t isDone = 0;

uint32_t frequencies_mod[] = {1000};

// Set up the sequence of notes and their durations in milliseconds
const int melody[] = {c, c, g, g, a, a, g, f, f, e, e, d, d, c, g, g, f, f, e, e, d, g, g, f, f, e, e, d, c, c, g, g, a, a, g, f, f, e, e, d, d, c};
int noteDurations[] = {500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000};
int melodySize = sizeof(melody) / sizeof(melody[0]);

// uint8_t pinsC[] = {9};
// uint32_t frequencies_mod[] = {1000};

const osThreadAttr_t priorityHigh = {
  .priority = osPriorityHigh};
const osThreadAttr_t priorityMax = {
  .priority = osPriorityRealtime};

void initLedGpio()
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

  // Set Data Direction Registers for PortB and PortE

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

  // Clear all pins
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
}

// UART2 Initialisation
void initUART2(void)
{
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[UART_RX_PIN] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PIN] |= PORT_PCR_MUX(4);
  // PORTE->PCR[22] &= ~PORT_PCR_MUX_MASK;
  // PORTE->PCR[22] |= PORT_PCR_MUX(4);
  UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK);

  uint32_t bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
  uint32_t divisor = bus_clock / (BAUD_RATE * 16);
  UART2->BDH = UART_BDH_SBR(divisor >> 8);
  UART2->BDL = UART_BDL_SBR(divisor);
  UART2->C1 = 0;
  UART2->S2 = 0;
  UART2->C3 = 0;
  UART2->C2 |= (UART_C2_RE_MASK | UART_C2_RIE_MASK);
  NVIC_SetPriority(UART2_IRQn, 128);
  NVIC_ClearPendingIRQ(UART2_IRQn);
  NVIC_EnableIRQ(UART2_IRQn);
}

void UART2_IRQHandler()
{
  NVIC_ClearPendingIRQ(UART2_IRQn);
  if (UART2_S1 & UART_S1_RDRF_MASK)
  {
    uartData = UART2->D;
		osSemaphoreRelease(brainSem);
  }
  // UART2->D = uartData;
}

void initBuzzerPWM(void)
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

void initMotorPWM(void)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTB->PCR[MOTOR_DRIVER_AIN1] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[MOTOR_DRIVER_AIN1] |= PORT_PCR_MUX(3);
  PORTB->PCR[MOTOR_DRIVER_AIN2] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[MOTOR_DRIVER_AIN2] |= PORT_PCR_MUX(3);
  PORTC->PCR[MOTOR_DRIVER_BIN1] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[MOTOR_DRIVER_BIN1] |= PORT_PCR_MUX(4);
  PORTC->PCR[MOTOR_DRIVER_BIN2] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[MOTOR_DRIVER_BIN2] |= PORT_PCR_MUX(4);

  SIM->SCGC6 |= (SIM_SCGC6_TPM2_MASK | SIM_SCGC6_TPM0_MASK);
  // SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  // SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

  TPM2->MOD = TIMER_THRESHOLD;
  TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM2->SC &= ~(TPM_SC_CPWMS_MASK);

  TPM0->MOD = TIMER_THRESHOLD;
  TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM0->SC &= ~(TPM_SC_CPWMS_MASK);

  TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C0SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_MSB(1));
  TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C1SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_MSB(1));
  TPM2_C0V = 0;
  TPM2_C1V = 0;

  TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM0_C0SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_MSB(1));
  TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM0_C1SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_MSB(1));
  TPM0_C0V = 0;
  TPM0_C1V = 0;
}

void change_frequency(int frequency)
{
  int mod_value = 375000 / frequency - 1;
  TPM1->MOD = mod_value;
  TPM1_C0V = mod_value / 16;
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
	for (;;) {
		if (isMoving)
		{
			for (int i = 0; i < 10; i++) {
				if (0 <= i && i <= 3) {
					PTB->PCOR |= MASK(greenPins[i]);
				} else {
					PTE->PCOR |= MASK(greenPins[i]);
				}
			}
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

  for (;;)
  {
    change_frequency(melody[i]);
    osDelay(noteDurations[i]);
    // change_frequency(0);
    // osDelay(5);
	int length = sizeof(melody) / sizeof(melody[0]);
	if (isDone == 0) {
		i = (i + 1) % length;
	} else {
		i = (i == 0) ? length - 1 : i - 1;
	}
  }
}

void brain_main(void *argument)
{
  for (;;)
  {
    osSemaphoreAcquire(brainSem, osWaitForever);
    if (uartData == 0b00000000)
    {
      leftDc = 0;
      rightDc = 0;
    }
    else if (uartData == 0b00000001)
    {
      leftDc = -0.5;
      rightDc = -0.5;
    }
    else
    {
      leftDc = ((uartData >> 4)) / 15.0;
      rightDc = ((uartData & 0b00001111)) / 15.0;
    }
		if (leftDc == 0 && rightDc == 0)
		{
			isMoving = 0;
		}
		else
		{
			isMoving = 1;
		}
    osSemaphoreRelease(motorSem);
  }
}

void motor_main(void *argument)
{
  for (;;)
  {
    osSemaphoreAcquire(motorSem, osWaitForever);
    if (leftDc > 0)
    {
      TPM2_C0V = TIMER_THRESHOLD * leftDc;
      TPM2_C1V = 0;
    }
    else
    {
      TPM2_C0V = 0;
      TPM2_C1V = TIMER_THRESHOLD * -leftDc;
    }
    if (rightDc > 0)
    {
      TPM0_C0V = TIMER_THRESHOLD * rightDc;
      TPM0_C1V = 0;
    }
    else
    {
      TPM0_C0V = 0;
      TPM0_C1V = TIMER_THRESHOLD * -rightDc;
    }
  }
}

int main(void)
{
  SystemCoreClockUpdate();
  initUART2();
  initLedGpio();
  initBuzzerPWM();
  initMotorPWM();
  brainSem = osSemaphoreNew(1, 0, NULL);
  motorSem = osSemaphoreNew(1, 0, NULL);
	

  osKernelInitialize(); // Initialize CMSIS-RTOS
  osThreadNew(red_blinky_main, NULL, NULL);
  osThreadNew(green_blinky_main, NULL, NULL);
  osThreadNew(buzz_main, NULL, NULL);
  osThreadNew(brain_main, NULL, NULL);
  osThreadNew(motor_main, NULL, NULL);
  osKernelStart(); // Start thread execution
  for (;;)
  {
  }
}
