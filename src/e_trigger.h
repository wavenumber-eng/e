#include "stdint.h"

#ifndef _E_TRIGGER_H
#define _E_TRIGGER_H

#ifdef __cplusplus

extern "C"
{
#endif

#define E_TRIG__FOREVER		(-1)

typedef struct e_trigger e_trigger_t;

typedef void (*e_trig__hook_t)(e_trigger_t *trig);

typedef struct e_trigger 
{
    struct e_trigger *next;    // internal singly linked list
    int32_t interval__ms;      // how often to fire
    int32_t duration__ms;      // time until 1st fire
    int32_t count;             // number of times to call, set to E_TRIG__FOREVER to keep triggering
    e_trig__hook_t hook;       // function to call when timer fires
    bool marked_for_exe;       // flag for hook call
    void *arg;                 // A user object to use however

} e_trigger_t;

void e_trig__crunch();

void e_trig__register(e_trigger_t *trigger, bool irq_exe);

//This needs called from irq/thread context for the IRQ List
void e_trig__irq(uint32_t irq_interval_ms);

#ifdef __cplusplus
}
#endif


#endif
