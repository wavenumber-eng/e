#include "e.h"
#include "e_activity.h"
#include "stdarg.h"


#ifdef CONFIG__ENABLE_ACTIVITY_DBG
    #define ACT_ERR(...)    E_ERR(__VA_ARGS__); E_ERR("\n")
#else
    #define ACT_ERR(...)
#endif

#ifndef CONFIG__E_NULL
        #define CONFIG__E_NULL      NULL
#endif
        
#ifndef CONFIG__E_ACTIVITY__STACK_DEPTH
    #define CONFIG__E_ACTIVITY__STACK_DEPTH 16 
#endif

static  uint32_t        act_stack_pointer = 0;
static  e_act_t        *act_stack[CONFIG__E_ACTIVITY__STACK_DEPTH];
static  e_act_t        *current_activity = CONFIG__E_NULL;

e_act_t * e_activity__current()
{
        return current_activity;
}

void e_activity__stack_reset()
{
        act_stack_pointer = 0;
}

e_act_err_e e_activity__switch(e_act_t *new_activity,int32_t msg_id,void *msg)
{
        if(new_activity == CONFIG__E_NULL)
        {
            ACT_ERR("e_activity__switch: new_activity is NULL");
            return e_activity_invalid;
        }

        e_activity__stack_reset();
         
        //Finish the current activity
        if(current_activity!=CONFIG__E_NULL)
        {
            if(current_activity->exit != CONFIG__E_NULL)
            {
                current_activity->exit(eGFX_ACTIVITY_MSG_ID_SWITCH,(void *)CONFIG__E_NULL);
            }
            else
            {
                ACT_ERR("e_activity__switch: current_activity->exit is NULL");
            }
        }
        else
        {
            ACT_ERR("e_activity__switch: current_activity is NULL, no exit to call");
        }
        
        //Switch to the new activity
        current_activity = new_activity;

        if(current_activity->enter != CONFIG__E_NULL)
        {
            current_activity->enter(msg_id,msg);
        }
        else
        {
            ACT_ERR("e_activity__switch: current_activity->enter is NULL");
        }

        //Todo, implement a global hook for clearing button activity, etc

        return e_activity_ok;
}

e_act_err_e e_activity__push(e_act_t *new_activity,int32_t msg_id,void *msg)
{
        if(new_activity == CONFIG__E_NULL)
        {
            ACT_ERR("e_activity__push: new_activity is NULL");
            return e_activity_invalid;
        }

        if(act_stack_pointer>=CONFIG__E_ACTIVITY__STACK_DEPTH)
        {
            ACT_ERR("e_activity__push: stack full (depth=%d)", CONFIG__E_ACTIVITY__STACK_DEPTH);
            return e_activity_stack_full;
        }

        if(current_activity == CONFIG__E_NULL)
        {
            ACT_ERR("e_activity__push: current_activity is NULL, calling switch instead");
            return e_activity__switch(new_activity,msg_id,msg);
        }

        //Finish the current activity and tell it it will be pushed
        if(current_activity->exit != CONFIG__E_NULL)
        {
            current_activity->exit(eGFX_ACTIVITY_MSG_ID_PUSH,(void *)CONFIG__E_NULL);
        }
        else
        {
            ACT_ERR("e_activity__push: current_activity->exit is NULL");
        }

        //push the current activity on the stack
        act_stack[act_stack_pointer] = current_activity;
        act_stack_pointer++;

        //switch to the new activity
        current_activity = new_activity;

        //send the push arguments to the new activity
        if(current_activity->enter != CONFIG__E_NULL)
        {
            current_activity->enter(msg_id,msg);
        }
        else
        {
            ACT_ERR("e_activity__push: current_activity->enter is NULL");
        }

        //ToDo Add a global hook

        return e_activity_ok;

}

e_act_err_e e_activity__pop(int32_t msg_id,void *msg)
{
        //See if the stack is empty
        if(act_stack_pointer == 0)
        {
            ACT_ERR("e_activity__pop: stack is empty");
            return e_activity_stack_empty;
        }

        //Exit the current activity.  We need to flag it that we are doing a pop (which is really an acknowledge)
        if(current_activity != CONFIG__E_NULL)
        {
            if(current_activity->exit != CONFIG__E_NULL)
            {
                current_activity->exit(eGFX_ACTIVITY_MSG_ID_POP,(void *)CONFIG__E_NULL);
            }
            else
            {
                ACT_ERR("e_activity__pop: current_activity->exit is NULL");
            }
        }
        else
        {
            ACT_ERR("e_activity__pop: current_activity is NULL");
        }

        //Get the activity off the stack
        act_stack_pointer--;

        current_activity = act_stack[act_stack_pointer];

        if(current_activity == CONFIG__E_NULL)
        {
            ACT_ERR("e_activity__pop: activity from stack is NULL");
        }

        //Enter the new activity with the parameters returned from the old
        if(current_activity != CONFIG__E_NULL && current_activity->enter != CONFIG__E_NULL)
        {
            current_activity->enter(msg_id,msg);
        }
        else
        {
            if(current_activity == CONFIG__E_NULL)
            {
                ACT_ERR("e_activity__pop: popped activity is NULL");
            }
            else
            {
                ACT_ERR("e_activity__pop: popped activity->enter is NULL");
            }
        }

        //ToDo add a global hook

        return e_activity_ok;
}

void e_activity__crunch()
{
    if(current_activity != CONFIG__E_NULL)
    {
        if(current_activity->crunch != CONFIG__E_NULL)
        {
            current_activity->crunch(current_activity->user);
        }
        else
        {
            ACT_ERR("e_activity__crunch: current_activity->crunch is NULL");
        }
    }
    else
    {
        ACT_ERR("e_activity__crunch: current_activity is NULL");
    }
}