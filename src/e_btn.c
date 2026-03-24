#include "e_btn.h"

#ifndef NULL
    #define NULL   0
#endif

#ifndef CONFIG_E_NULL
        #define CONFIG_E_NULL      NULL
#endif

#define E_BTN_DBG(...)

void e_btn__init(e_btn_t *btn,
                    uint32_t debounce_time_ms,
                    e_btn_read_t read
                 )
{
    if(btn!=CONFIG_E_NULL)
    {
        btn->next = CONFIG_E_NULL;
        btn->read = read;
        btn->down_cb = CONFIG_E_NULL;
        btn->up_cb = CONFIG_E_NULL;
        btn->user = CONFIG_E_NULL;
        btn->debounce_time_ms = debounce_time_ms;
        btn->down = 0;
        btn->up = 0;
        btn->hold_time = 0;
        btn->state = BTN_STATE__WAIT_FOR_PRESS;
        btn->debounce_timer = 0;
    }
}


bool e_btn__add_to_list(e_btn_t *btn_list,e_btn_t *btn)
{
    if(btn_list == btn)
    {
        //we can't add to ourselves
        E_BTN_DBG("We add a btn to ourselves");
        return false;
    }
    if(btn_list==CONFIG_E_NULL)
    {
        E_BTN_DBG("btn_list is null.");
        return false;
    }
    if(btn==CONFIG_E_NULL)
    {
        E_BTN_DBG("btn is null.  Cannot add to list.");
        return false;
    }

    while(btn_list->next != CONFIG_E_NULL)
    {
        if(btn_list->next == btn_list)
        {
            //trap the case where we might loop back
            E_BTN_DBG("Circular loop found in list. Breaking here");
            break;
        }
        btn_list = btn_list->next;
    }

    btn_list->next = btn;
 
    return true;
}


void e_btn__crunch_internal(e_btn_t *btn, uint32_t process_time_ms)
{
    if(btn == CONFIG_E_NULL)
    {
        return;
    }
   
    if(btn->read == CONFIG_E_NULL)
    {
        return;
    }

    switch(btn->state)
    {
        default:
        case BTN_STATE__WAIT_FOR_PRESS:
            
            if(btn->read(btn))
            {
                btn->state = BTN_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE;
                btn->debounce_timer = 0;
            }

            break;

        case BTN_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE:

            if(btn->read(btn))
            {
                btn->debounce_timer += process_time_ms;

                if(btn->debounce_timer > btn->debounce_time_ms)
                {
                    btn->state = BTN_STATE__WAIT_FOR_RELEASE;
                    btn->hold_time = btn->debounce_timer;
                    if(btn->down_cb != CONFIG_E_NULL)
                    {
                        btn->down_cb(btn);
                    }
                    else
                    {
                        btn->down = true;
                    }
                }
            }

            else
            {
                btn->state = BTN_STATE__WAIT_FOR_PRESS;
            }

            break;

        case BTN_STATE__WAIT_FOR_RELEASE:

            if(btn->read(btn))
            {
                if(btn->hold_time<0xFFFFFFFF)
                    btn->hold_time += process_time_ms;
            }

            else
            {
                btn->state = BTN_STATE__WAIT_FOR_STABLE;
                btn->debounce_timer = 0;
            }

            break;

        case BTN_STATE__WAIT_FOR_STABLE:

            if(btn->read(btn) == false)
            {

                btn->debounce_timer+= process_time_ms;

                if(btn->debounce_timer > btn->debounce_time_ms)
                {
                    btn->state = BTN_STATE__WAIT_FOR_PRESS;
                   
                    if(btn->up_cb != CONFIG_E_NULL)
                    {
                        btn->up_cb(btn);
                    }
                    else
                    {
                        btn->up = true;
                    }
                }
            }

            else
            {
                btn->debounce_timer = 0;
            }

            break;

        case BTN_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES:

                if(btn->read(btn) == false)
                {
                    btn->debounce_timer += process_time_ms;

                    if(btn->debounce_timer > btn->debounce_time_ms)
                    {
                        btn->state = BTN_STATE__WAIT_FOR_PRESS;
                    }
                }
                else
                {
                    btn->debounce_timer = 0;
                }
            break;


    }

}


void e_btn__crunch(e_btn_t *btn, uint32_t process_time_ms)
{
    while(btn!=CONFIG_E_NULL)
    {
        e_btn__crunch_internal(btn,process_time_ms);
        btn=btn->next;
    }
 }


uint32_t e_btn__is_active(e_btn_t *btn)
{
    if(btn->state == BTN_STATE__WAIT_FOR_RELEASE)
        return true;
    else
        return false;
}

uint32_t e_btn__down(e_btn_t *btn)
{
    uint32_t p = 0;

    if(btn->down == true)
    {
        p = true;
        btn->down = false;
    }

    return p;
}

uint32_t e_btn__up(e_btn_t *B)
{
    uint32_t p = 0;

    if(B->up == true)
    {
        p = B->hold_time;
        B->up = false;
    }
   
    return p;
}

void e_btn__programmatic_down(e_btn_t *btn)
{
    btn->state = BTN_STATE__WAIT_FOR_RELEASE;
    btn->hold_time = 0;
    btn->down = true;

}

void e_btn__programmatic_up(e_btn_t *p)
{
    p->state = BTN_STATE__WAIT_FOR_PRESS;
    p->up = true;
}

uint32_t e_btn__get_current_hold_time(e_btn_t *btn)
{
    uint32_t p = 0;

    if(btn->state == BTN_STATE__WAIT_FOR_RELEASE)
    {
            p = btn->hold_time;
    }

    return p;
}

void e_btn__reset_state(e_btn_t *btn)
{
        btn->state = BTN_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES;
        btn->up = 0;
        btn->down = 0;
        btn->hold_time = 0;
}


/*============================================================
 * Button Tap Detector
 *============================================================*/

void e_btn_tap__init(e_btn_tap_t *tap,
                     uint32_t debounce_time_ms,
                     uint32_t gap_window_ms,
                     e_btn_read_t read)
{
    if(tap == CONFIG_E_NULL)
    {
        return;
    }

    e_btn__init(&tap->btn, debounce_time_ms, read);
    tap->next = CONFIG_E_NULL;
    tap->event_cb = CONFIG_E_NULL;
    tap->gap_window_ms = gap_window_ms;
    tap->gap_timer = 0;
    tap->event = 0;
    tap->count = 0;
    tap->state = BTN_TAP_STATE__IDLE;
    tap->user = CONFIG_E_NULL;
}


static void e_btn_tap__crunch_internal(e_btn_tap_t *tap, uint32_t process_time_ms)
{
    if(tap == CONFIG_E_NULL)
    {
        return;
    }

    // Step the underlying button debouncer
    e_btn__crunch_internal(&tap->btn, process_time_ms);

    // Consume down flag (tap only uses UP events)
    e_btn__down(&tap->btn);

    // Check for release (UP) event
    uint32_t up = e_btn__up(&tap->btn);

    switch(tap->state)
    {
        default:
        case BTN_TAP_STATE__IDLE:

            if(up)
            {
                tap->count = 1;
                tap->gap_timer = 0;
                tap->state = BTN_TAP_STATE__COUNTING;
            }

            break;

        case BTN_TAP_STATE__COUNTING:

            if(up)
            {
                tap->count++;
                tap->gap_timer = 0;
            }
            else
            {
                tap->gap_timer += process_time_ms;

                if(tap->gap_timer > tap->gap_window_ms)
                {
                    if(tap->event_cb != CONFIG_E_NULL)
                    {
                        tap->event_cb(tap, tap->count);
                    }
                    else
                    {
                        tap->event = tap->count;
                    }

                    tap->count = 0;
                    tap->state = BTN_TAP_STATE__IDLE;
                }
            }

            break;
    }
}


void e_btn_tap__crunch(e_btn_tap_t *tap, uint32_t process_time_ms)
{
    while(tap != CONFIG_E_NULL)
    {
        e_btn_tap__crunch_internal(tap, process_time_ms);
        tap = tap->next;
    }
}


uint32_t e_btn_tap__get_event(e_btn_tap_t *tap)
{
    uint32_t e = 0;

    if(tap->event)
    {
        e = tap->event;
        tap->event = 0;
    }

    return e;
}


bool e_btn_tap__add_to_list(e_btn_tap_t *list, e_btn_tap_t *tap)
{
    if(list == tap)
    {
        return false;
    }
    if(list == CONFIG_E_NULL)
    {
        return false;
    }
    if(tap == CONFIG_E_NULL)
    {
        return false;
    }

    while(list->next != CONFIG_E_NULL)
    {
        if(list->next == list)
        {
            break;
        }
        list = list->next;
    }

    list->next = tap;
    return true;
}

