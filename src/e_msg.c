#include "e_util.h"
#include "e_msg.h"
#include "e_tick.h"
#include "e_queue.h"
#include "e_debug.h"


#ifndef CONFIG__E_MSG_HEADER_1
    #define CONFIG__E_MSG_HEADER_1        0xBE
#endif

#ifndef CONFIG__E_MSG_HEADER_2
    #define CONFIG__E_MSG_HEADER_2        0xEF
#endif

#ifndef CONFIG__E_MSG_HEADER_3
    #define CONFIG__E_MSG_HEADER_3        0xAA
#endif

#ifndef CONFIG__E_MSG_HEADER_4
    #define CONFIG__E_MSG_HEADER_4        0x55
#endif


 //Command detection possible states
typedef enum {
    SCAN_HEADER_1,
    SCAN_HEADER_2,
    SCAN_HEADER_3,
    SCAN_HEADER_4,
    SCAN_PAYLOAD_LENGTH_L,
    SCAN_PAYLOAD_LENGTH_H,
    SCAN_PAYLOAD,
    SCAN_CRC_1,
    SCAN_CRC_2,
    NUM_OF_MSG_DETECT_STATES,
} msg_detect_state;


#define ADD_TO_BUFFER(idx, buffer, data) buffer[idx++] = data;

#define ADD_TO_BUFFER_CRC(idx, buffer, data, crc) \
    buffer[idx++] = data;                         \
    crc = e__crc16_ccit_step(crc, data)

#define ADD_TO_QUEUE(queue_ptr, data)       bq__enqueue(queue_ptr, data)

#define ADD_TO_QUEUE_CRC(queue_ptr, data, crc)  \
    bq__enqueue(queue_ptr, data);               \
    crc = e__crc16_ccit_step(crc, data)


#define MSG_HANDLER_ERR(...) E_LOG_ERR(__VA_ARGS__)
#define MSG_HANDLER_DBG(...) E_LOG_DBG(__VA_ARGS__)


void e_msg__init(e_msg_proc_inst_t *msi,
                 byte_queue_t * rx_queue,
                 e_msg_handler_t msg_handler,
                 uint32_t timeout,
                 void * user)
{

    if (msi == NULL)
    {
        MSG_HANDLER_ERR("NULL command_handler_instance, abort...");
        return;
    }

    if(msg_handler == NULL)
    {
        MSG_HANDLER_ERR("NULL msg_handler function, abort...");
        return;
    }

    msi->detect_state = SCAN_HEADER_1;
    msi->last_byte_rx_time_msec = 0;
    msi->msg_handler = msg_handler;
    msi->user_object = user;
    msi->timeout = timeout;
}

void e_msg__reset(e_msg_proc_inst_t *msi)
{
     msi->detect_state = SCAN_HEADER_1;
}


void e_msg__process(e_msg_proc_inst_t *msi, uint8_t next_byte_in)
{

    if (msi == NULL)
    {
        return;
    }

    if( msi->detect_state != SCAN_HEADER_1)
    {
        if(e_tick__timeout(&msi->last_byte_rx_time_msec, msi->timeout))
        {
            msi->detect_state = SCAN_HEADER_1;
            MSG_HANDLER_ERR("Message timeout\r\n");
        }
        else
        {
            msi->last_byte_rx_time_msec = e_tick__get_ms();
        }
    }
    else
    {
        msi->last_byte_rx_time_msec = e_tick__get_ms();
    }


    // State machine execution
    switch (msi->detect_state)
    {
    case SCAN_HEADER_1:
        if (next_byte_in == CONFIG__E_MSG_HEADER_1)
        {
            msi->calc_crc16 = E__CRC_CCIT_SEED;
            msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
            msi->detect_state = SCAN_HEADER_2;
        }
        else
        {
            msi->detect_state = SCAN_HEADER_1;
        }
        break;

    case SCAN_HEADER_2:
        if (next_byte_in == CONFIG__E_MSG_HEADER_2)
        {
            msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
            msi->detect_state = SCAN_HEADER_3;
        }
        else
        {
            msi->detect_state = SCAN_HEADER_1;
        }
        break;

    case SCAN_HEADER_3:
        if (next_byte_in == CONFIG__E_MSG_HEADER_3)
        {
            msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
            msi->detect_state = SCAN_HEADER_4;
        }
        else
        {
            msi->detect_state = SCAN_HEADER_1;
        }
        break;

    case SCAN_HEADER_4:
        if (next_byte_in == CONFIG__E_MSG_HEADER_4)
        {
            msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
            msi->detect_state = SCAN_PAYLOAD_LENGTH_L;
        }
        else
        {
            msi->detect_state = SCAN_HEADER_1;
        }
        break;

    case SCAN_PAYLOAD_LENGTH_L:
        msi->payload_length = (uint16_t)next_byte_in;
        msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
        msi->detect_state = SCAN_PAYLOAD_LENGTH_H;
        break;

    case SCAN_PAYLOAD_LENGTH_H:
        msi->payload_length |= (uint16_t)(next_byte_in)<<8;
        msi->i_payload = 0;

        if(msi->payload_length > msi->max_payload_length)
        {
            MSG_HANDLER_ERR("Incoming Message too large");
            msi->detect_state = SCAN_HEADER_1;
        }
        else
        {
            msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
            msi->detect_state = SCAN_PAYLOAD;
        }
        break;

    case SCAN_PAYLOAD:
        msi->payload[msi->i_payload] = next_byte_in;
        msi->i_payload++;
        msi->calc_crc16 = e__crc16_ccit_step(msi->calc_crc16, next_byte_in);
        if ((msi->i_payload >= msi->payload_length))
        {
            msi->detect_state = SCAN_CRC_1;
        }
        break;

    case SCAN_CRC_1:
        msi->rx_crc16 = ((uint16_t)next_byte_in);
        msi->detect_state = SCAN_CRC_2;
        break;

    case SCAN_CRC_2:

        msi->rx_crc16 |= (uint16_t)next_byte_in << 8;

        if ((msi->rx_crc16 == msi->calc_crc16))
        {
            if(msi->msg_handler != NULL)
            {
               msi->msg_handler(msi->payload,msi->payload_length, msi);
            }
            else
            {
                MSG_HANDLER_ERR("Command_parser function pointing to NULL\r\n");
            }

            msi->detect_state = SCAN_HEADER_1;

        }

        else
        {
            MSG_HANDLER_ERR("CRC error\n");
            msi->detect_state = SCAN_HEADER_1;
        }
        break;


    default:
        msi->detect_state = SCAN_HEADER_1;
        break;
    }
}



uint32_t e_msg__frame_into_q(byte_queue_t *output_queue,
                          uint8_t *payload,
                          uint8_t payload_length)
{
    uint16_t crc16 = 0xFFFF;
    uint32_t idx = 9;

    int32_t free_bytes = bq__bytes_available_to_write(output_queue);

    if (free_bytes < payload_length + 9)
    {
        MSG_HANDLER_ERR("Message doesn't fit in TX Queue");
        return 0;
    }

    ADD_TO_QUEUE_CRC(output_queue, CONFIG__E_MSG_HEADER_1, crc16);
    ADD_TO_QUEUE_CRC(output_queue, CONFIG__E_MSG_HEADER_2, crc16);
    ADD_TO_QUEUE_CRC(output_queue, CONFIG__E_MSG_HEADER_3, crc16);
    ADD_TO_QUEUE_CRC(output_queue, CONFIG__E_MSG_HEADER_4, crc16);

    ADD_TO_QUEUE_CRC(output_queue, (payload_length&0xff), crc16);
    ADD_TO_QUEUE_CRC(output_queue, (uint8_t)(payload_length>>8), crc16);

    for (uint8_t i = 0; i < payload_length; i++)
    {
        ADD_TO_QUEUE_CRC(output_queue, payload[i], crc16);
        idx++;
    }

    ADD_TO_QUEUE(output_queue, ((uint8_t)crc16));
    ADD_TO_QUEUE(output_queue, ((uint8_t)(crc16 >> 8)));

    return idx;
}
