#ifndef INC_RING_BUFFER_H
#define INC_RING_BUFFER_H

#include "common-defines.h"


typedef struct ring_buffer_t{
    // pointer type doesn't matter as we use element_size to do pointer arithmetic, it will only matter if we try to access is like an array
    uint8_t * buffer; 
    uint32_t read_index;
    uint32_t write_index;
    uint32_t size; //size is always power of 2
    uint32_t mask;
    uint32_t element_size;
} ring_buffer_t;

void ring_buffer_setup(ring_buffer_t *rb, uint32_t size, uint32_t element_size, void* element);
bool ring_buffer_empty(ring_buffer_t *rb);
bool ring_buffer_read(ring_buffer_t *rb, void *byte);
bool ring_buffer_write(ring_buffer_t *rb, void *byte);

#endif /* INC_RING_BUFFER_H */