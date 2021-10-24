/*
		Arjun Krishna (a68krish@uwaterloo.ca)
		Andrei Ikic (a2ikic@uwaterloo.ca)
*/

#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



int main (void) // LED program (program 1)
{    
    LPC_GPIO1->FIODIR |= 1u << 28;  // configure P1.28, the leftmost led as output

    LPC_GPIO1->FIOCLR |= 1u << 28;	// turn leftmost led off
    
    while(1)
    {
        while(LPC_GPIO2->FIOPIN & (1u << 10)); // while 10th pin in register 2 (P2.10, the button) is 1 (button is not pressed)
        
        // turn the leftmost LED on
        LPC_GPIO1->FIOSET |= 1u << 28; // set P1.28 to high to turn the led on that pin on
        
        while(!(LPC_GPIO2->FIOPIN & (1u << 10)));
       
        LPC_GPIO1->FIOCLR |= 1u << 28;
    }
} 

/*
int main(void) // joystick program (program 2)
{
	
	while(1)
	{
		if(!(LPC_GPIO1->FIOPIN & (1u << 20)))
		{
			printf("PRESSED     ");
		}
		else
		{
			printf("NOT PRESSED ");
		}
		
		if(!(LPC_GPIO1->FIOPIN & (1u << 23)))
		{
			printf("NORTH\n");
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 24)))
		{
			printf("EAST\n");
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 25)))
		{
			printf("SOUTH\n");
		}
		
		else if(!(LPC_GPIO1->FIOPIN & (1u << 26)))
		{
			printf("WEST\n");
		}
		
		else
		{
			printf("CENTER\n");
		}
	}
} */


/*
int main (void) // number to LED program (program 3)
{
	LPC_GPIO1->FIODIR |= 1u << 28; // set LEDS as output
	LPC_GPIO1->FIODIR |= 1u << 29;
	LPC_GPIO1->FIODIR |= 1u << 31;
	LPC_GPIO2->FIODIR |= 1u << 2; 
	LPC_GPIO2->FIODIR |= 1u << 3;
	LPC_GPIO2->FIODIR |= 1u << 4;
	LPC_GPIO2->FIODIR |= 1u << 5;
	LPC_GPIO2->FIODIR |= 1u << 6; 
	
	char string[3];
	uint8_t number = 0;
	
	while(1)
	{
		printf("Please input your number and terminate with ctrl+j\n");
	
		scanf("%s", string);
		number = atoi(string); // atoi converts string to an integer
		
		if (number & (1 << 7))
			LPC_GPIO1->FIOSET |= 1u << 28;
		else
			LPC_GPIO1->FIOCLR |= 1u << 28;
		
		if (number & (1 << 6))
			LPC_GPIO1->FIOSET |= 1u << 29;
		else
			LPC_GPIO1->FIOCLR |= 1u << 29;
		
		if (number & (1 << 5))
			LPC_GPIO1->FIOSET |= 1u << 31;
		else
			LPC_GPIO1->FIOCLR |= 1u << 31;
		
		if (number & (1 << 4))
			LPC_GPIO2->FIOSET |= 1u << 2;
		else
			LPC_GPIO2->FIOCLR |= 1u << 2;
		
		if (number & (1 << 3))
			LPC_GPIO2->FIOSET |= 1u << 3;
		else
			LPC_GPIO2->FIOCLR |= 1u << 3;
		
		if (number & (1 << 2))
			LPC_GPIO2->FIOSET |= 1u << 4;
		else
			LPC_GPIO2->FIOCLR |= 1u << 4;
		
		if (number & (1 << 1))
			LPC_GPIO2->FIOSET |= 1u << 5;
		else
			LPC_GPIO2->FIOCLR |= 1u << 5;
		
		if (number & (1 << 0))
			LPC_GPIO2->FIOSET |= 1u << 6;
		else
			LPC_GPIO2->FIOCLR |= 1u << 6;		
	}
} */


/*
int main (void)  // ADC program (program 4)
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

	while(1)
	{
		if(LPC_ADC->ADGDR & (1u << 31)) 
		{
			// obtain bits 4 to 15 in ADGDR and convert
			float a = ((LPC_ADC->ADGDR & 0xfff0) >> 4) * (3.3 / (pow(2, 12) - 1));
			printf("%.1f\n", a);
			
			LPC_ADC->ADCR |= 1 << 24; // set START bits to initiate conversion
		}
	}
} 
*/
