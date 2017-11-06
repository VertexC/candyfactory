#include "bbuff.h"
#include <stdlib.h>
#include <semaphore.h>

static sem_t mutex;
static sem_t full;
static sem_t empty;
static int first;
static int last;
void * buffer[BUFFER_SIZE];

void bbuff_init(void){
	sem_init(&full,1,0);
	sem_init(&empty,1,BUFFER_SIZE);
	sem_init(&mutex,1,1);
	first = 0;
	last = 0;
	return;
}

void bbuff_blocking_insert(void* item){
	sem_wait(&empty);
	sem_wait(&mutex);
	//
	buffer[last] = item;
	last = (last + 1)%BUFFER_SIZE;
	sem_post(&mutex);
	sem_post(&full);
}

void* bbuff_blocking_extract(void){
	sem_wait(&full);
	sem_wait(&mutex);
	//
	void* item = buffer[first];
	first = (first + 1)%BUFFER_SIZE;
	sem_post(&mutex);
	sem_post(&empty);
	return item;
}

_Bool bbuff_is_empty(void){
	_Bool is_empty;
	sem_wait(&mutex);
	is_empty = (last == first);
	sem_post(&mutex);
	return is_empty;
}