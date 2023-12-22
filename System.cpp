// C program to demonstrate working of Semaphores 
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <signal.h>
#include <queue>
#include <time.h>
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KNRM  "\x1B[0m"

#define q1_SIZE 10
#define q2_SIZE 20
#define q3_SIZE 30

#define q1_QUANNTUM 8
#define q2_QUANNTUM 16

struct Process {    //struct of parameters for the threads.
    int pid;
    int burst_time;
    
};


std::queue<Process> q1,q2,q3;
sem_t mutex,full,empty; 


void print_queue(std::queue<Process> q,int queue_size)
{
   std::queue<Process>duplicate=q;
   int c=0;
   while(!duplicate.empty())
   {
   	printf("%sP%d\t",KNRM,duplicate.front().pid);
   	duplicate.pop();
   	c++;
   }
   while(c<queue_size)
   {
   	printf("%s%d\t",KNRM,0);
   	c++;
   }
   printf("\n");
}






void* longTerm_sch(void *args) 
{

	int lower_bound = 10; // Lower bound of the range
	int upper_bound = 20; // Upper bound of the range
	int sleep_time;
	
	int plower_bound = 1; // Lower bound of the range
	int pupper_bound = 100; // Upper bound of the range
	for(int i=1;i<=100;i++)
	{
		int empty_value;
		sem_getvalue(&empty,&empty_value);
		if(empty_value==0)
		{
			printf("%sLong Term Scheduler: q1 full!!",KYEL);
		}
		//wait 
		sem_wait(&empty);
		sem_wait(&mutex);
		
		printf("\n%sLong Term Scheduler: adding a process in q1.\n",KYEL); 
		//critical section 
		Process p;
		p.pid=i;
		srand(time(NULL));
		p.burst_time = (rand() % (pupper_bound - plower_bound + 1)) + plower_bound;
		
		q1.push(p);

		 
		sem_post(&mutex); 
		//signal 
		sem_post(&full); 
		//sleep after 
		srand(time(NULL));
		sleep_time = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
		sleep(sleep_time);
	}
	
	return NULL;
} 

void* ShortTerm_sch(void *args) 
{
	int lower_bound = 30; // Lower bound of the range
	int upper_bound = 40; // Upper bound of the range
	int full_value,rand_num;
	Process p;

	while (1)
        {
        	while(!q1.empty() || !q2.empty() || !q3.empty())
        	{
        		srand(time(NULL));
			rand_num = (rand() % (100 - 1 + 1)) + 1;
			
			if(rand_num >= 1 && rand_num <= 50 && !q1.empty())
			{	
				sem_getvalue(&full,&full_value);
				if(full_value==0)
				{
					printf("%sCollector thread: nothing is in the buffer!.\n",KGRN); 
				}

				//wait 
				sem_wait(&full);
				sem_wait(&mutex);
				
				p=q1.front();
				q1.pop();
				printf("%sCollector thread: reading from the buffer at position %d.\n",KGRN,0); 
				
				//printf("%s In consumer:\narray= ", KNRM);
				
				//print_queue(buffer); 

				sem_post(&mutex); 

				//signal
				sem_post(&empty); 
			}
			else if(rand_num >= 51 && rand_num <= 80 && !q2.empty())
			{	
				
				sem_wait(&full);
				sem_wait(&mutex);
				
				p=q1.front();
				q1.pop();
				printf("%sCollector thread: reading from the buffer at position %d.\n",KGRN,0); 
				
				//printf("%s In consumer:\narray= ", KNRM);
				
				//print_queue(buffer); 

				sem_post(&mutex); 
				//signal
				sem_post(&empty);
			}
			else if(rand_num >= 81 && rand_num <= 100 && !q3.empty())
			{
			}
		}
     	}
		
	
	    return NULL;
	
} 

void intHandler(int dummy) {
	// set the noramal color back
    	printf("%sExit\n", KNRM);
	// Destroy the semaphore 
	sem_destroy(&mutex);
	sem_destroy(&full); 
	sem_destroy(&empty); 
	exit(0);
}

int main() 
{ 
	signal(SIGINT, intHandler);
	
	// counter sem set to 1 mutex 
	sem_init(&mutex, 0, 1); 
	sem_init(&full, 0, 0); 
	sem_init(&empty, 0, q1_SIZE); 
	pthread_t t1,t2;
	pthread_create(&t1,NULL,longTerm_sch,NULL); 
	sleep(2); 
	pthread_create(&t2,NULL,ShortTerm_sch,NULL);
	
	
	 
	pthread_join(t1,NULL); 
	pthread_join(t2,NULL);
	
	sem_destroy(&mutex); 
	sem_destroy(&full); 
	sem_destroy(&empty); 
	printf("%sExit\n", KNRM);
	return 0; 
} 


