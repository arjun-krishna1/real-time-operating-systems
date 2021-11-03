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
#define MONITOR_PERIOD 1
#define AVERAGE_ARRIVAL_RATE 9
#define AVERAGE_SERVICE_RATE 10
#define QUEUE_SIZE 10

// TODO make sure osDelay wait is correct
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
		osDelay(((next_event() * osKernelGetTickFreq()) /  AVERAGE_ARRIVAL_RATE) >> 16);
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
		
		int randomSleepTime = ((next_event() * osKernelGetTickFreq()) /  AVERAGE_SERVICE_RATE) >> 16;
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
	
	float loadFactor = ((float)AVERAGE_ARRIVAL_RATE) / ((float)AVERAGE_SERVICE_RATE);
	float expectedBlockProbability = ((float)((pow(loadFactor, QUEUE_SIZE))*(1 - loadFactor)))/((float)(1 - pow(loadFactor, QUEUE_SIZE)));
	printf("expectedBlockProbability %8.5f", expectedBlockProbability);
	
	while(1)
	{
		int elapsedTime = osKernelGetTickCount() / osKernelGetTickFreq(); 
		
		if(!(elapsedTime % 20))
		{
			printf("Qid, Time, Sent, Recv, Over, Wait,   P_blk,");
			printf("   Arrv,  Serv,  Epblk, Earrv, Eserv\n");
		}
		for(int i = 0; i < N; i++)
		{
			printf(" Q%d,", i);
			printf("%5d,", elapsedTime);
			printf("%5d,", messagesSent[i]);
			printf("%5d,", messagesRecieved[i]);
			printf("%5d,", messagesOverflow[i]);
			printf("%5d,", osMessageQueueGetCount(messageQueues[i]));
			
			float blockProbability = ((float)messagesOverflow[i])/((float)messagesSent[i]);
			printf("%8.4f", blockProbability);
			float arrivalRate = ((float)messagesSent[i]) / ((float)elapsedTime);
			printf("%8.4f", arrivalRate);
			float serviceRate = ((float)messagesRecieved[i] )/ serverRandomSleepTime[i];
			printf("%8.4f", serviceRate);

			
			printf("%8.4f", ((float)(blockProbability - expectedBlockProbability) )/ ((float) expectedBlockProbability));	
			printf("%8.4f", ((float)(arrivalRate - AVERAGE_ARRIVAL_RATE))/ ((float) AVERAGE_ARRIVAL_RATE));	
			printf("%8.4f", ((float)(serviceRate - AVERAGE_SERVICE_RATE)/ ((float) AVERAGE_SERVICE_RATE)));	
			
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
		messageQueues[i] = osMessageQueueNew(QUEUE_SIZE, sizeof(int), NULL);
	
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
