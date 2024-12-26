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

struct Process {   
    int pid;
    int burst_time;
    
};


std::queue<Process> q1,q2,q3;
sem_t mutex,full,empty,empty_mutex; 
int TIME=0;
bool end =false;
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

	int lower_bound = 1;
	int upper_bound = 10; 
	int sleep_time;
	
	int plower_bound = 1; 
	int pupper_bound = 100;
	int empty_value;
	for(int i=1;i<=3;i++)
	{

		sem_wait(&empty_mutex);
		sem_getvalue(&empty,&empty_value);
		if(empty_value==0)
		{
			printf("%sLong Term Scheduler: q1 full!!",KYEL);
		}
		sem_wait(&empty);
		sem_post(&empty_mutex);
		sem_wait(&mutex);
		Process p;
		p.pid=i;
		srand(time(NULL));
		p.burst_time = (rand() % (pupper_bound - plower_bound + 1)) + plower_bound;
		printf("\n%sLong Term Scheduler: adding a process in q1 with burst time=%d.\n",KYEL,p.burst_time); 
		q1.push(p);
		sem_post(&mutex); 
		sem_post(&full); 
		srand(time(NULL));
		sleep_time = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
		sleep(sleep_time);
	}
	return NULL;
} 

void* ShortTerm_sch(void *args) 
{
	int full_value,rand_num,rand_num2,empty_value;
	Process p;
	bool in_queue;
	while (1)
        {
        	while(!q1.empty() || !q2.empty() || !q3.empty())
        	{
        		srand(time(NULL));
			rand_num = (rand() % (10 - 1 + 1)) + 1;
 
			if(rand_num >= 1 && rand_num <= 5 && !q1.empty())
			{	
				sem_getvalue(&full,&full_value);
				if(full_value==0)
				{
					printf("%sShort Term Scheduler: nothing is in the q1!.\n",KGRN); 
				}

				sem_wait(&full);
				sem_wait(&mutex);
				
				p=q1.front();
				q1.pop();
				printf("%s\t%d\n", KNRM,TIME);
				printf("%sP%d was in q1\n", KNRM,p.pid);
				sem_post(&mutex);
				sem_wait(&empty_mutex); 
				sem_post(&empty); 
				sem_post(&empty_mutex);
				
				if (p.burst_time > 8) 
				{
					TIME=TIME+8;
					p.burst_time = p.burst_time-8;
					if((int)q2.size()<q2_SIZE)
					{
						q2.push(p);
					}
					else
					{
						printf("%sDEADLOCK!!",KRED);
						printf("%sShort Term Scheduler: q2 is full!. P%d will be terminated.\n",KRED,p.pid);
					}

				}
				else
				{
					TIME=TIME+p.burst_time;
				}
			}
			else if(rand_num >= 6 && rand_num <= 8 && !q2.empty())
			{
				
				p=q2.front();
				q2.pop();
				printf("%s\t%d\n", KNRM,TIME);
				printf("%sP%d was in q2\n", KNRM,p.pid);

				if (p.burst_time > 16) 
				{	
					TIME=TIME+16;
					p.burst_time = p.burst_time-16;
					in_queue=false;
					rand_num2 = rand() % 2; 
					if (rand_num2 == 0) 
					{
						
						sem_wait(&empty_mutex);
						sem_getvalue(&empty,&empty_value);
						if(empty_value>0)
						{
							sem_wait(&empty);
							sem_post(&empty_mutex);
							sem_wait(&mutex);
							q1.push(p);
							in_queue=true;
							sem_post(&mutex);  
							sem_post(&full);
						}
						sem_post(&empty_mutex);

						if(!in_queue && (int)q3.size()<q3_SIZE)
						{	
							printf("%sShort Term Scheduler: q1 is full!. P%d will inserted in q3.\n",KRED,p.pid);
							q3.push(p);
							in_queue=true;
						}
						 

						
					}
					else 
					{	
						if((int)q3.size()<q3_SIZE)
						{
							q3.push(p);
							in_queue=true;
						}
						else
						{	
							printf("%sShort Term Scheduler: q3 is full!. P%d will inserted in q1.\n",KRED,p.pid);
							
							sem_wait(&empty_mutex);
							sem_getvalue(&empty,&empty_value);
							if(empty_value>0)
							{
								sem_wait(&empty);
								sem_post(&empty_mutex);
								sem_wait(&mutex);
								q1.push(p);
								in_queue=true;
								sem_post(&mutex); 
								sem_post(&full);
							}
							sem_post(&empty_mutex);
							
						}
						
					
					}
					if(!in_queue)
					{
						printf("%sDEADLOCK!!",KRED);	
						printf("%sShort Term Scheduler: q1 & q3  are full!. P%d will be terminated.\n",KRED,p.pid);
					}
					
					
						
				}
				else
				{
					TIME=TIME+p.burst_time;	
				} 
					

			}
			else if(rand_num >= 9 && rand_num <= 10 && !q3.empty())
			{	
				p=q3.front();
				q3.pop();
				printf("%s\t%d\n", KNRM,TIME);
				printf("%sP%d was in q3\n", KNRM,p.pid);
				TIME=TIME+p.burst_time;

			}

			if(q1.empty() && q2.empty() && q3.empty() && end)
			{
				printf("%s\t%d\n", KNRM,TIME);
				printf("%sEND\n", KRED);
				return NULL;
			}

		}
     	}
		
	
	    return NULL;
	
} 

void intHandler(int dummy) {

    	printf("%sExit\n", KNRM);
	sem_destroy(&empty_mutex);
	sem_destroy(&mutex);
	sem_destroy(&full); 
	sem_destroy(&empty); 
	exit(0);
}

int main() 
{ 
	signal(SIGINT, intHandler);
	
	sem_init(&empty_mutex, 0, 1); 
	sem_init(&mutex, 0, 1); 
	sem_init(&full, 0, 0); 
	sem_init(&empty, 0, q1_SIZE); 
	pthread_t t1,t2;
	pthread_create(&t1,NULL,longTerm_sch,NULL); 
	sleep(2); 
	pthread_create(&t2,NULL,ShortTerm_sch,NULL);
	
	pthread_join(t1,NULL); 
	end = true;
	pthread_join(t2,NULL);
	
	sem_destroy(&empty_mutex);
	sem_destroy(&mutex); 
	sem_destroy(&full); 
	sem_destroy(&empty); 
	printf("%sExit\n", KNRM);
	return 0; 
} 


