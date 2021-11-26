
/**
 * @brief Your source code goes here
 * @note You need to complete this file. New functions may be added to this file.
 */

#include <cmsis_os2.h>
#include "general.h"

// add any #includes here
#include "string.h"

// add any #defines here
#define n nGeneral
#define maxGenerals 7

int factorial(int8_t n)
{
	if (n == 1 || n == 0)
		return 1;
	else 
		return n*factorial(n-1);
}

// add global variables here
osMessageQueueId_t messageQueues[maxGenerals];
osSemaphoreId_t semaphore;
osSemaphoreId_t turnstile;
osMutexId_t mutex;
bool loyalty[maxGenerals];
uint8_t numberGenerals;
uint8_t commanderId;
uint8_t m; // number of traitors
uint8_t reporterId;
uint8_t generalSyncCount;
uint8_t turnstileCount;

/** Record parameters and set up any OS and other resources
  * needed by your general() and broadcast() functions.
  * nGeneral: number of generals
  * loyal: array representing loyalty of corresponding generals
  * reporter: general that will generate output
  * return true if setup successful and n > 3*m, false otherwise
  */
bool setup(uint8_t nGeneral, bool loyal[], uint8_t reporter) {
	
		m = 0; 
		reporterId = reporter;
	  numberGenerals = nGeneral;
		generalSyncCount = 0;
		commanderId = 0;
		turnstileCount = 0;
	
		for (uint8_t i = 0; i < nGeneral; i++)
		{
			loyalty[i] = loyal[i];
			if (loyal[i] == false)
				m++;
		}
					
		if (!c_assert(n > 3*m))
			return false;
	
		uint8_t queueSize = 1;
		for (uint8_t i = 1; i <= m; i++)
			queueSize += (factorial(n) / (n*(n-1)*factorial(n - (i+2))));
	
		for (uint8_t i = 0; i < nGeneral; i++) // NEED TO CHECK RETURN VALUES 
																						 // FOR RESOURCES
			messageQueues[i] = osMessageQueueNew(queueSize, (sizeof(char)*(m+2)), NULL);

		semaphore = osSemaphoreNew(1, 0, NULL);
		turnstile = osSemaphoreNew(1, 0, NULL);
		mutex = osMutexNew(NULL);

    return true;
}


/** Delete any OS resources created by setup() and free any memory
  * dynamically allocated by setup().
  */
void cleanup(void) {
	
	osMutexDelete(mutex);
	osSemaphoreDelete(semaphore);
	osSemaphoreDelete(turnstile);
	for (int i = 0; i < numberGenerals; i++)
		if (i != commanderId) // because commander's message queue already gets deleted in broadcast
			osMessageQueueDelete(messageQueues[i]);
}


/** This function performs the initial broadcast to n-1 generals.
  * It should wait for the generals to finish before returning.
  * Note that the general sending the command does not participate
  * in the OM algorithm.
  * command: either 'A' or 'R'
  * sender: general sending the command to other n-1 generals
  */
void broadcast(char command, uint8_t commander) {
	
	char commandEven = command;
	char commandOdd = command;
	commanderId = commander;
	
	if (loyalty[commander] == false) // commander not loyal
	{
		commandEven = 'R';
		commandOdd = 'A';
	}
	
	char commanderCharEven[2] = {commandEven, (commander + '0')}; 
	char commanderCharOdd[2] = {commandOdd, (commander + '0')}; 
	
	osMessageQueueDelete(messageQueues[commander]);
	
	for (uint8_t i = 0; i < maxGenerals; i++)
			if (i != commander) 
			{
				if (i % 2 == 0)
					osMessageQueuePut(messageQueues[i], &commanderCharEven, 0, osWaitForever);
				else
					osMessageQueuePut(messageQueues[i], &commanderCharOdd, 0, osWaitForever);
			}
			
	osSemaphoreAcquire(semaphore, osWaitForever);
	return;
	
}


/** Generals are created before each test and deleted after each
  * test.  The function should wait for a value from broadcast()
  * and then use the OM algorithm to solve the Byzantine General's
  * Problem.  The general designated as reporter in setup()
  * should output the messages received in OM(0).
  * idPtr: pointer to general's id number which is in [0,n-1]
  */
void general(void *idPtr) {
	
    uint8_t id = *(uint8_t *)idPtr;
	
		while (!(osMessageQueueGetCount(messageQueues[id])));
		// proceed after receiving message from commander
	
		char charBuffer[4];
		osMessageQueueGet(messageQueues[id], charBuffer, 0, osWaitForever);

		if (loyalty[id] == false)
		{
			if ((id % 2) == 0) // even
				charBuffer[0] = 'R';
			else
				charBuffer[0] = 'A';
		}
		
		charBuffer[2] = id + '0';
	
		for (uint8_t i = 0; i < numberGenerals; i++)
			if (i != id && i != commanderId)
			{
				osMessageQueuePut(messageQueues[i], charBuffer, 0, osWaitForever);
				
				if (m == 1 && i == reporterId)
				{
					for (uint8_t j = strlen(charBuffer); j > 0; j--)
						if (j == 1)
							printf("%c\n", charBuffer[j-1]);
						else
							printf("%c:", charBuffer[j-1]);
				}
					
			}
			
		osMutexAcquire(mutex, osWaitForever);
		turnstileCount++;
		osMutexRelease(mutex);
			
		if (turnstileCount == numberGenerals - 1)
			osSemaphoreRelease(turnstile);
			
		osSemaphoreAcquire(turnstile, osWaitForever);	
		osSemaphoreRelease(turnstile);
		// might need to add a stop or turnstile here, something to synchronize
		// ^^^  ya i think adding sync. was a good idea
		
		if (m > 1)
		{
			for (uint8_t i = 0; i < (numberGenerals - 2); i++)
				{
					osMessageQueueGet(messageQueues[id], charBuffer, 0, osWaitForever);
					charBuffer[3] = id + '0';
					
					if (loyalty[id] == false)
						{
							if ((id % 2) == 0) // even
								charBuffer[0] = 'R';
							else
								charBuffer[0] = 'A';
						}
					

					for (uint8_t j = 0; j < numberGenerals; j++)
						if (j != id && j != commanderId && j != (charBuffer[2] - '0') )
						{
							osMessageQueuePut(messageQueues[j], charBuffer, 0, osWaitForever);
						
							if (j == reporterId)
							{
								for (uint8_t j = strlen(charBuffer); j > 0; j--)
									if (j == 1)
										printf("%c\n", charBuffer[j-1]);
									else
										printf("%c:", charBuffer[j-1]);
							}		
						}
				}
		}
		
		osMutexAcquire(mutex, osWaitForever);
		generalSyncCount++;
		osMutexRelease(mutex);
		
		if (generalSyncCount == numberGenerals - 1)
			osSemaphoreRelease(semaphore);
		
}
