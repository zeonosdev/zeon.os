#include "zeonos_h"

uint32_t heap_pointer = KERNEL_HEAP_START;
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void* kernel_malloc(uint32_t size) {
    if(heap_pointer + size > KERNEL_HEAP_START + KERNEL_HEAP_SIZE) return 0;
    uint32_t* pointer = (uint32_t*)heap_pointer;
    heap_pointer += size;
    return (void*)pointer;
}

void init_paging(void) {
    for(int i = 0; i < 1024; i++) page_directory[i] = 0x00000002;
    for(int i = 0; i < 1024; i++) first_page_table[i] = (i * 0x1000) | 3;

    page_directory[0] = ((uint32_t)first_page_table) | 3;

    asm volatile(
        "mov %0, %%cr3\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        : : "r"(page_directory) : "%eax"
    );
    kernel_print("MMU: Paging Enabled (Identity Mapped 0-4MB)\n", COLOR_GREEN);
}
