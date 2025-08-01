#include "e_btn.h"

#ifndef NULL
    #define NULL 0
#endif

void e_btn__init(e_btn_t *btn,
                    uint8_t debounce_time_ms,
                    e_btn_read_t read
                 )
{
    if(btn!=NULL)
    {
        btn->debounce_time_ms = debounce_time_ms;
        btn->down = 0;
        btn->up = 0;
        btn->hold_time = 0;
        btn->state = BTN_STATE__WAIT_FOR_PRESS;
        btn->debounce_timer = 0;
    }
}

void e_btn__crunch(e_btn_t *btn, uint32_t process_time_ms)
{
    if(btn == NULL)
    {
        return;
    }
    
    if(btn->read == NULL)
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
                    if(btn->down_cb != NULL)
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
                   
                    if(btn->up_cb != NULL)
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



