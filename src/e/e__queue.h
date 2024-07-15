#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef QUEUE_H_
#define QUEUE_H_

#define CONFIG__BQ_MAX_PRINTF_LEN 64

typedef void (*bq_hook_t)(void * arg);

typedef struct {

    uint32_t read_ptr;
    uint32_t write_ptr;
    uint32_t size;
    uint8_t *storage;
    void *hook_arg;
    bq_hook_t enqueue_hook;

} byte_queue_t;

#define QUEUE_FULL       -1
#define QUEUE_EMPTY      -2
#define QUEUE_OK          0

void 		bq__init(byte_queue_t *bq,uint32_t size,uint8_t * storage);
uint32_t	bq__bytes_available(byte_queue_t *bq);
void	    bq__clear(byte_queue_t *bq);
int32_t 	bq__enqueue(byte_queue_t *bq,uint8_t Val);
int32_t 	bq__enqueue_array(byte_queue_t *bq,uint8_t *Buf,uint32_t len);
int32_t 	bq__dequeue(byte_queue_t *bq,uint8_t *Val);
uint8_t 	bq__dequeue_next(byte_queue_t *bq);
void 		bq__dequeue_array(byte_queue_t *bq,uint8_t *val,uint32_t len);
int32_t     bq__printf(byte_queue_t *bq, const char *format_string,...);



#define BYTE_QUEUE__MAKE(name,length) uint8_t bq__##name##__storage[length];\
									  byte_queue_t name = { \
									 .read_ptr=0,\
									 .write_ptr=0,\
								     .size = length,	\
								     .storage = bq__##name##__storage\
								      }

#endif /* QUEUE_H_ */
