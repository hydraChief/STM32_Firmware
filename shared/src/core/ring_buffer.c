#include "core/ring_buffer.h"
#include "common-defines.h"
#include "core/memcpy_t.h"

void ring_buffer_setup(ring_buffer_t *rb, uint32_t size, uint32_t element_size, void* element){
    rb->buffer=(uint8_t*)element;
    rb->size=size;
    rb->read_index=0U;
    rb->write_index=0U;
    rb->mask=size-1U;
    rb->element_size=element_size;
}

bool ring_buffer_empty(ring_buffer_t *rb){
    return rb->read_index==rb->write_index;
}

bool ring_buffer_read(ring_buffer_t *rb, void *byte){
    uint32_t local_read_index=rb->read_index;
    uint32_t local_write_index=rb->write_index;
    if(local_read_index==local_write_index){
        return false;
    }
    // *byte=rb->buffer[local_read_index];
    memcpy_t(byte, rb->buffer + (local_read_index* rb->element_size), rb->element_size);
    rb->read_index=(local_read_index+1U)& rb->mask;
    return true;
}

bool ring_buffer_write(ring_buffer_t *rb, void* byte){
    uint32_t local_write_index=rb->write_index;
    uint32_t local_read_index=rb->read_index;
    uint32_t next_write_index=(local_write_index+1U)& rb->mask;
    if(local_read_index==next_write_index){
        return false;
    }
    // rb->buffer[local_write_index]=*(uint32_t*)byte;
    memcpy_t(rb->buffer + (local_write_index* rb->element_size), byte, rb->element_size);
    rb->write_index=next_write_index;
    return true;
}
