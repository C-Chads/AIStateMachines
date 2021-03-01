#include <stdint.h>
//CC0 public domain state machine framework.

#define SM_VARS int state

#define JMP_STATE(n) {sm->state = n; break;}
#define JMP_INIT()	{sm->state = 0; break;}

#define SM_HANDLER(typename) void SM_HANDLE_##typename( typename *sm) {switch(sm->state) { case 0: default:

#define STATE(n)	case n

#define END_SM_HANDLER	} }

#define INIT_SM(var) var.state = 0

#define ENUM_SM(typename)	enum{ sm_init_##typename = 0,

#define ENUM_END_SM };
