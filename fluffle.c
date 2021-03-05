#include "fluffle.h"

SM_GLOBALS(fluffle);


//Define the state machine 

SM_HANDLER(fluffle)
//puts("I have awakened.\n");
STATE(idle):
{
	int a = (int)(rand()%256);
	a -= 128;
	sm->target = a + sm->mood;
	if(sm->target < -100)
		sm->target = -100;
	if(sm->target > 100)
		sm->target = 100;
}
STATE(changing):
//puts("Changing...\n");
if(sm->target > sm->mood)
	{sm->mood++; JMP_STATE(changing);}
if(sm->target < sm->mood)
	{sm->mood--; JMP_STATE(changing);}
//We have reached the target mood.
if(sm->mood > 80)
	JMP_STATE(singing);
if(sm->mood > 30)
	JMP_STATE(happy);
if(sm->mood < 30)
	JMP_STATE(sad);
JMP_STATE(idle);
STATE(happy):
//puts("Happy noises!\n");
JMP_STATE(idle);

STATE(singing):
//puts("Fo la lo di!\n");
JMP_STATE(idle);

STATE(sad):
//puts("Sad noises.\n");
JMP_STATE(idle);

END_SM_HANDLER
