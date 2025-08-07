#include "stddef.h"
#include "stdint.h"

#ifndef _E_ACTIVITY_H
#define _E_ACTIVITY_H

typedef enum
{
    eGFX_ACTIVITY_MSG_ID_RESET	= -1,
    eGFX_ACTIVITY_MSG_ID_NO_MSG	= -2,
   	eGFX_ACTIVITY_MSG_ID_POP	= -3,
    eGFX_ACTIVITY_MSG_ID_PUSH	= -4,
    eGFX_ACTIVITY_MSG_ID_SWITCH	= -5,
   	eGFX_ACTIVITY_MSG_ID_STRING	= -6
} e_act_msg_e;


typedef enum
{
    e_activity_stack_full	=	-1,
    e_activity_stack_empty	=	-2,
    e_activity_invalid		=	-3,
    e_activity_ok			=	 0
}e_act_err_e;



typedef struct
{
    char * name;

    int32_t id;

    void * user;
    
    void (*enter)(int32_t msg_id, void *msg);

	void (*crunch)(void * user);

	void (*exit)(int32_t msg_id, void *msg);

}e_act_t;


#define ACTIVITY__MAKE(_name,_id,_user,_enter,_crunch,_exit)     \
		e_act_t _name = {         \
		.name = #_name,           \
		.id = _id,                \
        .user = _user,            \
        .enter = _enter,          \
		.crunch = _crunch,        \
		.exit = _exit}

e_act_err_e e_act__switch(e_act_t *new_activity,int32_t msg_id,void *msg);

e_act_err_e e_act__push(e_act_t *new_activity,int32_t msg_id,void *msg);

e_act_err_e e_act__pop(int32_t msg_id,void *msg);

void e_act__crunch();

e_act_t * e_act__current();


#endif /* ACTITIVY_H_ */
