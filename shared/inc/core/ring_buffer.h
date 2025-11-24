#ifndef INC_RING_BUFFER_H
#define INC_RING_BUFFER_H

#include "common-defines.h"

typedef struct ring_buffer_t{
    uint32_t * buffer;
    uint32_t read_index;
    uint32_t write_index;
    uint32_t size; //size is always poer of 2
    uint32_t mask;
} ring_buffer_t;

void ring_buffer_setup(ring_buffer_t *rb, uint32_t *buffer, uint32_t size);
bool ring_buffer_empty(ring_buffer_t *rb);
bool ring_buffer_read(ring_buffer_t *rb, uint32_t *byte);
bool ring_buffer_write(ring_buffer_t *rb, uint32_t byte);

#endif /* INC_RING_BUFFER_H */