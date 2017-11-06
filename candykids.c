//candykids.c
#include "bbuff.h"
#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct{
	int factory_number;//track which factory thread produced
	double time_stamp_in_ms;//tracks when the item was created
}candy_t;

enum{
	false = 0,
	true
};

_Bool stop_produce = false;

int current_time = 0;
pthread_t * factory_ID;
pthread_t * kid_ID;
int * variable;
double current_time_in_ms(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void * factory_thread(void * arg){
	int factory_number = *(int *)arg;
	while(!stop_produce){
		//produce the candy
		//insert the item
		//printf("!!!!\n");
		int sleep_time = rand()%4;
		printf("\tFactory %d ships candy & wait %ds\n", factory_number,sleep_time);
		candy_t * candy = (candy_t *)malloc(sizeof(candy_t));
		candy->factory_number = factory_number;
		candy->time_stamp_in_ms = current_time_in_ms();
		bbuff_blocking_insert(candy);
		stats_record_produced(factory_number);
		sleep(sleep_time);
		//printf("!!!!\n");
	}
	printf("Candy-factory %d done\n", factory_number);
	pthread_exit(0);
}

void * kid_thread(void * arg){
	while(true){
		//comsume the candy
		//extract the item
		candy_t * candy = (candy_t *)bbuff_blocking_extract();
		//printf("consume!\n");
		if(candy)
			stats_record_consumed(candy->factory_number, current_time_in_ms() - candy->time_stamp_in_ms);
		free(candy);
		sleep(rand()%2);
	}
	pthread_exit(0);
}

int main(int argc, char * argv[]){
	//1. extract arguments
	int facotries = 0;
	int kids = 0;
	int seconds = 0;
	if(argc != 4){
		printf("Error: Expect 3 arguments\n");
		//free?
		exit(0);
	}
	facotries = atoi(argv[1]);
	kids = atoi(argv[2]);
	seconds = atoi(argv[3]);
	if(!(facotries > 0 && kids > 0 && seconds > 0)){
		printf("Error: All arguments shoud be int and > 0\n");
		exit(0);
	}
	//printf("1 done\n");
	srand((unsigned)time(NULL));//initialize the seed
	//2. Initialize module
	bbuff_init();
	stats_init(facotries);
	factory_ID = (pthread_t *)malloc(sizeof(pthread_t) * facotries);
	kid_ID = (pthread_t *)malloc(sizeof(pthread_t) * kids);
	variable = (int *)malloc(sizeof(int) * facotries);
	for(int i = 0; i < facotries; i++){
		variable[i] = i;
	}
	//printf("2 done\n");
	//3. launch factory threads
	for(int i = 0; i < facotries; i++){
		//spawn the thread
		pthread_t daThreadId;
		pthread_attr_t attr_t;
		pthread_attr_init(&attr_t);
		pthread_create(&daThreadId, &attr_t, factory_thread, &variable[i]);
		factory_ID[i] = daThreadId;
		//wait
	}
	//printf("3 done\n");
	//4. launch kid threads
	for(int i = 0; i < kids; i++){
		pthread_t daThreadId;
		pthread_attr_t attr_t;
		pthread_attr_init(&attr_t);
		pthread_create(&daThreadId, &attr_t, kid_thread, NULL);
		kid_ID[i] = daThreadId;
	}
	//printf("4 done\n");
	//5. wait for requested time
	for(int i = 0; i < seconds; i++){
		printf("Time  %ds:\n", i);
		sleep(1);
	}

	//printf("5 done\n");
	//6. stop factory threads
	printf("Stopping candy facotries...\n");
	stop_produce = true;
	for(int i = 0; i < facotries; i++){
		//printf("i:%d\n",i);
		pthread_join(factory_ID[i], NULL);
	}
	//printf("6 done\n");
	//7. wait until no more candy
	while(!bbuff_is_empty()){
		printf("Waiting for all candy to be consumed.\n");
		sleep(1);
	}
	//printf("7 done\n");
	//8. stop kid threads
	printf("Stopping Kids.\n");
	for(int i = 0; i < kids; i++){
		pthread_cancel(kid_ID[i]);
		pthread_join(kid_ID[i], NULL);
	}
	//printf("8 done\n");
	//9. print statistics
	stats_display();
	//printf("9 done\n");
	//10. clean up allcated momory
	stats_cleanup();
	free(factory_ID);
	free(kid_ID);
	free(variable);
	//printf("10 done\n");
	return 0;
}
