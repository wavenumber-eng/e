
#include "stdint.h"
#include "stdbool.h"

#ifndef __E_BTN_H
#define __E_BTN_H

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
    
    BTN_STATE__WAIT_FOR_PRESS                        =     0,
    BTN_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE        =     1,
    BTN_STATE__WAIT_FOR_RELEASE                      =     2,
    BTN_STATE__WAIT_FOR_STABLE                       =     3,
    BTN_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES     =     4

} e_btn_state_e;


// Forward declaration
struct e_btn;

// Now define the function pointer typedef
typedef bool (*e_btn_read_t)(struct e_btn *button);
typedef bool (*e_btn_up_t)(struct e_btn *button);
typedef bool (*e_btn_down_t)(struct e_btn *button);

typedef struct e_btn
{
    struct e_btn *next;                             //For a linked list.
    volatile bool down;                     //Flag to indicate button was pressed down. Set only if there isn't a callback registered
    volatile bool up;                       //Flag to indicate button was release et only if there isn't a callback registered
    e_btn_down_t down_cb;                   //If this is populated, you get callback, else the down flag is set
    e_btn_down_t up_cb;                     //If this is populated you get a call, else the up flag is set
    e_btn_read_t read;                      //HAL function to read the button
    volatile uint32_t   hold_time;          //Number of ticks the butt was pressed for
    uint32_t  debounce_time_ms;             //Number of ticks for the debounce time
    volatile uint32_t debounce_timer;       //used internally for the debouncer algorithm
    volatile e_btn_state_e state;           //State of the debounce algorithm
    void * user;
} e_btn_t;

#define BTN__MAKE(_name, _debounce_time_ms, read_handler)     \
	    e_btn_t _name = {                  \
		.debounce_time_ms = _debounce_time_ms,                     \
		.read =read_handler,                    \
		.down = 0,                    \
		.up = 0}

#define E_BTN__MAKE BTN__MAKE

void e_btn__init(e_btn_t *btn,
                 uint32_t debounce_time_ms,
                 e_btn_read_t read
                 );

// Computes the debounce algorithm. Must be called periodically
// process time is the interval in milliseconds between calls to this function
void e_btn__crunch(e_btn_t *btn , uint32_t process_time_ms);

//Indicates that the button is currently being help down
uint32_t e_btn__is_active(e_btn_t *btn);

//Reads the button down flag.  Flag will be cleared after read
uint32_t e_btn__down(e_btn_t *btn);

//Reads the button up flag.  Flag will be cleared after read
//Returns the number of ticks the button was held for
uint32_t e_btn__up(e_btn_t *btn);

//Set the button down flag.  This is used for programmatic button presses
void e_btn__programmatic_down(e_btn_t *btn);

//Set the button up flag.  This is used for programmatic button presses
void e_btn__programmatic_up(e_btn_t *btn);

//returns the current hold time from the debounce algorithm
uint32_t e_btn__get_current_hold_time(e_btn_t *btn);

//resets the button state and waits for a release (button up but generates no up or down code)
void e_btn__reset_state (e_btn_t *btn);

bool e_btn__add_to_list(e_btn_t *btn_list,e_btn_t *btn);


/*============================================================
 * Button Tap Detector
 *
 * Higher-level processor that sits on top of e_btn.
 * Detects single, double, triple (N) tap sequences by
 * counting UP events within a configurable gap window.
 *
 * Usage:
 *   e_btn_tap_t tap;
 *   e_btn_tap__init(&tap, 10, 400, my_read_fn);
 *   // In periodic loop (e.g. every 10ms):
 *   e_btn_tap__crunch(&tap, 10);
 *   uint32_t ev = e_btn_tap__get_event(&tap);
 *   if (ev == 1)  // single tap
 *   if (ev == 2)  // double tap
 *
 * Note: single-tap events are delayed by gap_window_ms
 * (the time we wait to see if another tap is coming).
 *============================================================*/

typedef enum
{
    BTN_TAP_STATE__IDLE     = 0,
    BTN_TAP_STATE__COUNTING = 1

} e_btn_tap_state_e;

// Forward declaration
struct e_btn_tap;

// Callback: fired when a tap sequence completes.
// press_count: 1 = single, 2 = double, 3 = triple, etc.
typedef void (*e_btn_tap_cb_t)(struct e_btn_tap *tap, uint32_t press_count);

typedef struct e_btn_tap
{
    struct e_btn_tap *next;             // Linked list support
    e_btn_t btn;                        // Underlying debounced button
    e_btn_tap_cb_t event_cb;            // If set: callback. If NULL: poll via get_event
    uint32_t gap_window_ms;             // Max gap between presses (e.g. 400ms)
    uint32_t gap_timer;                 // Internal timer
    volatile uint32_t event;            // Latched tap count (0 = no event). Cleared on read.
    uint32_t count;                     // Internal press counter during sequence
    volatile e_btn_tap_state_e state;   // IDLE / COUNTING
    void *user;                         // User context
} e_btn_tap_t;

// Initialize tap detector. Sets up inner e_btn with debounce and read callback.
void e_btn_tap__init(e_btn_tap_t *tap,
                     uint32_t debounce_time_ms,
                     uint32_t gap_window_ms,
                     e_btn_read_t read);

// Process tap detector. Walks linked list. Call periodically.
void e_btn_tap__crunch(e_btn_tap_t *tap, uint32_t process_time_ms);

// Read tap event. Returns press count (1=single, 2=double, N=N taps).
// Returns 0 if no event. Clears event after read.
uint32_t e_btn_tap__get_event(e_btn_tap_t *tap);

// Add tap detector to linked list
bool e_btn_tap__add_to_list(e_btn_tap_t *list, e_btn_tap_t *tap);


#ifdef __cplusplus
}
#endif

#endif
