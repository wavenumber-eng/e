
#ifndef _E_MSG_H_
#define _E_MSG_H_

#include "e_queue.h"
#include <stdint.h>
#include <stdbool.h>


#ifndef CONFIG_E_MSG_HANDLER_DEFAULT_TIMEOUT
    #define CONFIG_E_MSG_HANDLER_DEFAULT_TIMEOUT     100
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct e_msg_proc_inst e_msg_proc_inst_t;
typedef void (*e_msg_handler_t)(uint8_t * payload, uint16_t length, e_msg_proc_inst_t *);

typedef struct e_msg_proc_inst
{
    uint16_t payload_length;
    uint16_t i_payload;
    uint8_t *payload;
    uint16_t max_payload_length;
    uint16_t rx_crc16;
    uint16_t calc_crc16;
    e_msg_handler_t msg_handler;
    uint32_t last_byte_rx_time_msec;
    uint32_t timeout;
    uint32_t detect_state;
    void * user_object;
} e_msg_proc_inst_t;


#define MSG_PROC__MAKE(name,length,handler)   uint8_t  msg_##name##__payload__storage[length];\
                                               e_msg_proc_inst_t name = { \
                                              .max_payload_length=length, \
                                              .payload=msg_##name##__payload__storage,\
                                              .timeout=CONFIG_E_MSG_HANDLER_DEFAULT_TIMEOUT,\
                                              .msg_handler=handler\
                                              }

void e_msg__init(e_msg_proc_inst_t *msi,
                 byte_queue_t * rx_queue,
                 e_msg_handler_t msg_handler,
                 uint32_t timeout,
                 void * user);

extern void e_msg__reset(e_msg_proc_inst_t * msi);

extern void e_msg__crunch(e_msg_proc_inst_t *msi);

extern void e_msg__feed(e_msg_proc_inst_t *msi, uint8_t next_byte_in);

void e_msg__process(e_msg_proc_inst_t *msi, uint8_t next_byte_in);

uint32_t e_msg__frame_into_q(byte_queue_t *output_queue,
                          uint8_t *payload,
                          uint8_t payload_length);

#ifdef __cplusplus
}
#endif

#endif /* E_SRC_E_MSG_H_ */
