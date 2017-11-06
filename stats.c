//stats.c
#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
typedef struct{
	int made;
	int eaten;
	double min_time;
	double max_time;
	double avg_time;
	int count_time;
}statics_t;

enum{
	false = 0,
	true = 1
};

static statics_t * record;
int num_record = 0;

void stats_init(int num_producers){
	record = (statics_t *)malloc(sizeof(statics_t)*(num_producers));
	for(int i = 0; i < num_producers; i++){	
		record[i].made = 0;
		record[i].eaten = 0;
		record[i].min_time = 0;
		record[i].max_time = 0;
		record[i].avg_time = 0;
		record[i].count_time = 0;
	}
	num_record = num_producers;
}

void stats_cleanup(void){
	//free the memory
	free(record);
}

void stats_record_produced(int factory_number){
	record[factory_number].made++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	//printf("delay_in_m%lf\n", delay_in_ms);
	record[factory_number].eaten++;
	if(delay_in_ms > record[factory_number].max_time){
		record[factory_number].max_time = delay_in_ms;
	} 
	if(delay_in_ms < record[factory_number].min_time || record[factory_number].count_time == 0){
		record[factory_number].min_time = delay_in_ms;
	}
	record[factory_number].avg_time = record[factory_number].avg_time*record[factory_number].count_time;
	record[factory_number].avg_time += delay_in_ms;
	record[factory_number].count_time ++;
	record[factory_number].avg_time /= record[factory_number].count_time;
}

void stats_display(void){
	printf("Statistics:\n");
	printf("%10s %6s %6s %15s %15s %15s\n", "Factory#", "#Maden", "#Eaten", "Min_Delay[ms]", "Avg_delay[ms]", "Max_delay[ms]");
	_Bool mis_match = false;
	for(int i = 0; i < num_record; i++){
		if(record[i].made != record[i].eaten){
			mis_match = true;
		}
		printf("%10d %6d %6d %15.5lf %15.5lf %15.5lf\n", i, record[i].made, record[i].eaten, record[i].min_time, record[i].avg_time, record[i].max_time);		
		//printf("%10d %6d %6d %10.5lf %10.5f\n", i, record[i].made, record[i].eaten, record[i].min_time, record[i].max_time);
	}	
	if(mis_match){
		printf("“ERROR: Mismatch between number made and eaten\n");
	}
}