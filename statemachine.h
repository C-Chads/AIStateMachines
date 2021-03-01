#include <stdint.h>
//CC0 public domain state machine framework.
#define SM_NEXT_HCODE(typename) SM_HCODE_##typename ++;
#define SM_HCODE_CHECK(typename, elem) (elem.hcode != SM_HCODE_##typename )

#define SM_VARS int state; unsigned int hcode;
#define SM_VARS_EXTERN extern int state; extern unsigned int hcode;

#define JMP_STATE(n) {sm->state = n; break;}
#define JMP_INIT()	{sm->state = 0; break;}

#define SM_HANDLER(typename) void SM_HANDLE_##typename( typename *sm) { sm->hcode = SM_HCODE_##typename ; switch(sm->state) { case 0: default:

#define SM_EXTERN_HANDLER(typename) extern void SM_HANDLE_##typename( typename *sm);

#define STATE(n)	case n

#define END_SM_HANDLER	} }

#define INIT_SM(typename, var) var.state = 0; var.hcode = SM_HCODE_##typename;

#define SM_GLOBALS(typename) unsigned int SM_HCODE_##typename = 0;

#define SM_EXTERN_GLOBALS(typename) extern unsigned int SM_HCODE_##typename;

#define ENUM_SM(typename) 	enum{ sm_init_##typename = 0,

#define ENUM_END_SM };
