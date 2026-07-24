// keyboard.c
#include "keyboard.h"

static const char scancode_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0,  ' '
};

static char kbd_buffer[256];
static uint8_t head = 0, tail = 0;

extern uint8_t inb(uint16_t port); // Assembly low-level reader

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (!(scancode & 0x80)) { // Keydown event
        char c = scancode_ascii[scancode];
        if (c != 0) {
            kbd_buffer[head++] = c;
        }
    }
}

char keyboard_getchar() {
    while (head == tail); // Tunggu tombol ditekan
    return kbd_buffer[tail++];
}
