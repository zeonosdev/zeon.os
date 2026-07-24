#include "zeonos.h"

uint32_t heap_pointer = KERNEL_HEAP_START;

void* kernel_malloc(uint32_t size) {
    if(heap_pointer + size > KERNEL_HEAP_START + KERNEL_HEAP_SIZE) return 0;
    uint32_t* pointer = (uint32_t*)heap_pointer;
    heap_pointer += size;
    return (void*)pointer;
}

void init_paging(void) {
    kernel_print("MMU: Paging System Standby\\n", COLOR_LIGHT_BLUE);
}
