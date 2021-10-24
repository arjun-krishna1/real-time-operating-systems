/*
		Arjun Krishna (a68krish@uwaterloo.ca)
		Andrei Ikic (a2ikic@uwaterloo.ca)
*/

#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void setupLEDS (void)
{
	// set these LED's as output
	LPC_GPIO1->FIODIR |= 0xB0000000; // bit 28,29, 31 on GPIO1
	LPC_GPIO2->FIODIR |= 0x0000007C; // bit 2,3,4,5,6 on GPIO2
} 

void setupADC (void)
{
	LPC_SC->PCONP |= 1u << 12; // enable ADC power/clock control bit
	LPC_ADC->ADCR |= 1 << 21; // PDN = 1 --> ADC is operational
	LPC_SC->PCLKSEL0 &= ~(00u << 24); // divide cclk by 4

	LPC_PINCON->PINSEL1 &= ~(1u << 19); // set pin function to AD0.2
	LPC_PINCON->PINSEL1 |= 1u << 18;
	
	LPC_ADC->ADCR &= ~(0xff); // clear bits 0 - 7 before setting bit 2
	LPC_ADC->ADCR |= 1u << 2; // select bit 2 so AD0.2 is sampled
	LPC_ADC->ADCR |= 0x03 << 8; // set clock division by (3+1)
	LPC_ADC->ADCR &= ~(0 << 16); // burst  bit is 0
	LPC_ADC->ADCR |= 1 << 24; // set START bits to initiate conversion
}

__NO_RETURN void printADC(void *arg)
{
	while(1)
	{
		if(LPC_ADC->ADGDR & (1u << 31)) 
		{
			// obtain bits 4 to 15 in ADGDR and convert
			float a = ((LPC_ADC->ADGDR & 0xfff0) >> 4) * (3.3 / (pow(2, 12) - 1));
			printf("%.1f\n", a);
			
			LPC_ADC->ADCR |= 1 << 24; // set START bits to initiate conversion
			osThreadYield();
		}
	}
}

__NO_RETURN void joystickLED (void *arg)
{
	while(1)
	{
		LPC_GPIO2->FIOCLR |= 0x0000007C; // clear 5 rightmost LEDs 
		
		if(!(LPC_GPIO1->FIOPIN & (1u << 20))) // check if joystick is pressed
		{
			LPC_GPIO2->FIOSET |= (1 << 2);
		}
		else
		{
			LPC_GPIO2->FIOCLR |= (1 << 2);
		}
		
		if(!(LPC_GPIO1->FIOPIN & (1u << 23))) // check north
		{
			LPC_GPIO2->FIOSET |= (1 << 6);
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 24))) // check east
		{
			LPC_GPIO2->FIOSET |= (1 << 5);
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 25))) // check south
		{
			LPC_GPIO2->FIOSET |= (1 << 6) | (1 << 5);
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 26))) // check west
		{
			LPC_GPIO2->FIOSET |= (1 << 4);
		}
		
		osThreadYield();
	}	
}


__NO_RETURN void pushButton (void *arg)
{ // active low button
	while (1)
	{
		if (!(LPC_GPIO2->FIOPIN & (1u << 10))) // wait for button to be pressed (low)
		{
			while (!(LPC_GPIO2->FIOPIN & (1u << 10)) ); // wait for button to be released after press
			
			if (LPC_GPIO1->FIOPIN & (1u << 28)) // switch state of LED
				LPC_GPIO1->FIOCLR |= (1u << 28);
			else
				LPC_GPIO1->FIOSET |= (1u << 28);
		}
		osThreadYield();
	}
}


int main (void)
{
	setupLEDS();
	setupADC();
	
	SystemCoreClockUpdate(); 
	osKernelInitialize();
	osThreadNew(printADC, NULL, NULL);
	osThreadNew(joystickLED, NULL, NULL);
	osThreadNew(pushButton, NULL, NULL);
	osKernelStart();
	
}


