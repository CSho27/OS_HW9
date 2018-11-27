//Christopher Shorter
//OS_HW7
//Problem 1

#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N 25

int order[2*N];
int global_index;
sem_t mutex;
sem_t empty;
sem_t full;
int buffer[N/2];

void *child();

int main(){
	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, N/2);
	sem_init(&full, 0, 0);

	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, child, NULL);

	int i=0;
	int buffer_index = 0;
	for(; i<N; i++){
		/*produce an item in next_produced */
		int next_produced = rand() % 10;
		printf("Parent Iteration #%d. data = %d\n", i, next_produced);
		fflush(stdout);

		sem_wait(&empty);
		sem_wait(&mutex);

		/*add next_produced to buffer*/
		buffer[buffer_index] = next_produced;
		order[global_index] = 1;
		global_index++;

		sem_post(&mutex); 
		sem_post(&full);
		
		buffer_index++;
		if(buffer_index == N/2){
			buffer_index = 0;
		}
		
	}
	
	sleep(2);
	i = 0;
	for(; i<N*2; i++){
		printf("%d\n", order[i]);
		fflush(stdout);
	}
	
	return 0;
}

void* child(){
	sleep(1);

	int i = 0;
	int buffer_index = 0;
	for(; i<N; i++){
		sem_wait(&full);
		sem_wait(&mutex);

		int next_consumed = buffer[buffer_index];
		buffer[buffer_index] = 0;
		
		order[global_index] = 2;
		global_index++;

		sem_post(&mutex);
		sem_post(&empty);
		
		printf("Child Iteration #%d. data = %d\n", i, next_consumed);
		fflush(stdout);
		
		buffer_index++;
		if(buffer_index == N/2)
			buffer_index = 0;
	}
	
	return NULL;
}