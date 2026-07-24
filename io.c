#include "zeonos.h"

void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void memory_set(void* destination, uint8_t value, uint32_t count) {
    uint8_t* dest_ptr = (uint8_t*)destination;
    for (uint32_t i = 0; i < count; i++) {
        dest_ptr[i] = value;
    }
}

void memory_copy(void* destination, const void* source, uint32_t count) {
    uint8_t* dest_ptr = (uint8_t*)destination;
    const uint8_t* src_ptr = (const uint8_t*)source;
    for (uint32_t i = 0; i < count; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

uint32_t string_length(const char* string) {
    uint32_t length = 0;
    while (string[length] != '\0') length++;
    return length;
}

int string_compare(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++; str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}
