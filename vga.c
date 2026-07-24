#include "zeonos_h"

volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_MEMORY;
int vga_cursor_x = 0;
int vga_cursor_y = 0;

void vga_clear_screen(void) {
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (COLOR_BLACK << 8) | ' ';
    }
    vga_cursor_x = 0;
    vga_cursor_y = 0;
}

void vga_scroll(void) {
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = (COLOR_BLACK << 8) | ' ';
    }
    vga_cursor_y = VGA_HEIGHT - 1;
}

void vga_putchar(char character, uint8_t color) {
    if (character == '\n') {
        vga_cursor_x = 0;
        vga_cursor_y++;
    } else if (character == '\r') {
        vga_cursor_x = 0;
    } else if (character == '\b') {
        if (vga_cursor_x > 0) {
            vga_cursor_x--;
            vga_buffer[vga_cursor_y * VGA_WIDTH + vga_cursor_x] = (color << 8) | ' ';
        }
    } else {
        vga_buffer[vga_cursor_y * VGA_WIDTH + vga_cursor_x] = (color << 8) | character;
        vga_cursor_x++;
    }

    if (vga_cursor_x >= VGA_WIDTH) {
        vga_cursor_x = 0;
        vga_cursor_y++;
    }
    if (vga_cursor_y >= VGA_HEIGHT) {
        vga_scroll();
    }
}

void kernel_print(const char* message, uint8_t color) {
    while(*message) {
        vga_putchar(*message++, color);
    }
}

void kernel_print_hex(uint32_t number, uint8_t color) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0'; buffer[1] = 'x'; buffer[10] = '\0';
    for (int i = 7; i >= 0; i--) {
        buffer[i + 2] = hex_chars[(number >> (i * 4)) & 0xF];
    }
    kernel_print(buffer, color);
}

void kernel_print_dec(uint32_t number, uint8_t color) {
    if (number == 0) {
        vga_putchar('0', color);
        return;
    }
    char buffer[11];
    int index = 0;
    while (number > 0) {
        buffer[index++] = '0' + (number % 10);
        number /= 10;
    }
    while (index > 0) {
        vga_putchar(buffer[--index], color);
    }
}
