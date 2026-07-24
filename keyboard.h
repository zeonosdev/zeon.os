// keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60

void keyboard_init();
char keyboard_getchar();
void keyboard_handler();

#endif
