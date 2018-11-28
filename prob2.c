//Christopher Shorter
//OS_HW8
//Problem 1
#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define BUFLEN 1024

// Struct for condition variable
struct condition {
	sem_t sem; // Semaphore to be initialized to 0
	int count; // Count of threads waiting
};

struct condition writer_done;
struct condition reader_done;
int next_count;

sem_t next;


char buffer[BUFLEN];
int version = 0;

int read_count = 0;

void* reader1();
void* reader2();

void cwait(struct condition *c); // Semaphore implementation of conditional wait
void cpost(struct condition *c); // Semaphore implementation of conditional signal

int main(){
	sem_init(&next, 0, 0);

	bzero(buffer, BUFLEN);

	pthread_t rd1_tid;
	pthread_t rd2_tid;

	pthread_attr_t rd1_attr;
	pthread_attr_t rd2_attr;

	pthread_attr_init(&rd1_attr);
	pthread_attr_init(&rd2_attr);

	pthread_create(&rd1_tid, &rd1_attr, reader1, NULL);
	pthread_create(&rd2_tid, &rd2_attr, reader2, NULL);

	do{
		sleep(1);
		

		bzero(buffer, BUFLEN);
		sprintf(buffer, "version %d: \"string\"", version+1);
		version++;

		cpost(&writer_done);

	}while(version<2);
	cwait(&reader_done);

	return 0;
}

void* reader1(){
	bool done = false;
	while(!done){

		cwait(&writer_done);
		read_count++;
		
		if(version == 1){
			printf("Reader #1: %s\n", buffer);
			fflush(stdout);
			done = true;
		}

		read_count--;
		if(read_count == 0)
			cpost(&reader_done);
		
	}
	return NULL;
}

void* reader2(){
	bool done = false;
	while(!done){
		fflush(stdout);
		fflush(stdout);
		read_count++;
		
		if(version == 2){
			printf("Reader #2: %s\n", buffer);
			fflush(stdout);
			done = true;
		}

		read_count--;
		if(read_count == 0)
			cpost(&reader_done);
		
	}
	return NULL;
}

// Semaphore implementation of conditional wait
void cwait(struct condition *c) {
	c->count++;
	sem_wait(&(c->sem));
	c->count--;
}

// Semaphore implementation of conditional signal
void cpost(struct condition *c) {
	if (c->count > 0) {
		sem_post(&(c->sem));
	}
}
