/*
		Arjun Krishna (a68krish@uwaterloo.ca)
		Andrei Ikic (a2ikic@uwaterloo.ca)
*/

#include <cmsis_os2.h> // include RTOS functions
#include <LPC17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random.h>

#define N 2

// TODO make sure osDelay wait is correct
int averageArrivalRate = 9;
int averageServiceRate = 10;
int workOrder = 7;

__NO_RETURN void client(osMessageQueueId_t q_id)
{
	printf("client %d\n", osMessageQueueGetCapacity(q_id));
	while(1)
	{	
		osDelay(((next_event() * osKernelGetTickFreq()) /  averageArrivalRate) >> 16);
		osMessageQueuePut(q_id, &workOrder, 0, 0);
		osThreadYield();
	}
}

__NO_RETURN void server(osMessageQueueId_t q_id)
{
	printf("server %d\n", osMessageQueueGetCapacity(q_id));
	while(1)
	{
		osDelay(((next_event() * osKernelGetTickFreq()) /  averageServiceRate) >> 16);
		int msg;
		osMessageQueueGet(q_id, &msg, 0, 0);
		printf("message %d\n", msg);
		osThreadYield();
	}
}

__NO_RETURN void monitor(osMessageQueueId_t q_id)
{
	printf("entered monitor\n");
	while(1)
	{
		printf("monitor\n");
		osThreadYield();
	}
}

int main (void)
{
	SystemCoreClockUpdate(); 
	osKernelInitialize();

	// create a new message queue
	osMessageQueueId_t messageQueues [N];
	
	// create message queues
	for(int i = 0; i < N; i++)
		messageQueues[i] = osMessageQueueNew(10, sizeof(int), NULL);
	
	// create client and serverthreads
	for(int i = 0; i < N; i ++)
	{
		osThreadNew(client, messageQueues[i], NULL);
		osThreadNew(server, messageQueues[i], NULL);
	}
	
	//osThreadNew(monitor, NULL, NULL);
	printf("starting kernels\n");
	osKernelStart();
	return 0;
}

/*
// create a new message queue with space for 10 messages
	// each of which is an integer
	// 3rd param, pointer to a struct defining message queue attrs
	// q_id: message queue identifier
	// use q_id in future get and put operations
	osMessageQueueId_t q_id = osMessageQueueNew(10, sizeof(int), NULL);
	int msg = 7;
	// use the q_id queue, store msg as a message
	// 3rd param is the priority
	// 4th param is timout
	// returns osErrorResource if not enough space in queue
	// put: share message in queue
	osMessageQueuePut(q_id, &msg, 0, 0);
	
	msg = 0;
	// get: recieve message from queue
	// receive a message from a queue with blocking indefinitely
	// 2nd param : points to a buffer (&msg)
	// buffer: temp place to store data
	// 	the kernel copies the message to
	// 3rd paramater: records message priority, NULL discards it
	// 4th: timeout, osWaitForever, blocks the calling thread
	// until message is retrieved
	osMessageQueueGet(q_id, &msg, NULL, osWaitForever);
	
	// create a total of 2*N + 1 concurrent threads
	// N clients, N servers (one for each queue)
	// one monitor thread, displays system status on the serial port
	
	// each client thread starts by determining which
	// queue it is sending messages to
	// waits a random period of time
	// sends a messge to one queue
	// GOTO wait
	
	// server:

	// have one server function that is used to create N server threads
	// each with a parameter, queue id
	// don't have N separate server functions
	// one client function, create N client threads
	// parameter that specifies what queue to send messages to
	
	MONITOR THREAD
	runs once a second
	prints the following info for each of the N queues to UART
	1. elapsed time in seconds
	2. total number of messages sent
	3. total number of messages recieved
	4. total number of overflows
		osMessageQueuePut returns error
	5. current number of messages in the queue
		use osMessageQueueGetCount
	6. Average message loss ratio
		number of overflows divided by total messages sent
	7. average message arrival rate (total number of messages sent / elapsed time)
	8. average service rate (total number of recieved messages / sum of server random sleep time)

	TODO
	create array of size N, to store queue id's
	create N message queues, and store queue id's in an array
	create N client threads
	create N server threads
	DONE
	client:
		determine which queue you are sending messages to
		DONE
		wait a random period of time
		DONE
		send a message to the queue
		GOTO wait
		server:
			DONE
			// determining which queue it is watching
			// wait a random amount of time
			DONE
			// retrieve a message from queue
			// GOTO wait
			// wait: simulate service time
			DONE
			
		
*/
