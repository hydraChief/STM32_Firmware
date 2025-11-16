#include "common-defines.h"
#include <libopencm3/stm32/memorymap.h>

#define BOOTLOADER_SIZE 0x8000U
#define FIRMWARE_START_ADDRESS (FLASH_BASE + BOOTLOADER_SIZE)

static void firmware_init(void){
    typedef void (*void_fn)(void);
    uint32_t* reset_vector_entry = (uint32_t*)(FIRMWARE_START_ADDRESS + 4U);
    uint32_t* reset_vector = (uint32_t*)(*reset_vector_entry);

    void_fn jumpfn = (void_fn)reset_vector;
  
    jumpfn();
}

int main(void){
    firmware_init();
    return 0;
}