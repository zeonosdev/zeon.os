#include "zeonos.h"

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

void vga_putchar(char character, uint8_t color) {
    if (character == '\n') {
        vga_cursor_x = 0;
        vga_cursor_y++;
    } else {
        vga_buffer[vga_cursor_y * VGA_WIDTH + vga_cursor_x] = (color << 8) | character;
        vga_cursor_x++;
    }

    if (vga_cursor_x >= VGA_WIDTH) {
        vga_cursor_x = 0;
        vga_cursor_y++;
    }
}

void kernel_print(const char* message, uint8_t color) {
    while(*message) {
        vga_putchar(*message++, color);
    }
}
