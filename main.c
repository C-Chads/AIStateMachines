#define LOCKSTEPTHREAD_IMPL
#include "lockstepthread.h"
#include "statemachine.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
//Typedef your state machine.
typedef struct {
	SM_VARS;
	int mood;
} mysm;



//Enumerate the states of your state machine.
ENUM_SM(mysm)
happy,
sad,
singing,
idle,
ENUM_END_SM;

//Define the state machine 
static inline SM_HANDLER(mysm)
puts("I have awakened.\n");
STATE(idle):
{
	int a = (int)(rand()%256);
	a -= 128;
	sm->mood += a;
	if(sm->mood < -100)
		sm->mood = -100;
	if(sm->mood > 100)
		sm->mood = 100;
	if(sm->mood > 80)
		JMP_STATE(singing);
	if(sm->mood > 30)
		JMP_STATE(happy);
	if(sm->mood == 30)
		JMP_STATE(idle);
	if(sm->mood < 30)
		JMP_STATE(sad);
}
STATE(happy):
puts("Happy noises!\n");
JMP_STATE(idle);

STATE(singing):
puts("Fo la lo di!\n");
JMP_STATE(idle);

STATE(sad):
puts("Sad noises.\n");
JMP_STATE(idle);

END_SM_HANDLER



mysm* array = NULL;
unsigned long long n = 10;

void process_state_machines(){
	for(unsigned long long i = 0; i < n; i++)
		SM_HANDLE_mysm(array+i);
}

int main(int argc, char** argv){
	n = 300;
	if(argc > 1) {n = strtoull(argv[1],0,10);puts("Thanks for the argument!\n");}
	
	array = malloc(n * sizeof(mysm));

	if(!array) return 1;
	srand(time(NULL));
	for(unsigned long long i = 0; i < n; i++){
		array[i].mood = 0;
		INIT_SM(array[i]);
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
