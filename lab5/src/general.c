
/**
 * @brief Your source code goes here
 * @note You need to complete this file. New functions may be added to this file.
 */

#include <cmsis_os2.h>
#include "general.h"

// add any #includes here

// add any #defines here
#define maxGenerals 7


int factorial(int8_t n)
{
	if (n == 1 || n == 0)
		return 1;
	else 
		return n*factorial(n-1);
}

// add global variables here
uint8_t n;
osMessageQueueId_t messageQueues[maxGenerals];
bool done;
osSemaphoreId_t semaphore;

/** Record parameters and set up any OS and other resources
  * needed by your general() and broadcast() functions.
  * nGeneral: number of generals
  * loyal: array representing loyalty of corresponding generals
  * reporter: general that will generate output
  * return true if setup successful and n > 3*m, false otherwise
  */
bool setup(uint8_t nGeneral, bool loyal[], uint8_t reporter) {
		semaphore = osSemaphoreNew(1, 0, NULL);
		done = true;
		n = nGeneral;
		uint8_t m = 0; // number of traitors
		for (uint8_t i = 0; i < nGeneral; i++)
			if (loyal[i] == false)
				m++;
	
		if (!c_assert(n > 3*m))
			return false;
	
		uint8_t queueSize = 1;
		for (uint8_t i = 1; i <= m; i++)
			queueSize += (factorial(n) / (n*(n-1)*factorial(n - (i+2))));
	
		for (uint8_t i = 0; i < nGeneral-1; i++) // NEED TO CHECK RETURN VALUES 
																						 // FOR RESOURCES
			messageQueues[i] = osMessageQueueNew(queueSize, sizeof(char)*maxGenerals, NULL);
		
    return true;
}


/** Delete any OS resources created by setup() and free any memory
  * dynamically allocated by setup().
  */
void cleanup(void) {
}


/** This function performs the initial broadcast to n-1 generals.
  * It should wait for the generals to finish before returning.
  * Note that the general sending the command does not participate
  * in the OM algorithm.
  * command: either 'A' or 'R'
  * sender: general sending the command to other n-1 generals
  */
void broadcast(char command, uint8_t commander) {
	/*
	send command to each general except for commander
	*/
	//don't need mutex because
	//only thread working
	for(int i = 0; i < n; i++)
	{
		if(i != commander)
		{
			// make sure command's address is ok to be used
			// by threads, they might need mutex to access it
			osMessageQueuePut(
				messageQueues[i], &command, 0, osWaitForever);
		}
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
		/*
		wait for a value from broadcast()
			wait until there is a message in the queue?
		use the OM algorithm to solve the BG problem
		general designated as reporter in setup() should output the messages
		recieved in OM(0)
		*/
    uint8_t id = *(uint8_t *)idPtr;
		while(1)
		{
			// wait until they recieve a message
		}
		
		// OM algorithm starts here
		
		// if reporter output all recieved values
		
}
