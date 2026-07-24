#ifndef ZEONOS_H
#define ZEONOS_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define MAX_TASKS 16
#define STACK_SIZE 16384
#define KERNEL_HEAP_START 0x100000
#define KERNEL_HEAP_SIZE 0x1000000

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ATA_DATA_PORT 0x1F0
#define ATA_ERROR_PORT 0x1F1
#define ATA_SECTOR_COUNT_PORT 0x1F2
#define ATA_LBA_LOW_PORT 0x1F3
#define ATA_LBA_MID_PORT 0x1F4
#define ATA_LBA_HIGH_PORT 0x1F5
#define ATA_DRIVE_PORT 0x1F6
#define ATA_COMMAND_PORT 0x1F7
#define ATA_STATUS_PORT 0x1F7

#define MAX_FILES 32
#define MAX_FILENAME_LEN 32
#define SECTOR_SIZE 512

enum vga_color {
    COLOR_BLACK = 0, COLOR_BLUE = 1, COLOR_GREEN = 2, COLOR_CYAN = 3,
    COLOR_RED = 4, COLOR_MAGENTA = 5, COLOR_BROWN = 6, COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8, COLOR_LIGHT_BLUE = 9, COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11, COLOR_LIGHT_RED = 12, COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14, COLOR_WHITE = 15
};

struct gdt_entry_struct {
    uint16_t limit_low; uint16_t base_low; uint8_t base_middle;
    uint8_t access; uint8_t granularity; uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr_struct {
    uint16_t limit; uint32_t base;
} __attribute__((packed));

struct idt_entry_struct {
    uint16_t base_lo; uint16_t sel; uint8_t always0;
    uint8_t flags; uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr_struct {
    uint16_t limit; uint32_t base;
} __attribute__((packed));

struct Task {
    int id; int state; uint32_t* stack_top;
    uint32_t stack[STACK_SIZE];
    void (*entry_function)(void);
    char task_name[32];
};

struct FileEntry {
    char filename[MAX_FILENAME_LEN];
    uint32_t start_sector; uint32_t file_size; uint8_t is_used;
};

struct FileSystem {
    uint32_t next_free_sector;
    char volume_label[32];
    uint8_t is_mounted;
    struct FileEntry files[MAX_FILES];
};

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void memory_set(void* destination, uint8_t value, uint32_t count);
void memory_copy(void* destination, const void* source, uint32_t count);
uint32_t string_length(const char* string);
int string_compare(const char* str1, const char* str2);

void vga_clear_screen(void);
void vga_scroll(void);
void vga_putchar(char character, uint8_t color);
void kernel_print(const char* message, uint8_t color);
void kernel_print_hex(uint32_t number, uint8_t color);
void kernel_print_dec(uint32_t number, uint8_t color);

void* kernel_malloc(uint32_t size);
void init_paging(void);

void init_gdt(void);
void init_idt(void);
void pic_remap(int offset1, int offset2);
void pic_send_eoi(unsigned char irq);
void init_timer(uint32_t frequency);
void init_keyboard(void);
void check_pci_bus(void);
void init_ata(void);

void scheduler_initialize(void);
void scheduler_switch_task(void);
void filesystem_initialize(void);
int filesystem_create_file(const char* filename, uint32_t size);
void filesystem_list_files(void);
void shell_start(void);

/* Auth Subsystem Prototypes */
void auth_init(void);
int auth_register_user(const char* username, const char* password);
int auth_login_user(const char* username, const char* password);
void show_register_screen(void);
void show_login_screen(void);


#endif
