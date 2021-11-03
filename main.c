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
#define MONITOR_PERIOD 0.01

// TODO make sure osDelay wait is correct
int averageArrivalRate = 9;
int averageServiceRate = 10;
int workOrder = 7;
int balance[N];
// create a new message queue
osMessageQueueId_t messageQueues [N];
int messageIndices[N];

// arrays to hold statistics for each message queue
int messagesSent[N];
int messagesRecieved[N];
int messagesOverflow[N];
float serverRandomSleepTime[N];

__NO_RETURN void client(void *q_id_void)
{
	int q_id_int = *(int *) q_id_void;
	osMessageQueueId_t q_id = messageQueues[q_id_int];
	printf("client %d\n", osMessageQueueGetCapacity(q_id));
	while(1)
	{	
		osDelay(((next_event() * osKernelGetTickFreq()) /  averageArrivalRate) >> 16);
		osStatus_t status = osMessageQueuePut(q_id, &workOrder, 0, 0);
		if(status == osOK)
			messagesSent[q_id_int] += 1;
		else if(status == osErrorResource)
			messagesOverflow[q_id_int] += 1;
		
		osThreadYield();
	}
}

__NO_RETURN void server(void *q_id_void)
{
	int q_id_int = *(int *) q_id_void;
	osMessageQueueId_t q_id = messageQueues[q_id_int];
	printf("server %d\n", osMessageQueueGetCapacity(q_id));
	while(1)
	{
		
		int randomSleepTime = ((next_event() * osKernelGetTickFreq()) /  averageServiceRate) >> 16;
		osDelay(randomSleepTime);
		serverRandomSleepTime[q_id_int] += ((float)randomSleepTime) / ((float)osKernelGetTickFreq());
		int msg;
		osStatus_t status = osMessageQueueGet(q_id, &msg, 0, osWaitForever);
		if(status == osOK)
			messagesRecieved[q_id_int] += 1;
		osThreadYield();
	}
}

__NO_RETURN void monitor(void *arg)
{
	printf("monitor\n");
	while(1)
	{
		int n_iter = 0;
		if(n_iter % 20) printf("Qid, Time, Sent, Recv, Over, Wait,   P_blk,    Arrv,    Serv\n");
		n_iter = (n_iter + 1)%20;
		for(int i = 0; i < N; i++)
		{
			printf(" Q%d,", i);
			int elapsedTime = osKernelGetTickCount() / osKernelGetTickFreq(); 
			printf("%5d,", elapsedTime);
			printf("%5d,", messagesSent[i]);
			printf("%5d,", messagesRecieved[i]);
			printf("%5d,", messagesOverflow[i]);
			printf("%5d,", osMessageQueueGetCount(messageQueues[i]));
			printf("%8.4f", ((float)messagesOverflow[i])/((float)messagesSent[i]));
			printf("%8.4f", ((float)messagesSent[i]) / ((float)elapsedTime));	
			printf("%8.4f", ((float)messagesRecieved[i] )/ serverRandomSleepTime[i]);	
			
			printf("\n");
		}
		printf("\n");
		// wait for one second
		osDelay(osKernelGetTickFreq()*(MONITOR_PERIOD));
	}
}


int main (void)
{
	SystemCoreClockUpdate(); 
	osKernelInitialize();
	
	// create message queues
	for(int i = 0; i < N; i++)
		messageQueues[i] = osMessageQueueNew(10, sizeof(int), NULL);
	
	// create client and serverthreads
	for(int i = 0; i < N; i ++)
	{
		messageIndices[i] = i;
		osThreadNew(client, (void*)&messageIndices[i], NULL);
		osThreadNew(server, (void*)&messageIndices[i], NULL);
	}
	
	osThreadNew(monitor, NULL, NULL);
	printf("starting kernels\n");
	osKernelStart();
	return 0;
}
