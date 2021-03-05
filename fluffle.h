#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "statemachine.h"
//Typedef your state machine.
typedef struct {
	SM_VARS;
	int mood;
	int target;
} fluffle;

SM_EXTERN_GLOBALS(fluffle);

//Enumerate the states of your state machine.
ENUM_SM(fluffle)
happy,
sad,
singing,
idle,
changing,
ENUM_END_SM;

SM_EXTERN_HANDLER(fluffle)
