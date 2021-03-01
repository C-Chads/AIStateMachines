#define LOCKSTEPTHREAD_IMPL
#include "lockstepthread.h"

#include "fluffle.h"
















fluffle* array = NULL;
unsigned long long n = 10;

void process_state_machines(){
	SM_NEXT_HCODE(fluffle);	//"Handled Code" this is a way of avoiding creating an array of booleans.
	//Cache efficient way to handle states.
	uint32_t q = 0;
	for(uint32_t i = 0; i < n; i++)
		if(array[i].state == happy && SM_HCODE_CHECK(fluffle, array[i]))
			{SM_HANDLE_fluffle(array+i);q++;}
	printf("We had %u fluffles that were happy.\n", q);
	q = 0;
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == sad && SM_HCODE_CHECK(fluffle, array[i]))
				{SM_HANDLE_fluffle(array+i);q++;}
	printf("We had %u fluffles that were sad.\n", q);
	q = 0;
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == singing && SM_HCODE_CHECK(fluffle, array[i]))
				{SM_HANDLE_fluffle(array+i);q++;}
	printf("We had %u fluffles that were singing.\n", q);
	q = 0;
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == changing && SM_HCODE_CHECK(fluffle, array[i]))
				{SM_HANDLE_fluffle(array+i);q++;}
	printf("We had %u fluffles that were changing.\n", q);
	q = 0;
	for(uint32_t i = 0; i < n; i++)		//Handle the idle, init, and default states (for state machines that started with an invalid state)
			if(SM_HCODE_CHECK(fluffle, array[i]))
				SM_HANDLE_fluffle(array+i);
}

int main(int argc, char** argv){
	n = 300;
	if(argc > 1) {n = strtoull(argv[1],0,10);puts("Thanks for the argument!\n");}
	
	array = malloc(n * sizeof(fluffle));

	if(!array) return 1;
	srand(time(NULL));
	for(unsigned long long i = 0; i < n; i++){
		array[i].mood = 0;
		INIT_SM(fluffle, array[i]);
	}
	lsthread t1;
	init_lsthread(&t1);
	t1.execute = process_state_machines;
	start_lsthread(&t1);
	for(int i = 0; i < 10000; i++)
	{
		lock(&t1);
		//synced workload.
		puts("Synchronizing state with the main thread doo dee doo...\n");
		if(i == 9999) break;
		step(&t1);
		//Multithreaded workload
		puts("Doing some sort of important task offloaded!\n");
	}
	kill_lsthread(&t1);
	destroy_lsthread(&t1);
	free(array);
}
