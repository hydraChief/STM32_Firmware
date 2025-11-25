#include "core/memcpy_t.h"

void memcpy_t(void* dest, const void* src, uint32_t n){
    uint8_t* d=(uint8_t*)dest;
    uint8_t* s=(uint8_t*)src;
    for(uint32_t i=0;i<n;i++){
        d[i]=s[i];
    }
}