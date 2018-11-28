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

// Struct for condition variable
struct condition {
	sem_t sem; // Semaphore to be initialized to 0
	int count; // Count of threads waiting
};

int order[2*N];
int global_index;

sem_t mutex;
sem_t next;
int next_count = 0;
int item_count = 0;

struct condition not_full;
struct condition not_empty;

int buffer[N/2];

void *child();

void cwait(struct condition *c); // Semaphore implementation of conditional wait
void cpost(struct condition *c); // Semaphore implementation of conditional signal

int main(){
	sem_init(&mutex, 0, 1);
	sem_init(&next, 0, 0);

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

		sem_wait(&mutex);
		if(item_count >= N/2)
			cwait(&not_full);

		/*add next_produced to buffer*/
		buffer[buffer_index] = next_produced;
		order[global_index] = 1;
		global_index++;
		item_count++;
		
		cpost(&not_empty);
		if(next_count>0)
			sem_post(&next);
		else
			sem_post(&mutex);
		
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
		sem_wait(&mutex);
		if(item_count <= 0)
			cwait(&not_empty);

		int next_consumed = buffer[buffer_index];
		buffer[buffer_index] = 0;
		
		order[global_index] = 2;
		global_index++;
		item_count--;

		cpost(&not_full);
		if(next_count>0)
			sem_post(&next);
		else
			sem_post(&mutex);
		
		printf("Child Iteration #%d. data = %d\n", i, next_consumed);
		fflush(stdout);
		
		buffer_index++;
		if(buffer_index == N/2)
			buffer_index = 0;
	}
	
	return NULL;
}

// Semaphore implementation of conditional wait
void cwait(struct condition *c) {
	c->count++;
	if (next_count > 0)
		sem_post(&next);
	else
		sem_post(&mutex);
	sem_wait(&(c->sem));
	c->count--;
}

// Semaphore implementation of conditional signal
void cpost(struct condition *c) {
	if (c->count > 0) {
		next_count++;
		sem_post(&(c->sem));
		sem_wait(&next);
		next_count--;
	}
}
