
#ifndef _E_MSG_H_
#define _E_MSG_H_


#include "e_queue.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef CONFIG__E_MSG_PAYLOAD_MAX_LENGTH
    #define CONFIG__E_MSG_PAYLOAD_MAX_LENGTH  1024
#endif


typedef struct e_msg_proc_inst e_msg_proc_inst_t;
typedef void (*e_msg_handler_t)(e_msg_proc_inst_t *);

typedef struct e_msg_proc_inst
{
    void * user_object;

    uint16_t payload_length;
    uint16_t i_payload; //current payload counter

    uint8_t payload[CONFIG__E_MSG_PAYLOAD_MAX_LENGTH];

    uint16_t rx_crc16;
    uint16_t calc_crc16;
    e_msg_handler_t msg_handler;
    uint32_t last_byte_rx_time_msec;
    byte_queue_t * rx_queue;
    uint32_t timeout;
    uint32_t detect_state;

} e_msg_proc_inst_t;

void e_msg__init(e_msg_proc_inst_t *msi,
                 byte_queue_t * rx_queue,
                 e_msg_handler_t msg_handler,
                 uint32_t timeout,
                 void * user);

extern void e_msg__reset(e_msg_proc_inst_t * msi);
extern void e_msg__crunch(e_msg_proc_inst_t *msi);
extern void e_msg__feed(e_msg_proc_inst_t *msi, uint8_t next_byte_in);

uint32_t e_msg__frame_into_q(byte_queue_t *output_queue,
                          uint8_t *payload,
                          uint8_t payload_length);

#endif /* E_SRC_E_MSG_H_ */
