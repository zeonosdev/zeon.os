// ============================================
// ZEONOS
// Target Architecture: x86 (IA-32) Bare-Metal
// ============================================

#include <stddef.h>
#include <stdint.h>

// ========== 1. BASIC DEFINITIONS & MACROS ==========

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
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15
};

// ========== 2. STRUCT DEFINITIONS ==========

struct gdt_entry_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry_struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct Task {
    int id;
    int state;
    uint32_t* stack_top;
    uint32_t stack[STACK_SIZE];
    void (*entry_function)(void);
    char task_name[32];
};

struct FileEntry {
    char filename[MAX_FILENAME_LEN];
    uint32_t start_sector;
    uint32_t file_size;
    uint8_t is_used;
};

struct FileSystem {
    uint32_t next_free_sector;
    char volume_label[32];
    uint8_t is_mounted;
    struct FileEntry files[MAX_FILES];
};

// ========== 3. FORWARD DECLARATIONS ==========

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
void kernel_free(void* ptr);

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void init_gdt(void);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_idt(void);
void isr_handler(uint32_t int_no, uint32_t err_code);

void pic_send_eoi(unsigned char irq);
void pic_remap(int offset1, int offset2);

void timer_callback(void);
void init_timer(uint32_t frequency);

void keyboard_callback(void);
void init_keyboard(void);

uint32_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void check_pci_bus(void);

void ata_wait_busy(void);
void ata_wait_ready(void);
void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, const uint8_t* buffer);
void init_ata(void);

void task_idle(void);
void task_print_a(void);
void task_print_b(void);
void create_new_task(void (*entry_point)(void), const char* name);
void scheduler_initialize(void);
void scheduler_switch_task(void);

void filesystem_initialize(void);
int filesystem_create_file(const char* filename, uint32_t size);
void filesystem_list_files(void);

void init_paging(void);
void hardware_initialize(void);
void shell_parse_command(const char* input);
void shell_start(void);

// ========== 4. GLOBAL VARIABLES ==========

volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_MEMORY;
int vga_cursor_x = 0;
int vga_cursor_y = 0;

uint32_t heap_pointer = KERNEL_HEAP_START;

struct gdt_entry_struct gdt_entries[5];
struct gdt_ptr_struct   gdt_ptr;

struct idt_entry_struct idt_entries[256];
struct idt_ptr_struct   idt_ptr;

struct Task task_array[MAX_TASKS];
int current_task_index = -1;
int total_task_count = 0;

struct FileSystem main_filesystem;
uint32_t system_ticks = 0;

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',  0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// ========== 5. LOW-LEVEL PORT I/O ==========

void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// ========== 6. UTILITY FUNCTIONS ==========

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
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

int string_compare(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

// ========== 7. VGA TEXT MODE DRIVER ==========

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
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = '\0';
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

// ========== 8. KERNEL HEAP MEMORY MANAGER ==========

void* kernel_malloc(uint32_t size) {
    if(heap_pointer + size > KERNEL_HEAP_START + KERNEL_HEAP_SIZE) {
        return 0;
    }
    uint32_t* pointer = (uint32_t*)heap_pointer;
    heap_pointer += size;
    return (void*)pointer;
}

void kernel_free(void* ptr) {
    (void)ptr;
}

// ========== 9. GLOBAL DESCRIPTOR TABLE (GDT) ==========

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data

    asm volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "ljmp $0x08, $1f\n\t"
        "1:\n\t"
        : : "m"(gdt_ptr) : "memory"
    );
    kernel_print("GDT: Initialized Successfully\n", COLOR_GREEN);
}

// ========== 10. INTERRUPT DESCRIPTOR TABLE (IDT) ==========

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags | 0x60;
}

void isr_handler(uint32_t int_no, uint32_t err_code) {
    (void)err_code;
    kernel_print("Unhandled Interrupt Exception: ", COLOR_RED);
    kernel_print_dec(int_no, COLOR_RED);
    kernel_print("\n", COLOR_RED);
}

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memory_set(&idt_entries, 0, sizeof(struct idt_entry_struct) * 256);

    asm volatile("lidt %0" : : "m"(idt_ptr));
    kernel_print("IDT: Initialized Successfully\n", COLOR_GREEN);
}

// ========== 11. PROGRAMMABLE INTERRUPT CONTROLLER (PIC) ==========

void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(int offset1, int offset2) {
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    kernel_print("PIC: Remapped Successfully (IRQs 0x20-0x2F)\n", COLOR_GREEN);
}

// ========== 12. PROGRAMMABLE INTERVAL TIMER (PIT) ==========

void timer_callback(void) {
    system_ticks++;
    pic_send_eoi(0);
}

void init_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
    kernel_print("PIT: Timer Initialized at 100Hz\n", COLOR_GREEN);
}

// ========== 13. PS/2 KEYBOARD DRIVER ==========

void keyboard_callback(void) {
    uint8_t status = inb(0x64);
    if (status & 0x01) {
        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                vga_putchar(c, COLOR_WHITE);
            }
        }
    }
    pic_send_eoi(1);
}

void init_keyboard(void) {
    kernel_print("Keyboard: PS/2 Driver Initialized\n", COLOR_GREEN);
}

// ========== 14. PCI BUS ENUMERATOR ==========

uint32_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outb(0xCF8, address);
    return (uint32_t)((inb(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

void check_pci_bus(void) {
    kernel_print("PCI: Scanning Bus...\n", COLOR_LIGHT_BLUE);
    int devices_found = 0;
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
            if (vendor != 0xFFFF) {
                devices_found++;
                uint16_t device = pci_config_read_word(bus, slot, 0, 2);
                kernel_print("PCI Device Found - Vendor: ", COLOR_CYAN);
                kernel_print_hex(vendor, COLOR_CYAN);
                kernel_print(" Device: ", COLOR_CYAN);
                kernel_print_hex(device, COLOR_CYAN);
                kernel_print("\n", COLOR_CYAN);
            }
        }
    }
    if(devices_found == 0) {
        kernel_print("PCI: No devices found.\n", COLOR_LIGHT_RED);
    }
}

// ========== 15. ATA PIO HARD DISK DRIVER ==========

void ata_wait_busy(void) {
    while (inb(ATA_STATUS_PORT) & 0x80);
}

void ata_wait_ready(void) {
    while (!(inb(ATA_STATUS_PORT) & 0x40));
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait_busy();

    outb(ATA_DRIVE_PORT, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_PORT, 1);
    outb(ATA_LBA_LOW_PORT, (uint8_t) lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_PORT, 0x20);

    ata_wait_busy();
    ata_wait_ready();

    uint16_t* ptr = (uint16_t*) buffer;
    for (int i = 0; i < 256; i++) {
        uint16_t word;
        asm volatile("inw %1, %0" : "=a"(word) : "Nd"(ATA_DATA_PORT));
        ptr[i] = word;
    }
}

void ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    ata_wait_busy();

    outb(ATA_DRIVE_PORT, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_PORT, 1);
    outb(ATA_LBA_LOW_PORT, (uint8_t) lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_PORT, 0x30);

    ata_wait_busy();
    ata_wait_ready();

    const uint16_t* ptr = (const uint16_t*) buffer;
    for (int i = 0; i < 256; i++) {
        uint16_t word = ptr[i];
        asm volatile("outw %0, %1" : : "a"(word), "Nd"(ATA_DATA_PORT));
    }
}

void init_ata(void) {
    kernel_print("ATA: Primary Bus Initialized.\n", COLOR_GREEN);
}

// ========== 16. TASK MANAGER & SCHEDULER ==========

void create_new_task(void (*entry_point)(void), const char* name) {
    if (total_task_count >= MAX_TASKS) return;

    struct Task* new_task = &task_array[total_task_count];
    new_task->id = total_task_count;
    new_task->state = 1;
    new_task->entry_function = entry_point;

    int i = 0;
    while(name[i] != '\0' && i < 31) {
        new_task->task_name[i] = name[i];
        i++;
    }
    new_task->task_name[i] = '\0';

    new_task->stack_top = &new_task->stack[STACK_SIZE - 1];

    total_task_count++;
}

void task_idle(void) {
    kernel_print("TASK: Idle task active.\n", COLOR_DARK_GREY);
    while (1) {
        asm volatile("hlt");
    }
}

void task_print_a(void) {
    kernel_print("TASK A Executed.\n", COLOR_LIGHT_BLUE);
    while(1) {
        asm volatile("nop");
    }
}

void task_print_b(void) {
    kernel_print("TASK B Executed.\n", COLOR_LIGHT_MAGENTA);
    while(1) {
        asm volatile("nop");
    }
}

void scheduler_initialize(void) {
    create_new_task(task_idle, "idle_task");
    create_new_task(task_print_a, "task_a");
    create_new_task(task_print_b, "task_b");
    current_task_index = 0;
    kernel_print("Scheduler: Initialized Successfully\n", COLOR_GREEN);
}

void scheduler_switch_task(void) {
    if (total_task_count == 0) return;

    current_task_index++;
    if (current_task_index >= total_task_count) {
        current_task_index = 0;
    }
}

// ========== 17. VIRTUAL FILE SYSTEM (VFS) ==========

void filesystem_initialize(void) {
    memory_set(&main_filesystem, 0, sizeof(main_filesystem));
    main_filesystem.next_free_sector = 100;
    main_filesystem.is_mounted = 1;

    char label[] = "ZEONOS_ROOT";
    for(int i = 0; label[i] != '\0'; i++) {
        main_filesystem.volume_label[i] = label[i];
    }

    kernel_print("Filesystem: Initialized Successfully\n", COLOR_GREEN);
}

int filesystem_create_file(const char* filename, uint32_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!main_filesystem.files[i].is_used) {
            main_filesystem.files[i].is_used = 1;
            main_filesystem.files[i].start_sector = main_filesystem.next_free_sector;
            main_filesystem.files[i].file_size = size;

            int len = 0;
            while(filename[len] != '\0' && len < MAX_FILENAME_LEN - 1) {
                main_filesystem.files[i].filename[len] = filename[len];
                len++;
            }
            main_filesystem.files[i].filename[len] = '\0';

            uint32_t sectors_needed = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
            main_filesystem.next_free_sector += sectors_needed;
            return i;
        }
    }
    return -1;
}

void filesystem_list_files(void) {
    kernel_print("--- File System Directory ---\n", COLOR_LIGHT_CYAN);
    for (int i = 0; i < MAX_FILES; i++) {
        if (main_filesystem.files[i].is_used) {
            kernel_print("File: ", COLOR_WHITE);
            kernel_print(main_filesystem.files[i].filename, COLOR_WHITE);
            kernel_print(" | Size: ", COLOR_WHITE);
            kernel_print_dec(main_filesystem.files[i].file_size, COLOR_WHITE);
            kernel_print(" bytes | Sector: ", COLOR_WHITE);
            kernel_print_dec(main_filesystem.files[i].start_sector, COLOR_WHITE);
            kernel_print("\n", COLOR_WHITE);
        }
    }
}

// ========== 18. MEMORY MANAGEMENT UNIT (MMU / PAGING) ==========

void init_paging(void) {
    for(int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }

    for(int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3;
    }

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

// ========== 19. HARDWARE SUBSYSTEM INITIALIZATION ==========

void hardware_initialize(void) {
    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    init_timer(100);
    init_keyboard();
    init_ata();
    check_pci_bus();
    init_paging();

    asm volatile("sti");
    kernel_print("HW: Hardware Subsystem Ready.\n", COLOR_WHITE);
}

// ========== 20. KERNEL SHELL INTERFACE ==========

void shell_parse_command(const char* input) {
    if (string_compare(input, "help") == 0) {
        kernel_print("ZEONOS Commands: help, clear, sysinfo, ls, create\n", COLOR_LIGHT_CYAN);
    } else if (string_compare(input, "clear") == 0) {
        vga_clear_screen();
    } else if (string_compare(input, "sysinfo") == 0) {
        kernel_print("ZEONOS Kernel v0.3.0 Architecture: x86\n", COLOR_LIGHT_GREEN);
        kernel_print("Heap Start: ", COLOR_LIGHT_GREEN);
        kernel_print_hex(KERNEL_HEAP_START, COLOR_LIGHT_GREEN);
        kernel_print("\n", COLOR_LIGHT_GREEN);
    } else if (string_compare(input, "ls") == 0) {
        filesystem_list_files();
    } else {
        kernel_print("Unknown Command: ", COLOR_LIGHT_RED);
        kernel_print(input, COLOR_LIGHT_RED);
        kernel_print("\n", COLOR_LIGHT_RED);
    }
}

void shell_start(void) {
    kernel_print("ZEONOS Shell v0.1 Ready.\n", COLOR_LIGHT_BROWN);
    kernel_print("Type 'help' for available commands.\n", COLOR_WHITE);
    kernel_print("zeonos# ", COLOR_LIGHT_GREEN);
}

// ========== 21. KERNEL MAIN ENTRY POINT ==========

void kernel_main(void) {
    vga_clear_screen();
    kernel_print("========================================\n", COLOR_CYAN);
    kernel_print("        ZEONOS v0.3.0 BOOTING          \n", COLOR_WHITE);
    kernel_print("========================================\n", COLOR_CYAN);

    hardware_initialize();
    scheduler_initialize();
    filesystem_initialize();

    filesystem_create_file("system.log", 1024);
    filesystem_create_file("kernel.bin", 65536);

    shell_start();

    while(1) {
        scheduler_switch_task();
        asm volatile("hlt");
    }
}
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15
};

// ========== 2. STRUCT DEFINITIONS ==========

struct gdt_entry_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry_struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct Task {
    int id;
    int state;
    uint32_t* stack_top;
    uint32_t stack[STACK_SIZE];
    void (*entry_function)(void);
    char task_name[32];
};

struct FileEntry {
    char filename[MAX_FILENAME_LEN];
    uint32_t start_sector;
    uint32_t file_size;
    uint8_t is_used;
};

struct FileSystem {
    uint32_t next_free_sector;
    char volume_label[32];
    uint8_t is_mounted;
    struct FileEntry files[MAX_FILES];
};

// ========== 3. FORWARD DECLARATIONS ==========

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
void kernel_free(void* ptr);

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void init_gdt(void);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_idt(void);
void isr_handler(uint32_t int_no, uint32_t err_code);

void pic_send_eoi(unsigned char irq);
void pic_remap(int offset1, int offset2);

void timer_callback(void);
void init_timer(uint32_t frequency);

void keyboard_callback(void);
void init_keyboard(void);

uint32_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void check_pci_bus(void);

void ata_wait_busy(void);
void ata_wait_ready(void);
void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, const uint8_t* buffer);
void init_ata(void);

void task_idle(void);
void task_print_a(void);
void task_print_b(void);
void create_new_task(void (*entry_point)(void), const char* name);
void scheduler_initialize(void);
void scheduler_switch_task(void);

void filesystem_initialize(void);
int filesystem_create_file(const char* filename, uint32_t size);
void filesystem_list_files(void);

void init_paging(void);
void hardware_initialize(void);
void shell_parse_command(const char* input);
void shell_start(void);

// ========== 4. GLOBAL VARIABLES ==========

volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_MEMORY;
int vga_cursor_x = 0;
int vga_cursor_y = 0;

uint32_t heap_pointer = KERNEL_HEAP_START;

struct gdt_entry_struct gdt_entries[5];
struct gdt_ptr_struct   gdt_ptr;

struct idt_entry_struct idt_entries[256];
struct idt_ptr_struct   idt_ptr;

struct Task task_array[MAX_TASKS];
int current_task_index = -1;
int total_task_count = 0;

struct FileSystem main_filesystem;
uint32_t system_ticks = 0;

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',  0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// ========== 5. LOW-LEVEL PORT I/O ==========

void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// ========== 6. UTILITY FUNCTIONS ==========

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
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

int string_compare(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

// ========== 7. VGA TEXT MODE DRIVER ==========

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
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = '\0';
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

// ========== 8. KERNEL HEAP MEMORY MANAGER ==========

void* kernel_malloc(uint32_t size) {
    if(heap_pointer + size > KERNEL_HEAP_START + KERNEL_HEAP_SIZE) {
        return 0;
    }
    uint32_t* pointer = (uint32_t*)heap_pointer;
    heap_pointer += size;
    return (void*)pointer;
}

void kernel_free(void* ptr) {
    (void)ptr;
}

// ========== 9. GLOBAL DESCRIPTOR TABLE (GDT) ==========

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data

    asm volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "ljmp $0x08, $1f\n\t"
        "1:\n\t"
        : : "m"(gdt_ptr) : "memory"
    );
    kernel_print("GDT: Initialized Successfully\n", COLOR_GREEN);
}

// ========== 10. INTERRUPT DESCRIPTOR TABLE (IDT) ==========

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags | 0x60;
}

void isr_handler(uint32_t int_no, uint32_t err_code) {
    (void)err_code;
    kernel_print("Unhandled Interrupt Exception: ", COLOR_RED);
    kernel_print_dec(int_no, COLOR_RED);
    kernel_print("\n", COLOR_RED);
}

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memory_set(&idt_entries, 0, sizeof(struct idt_entry_struct) * 256);

    asm volatile("lidt %0" : : "m"(idt_ptr));
    kernel_print("IDT: Initialized Successfully\n", COLOR_GREEN);
}

// ========== 11. PROGRAMMABLE INTERRUPT CONTROLLER (PIC) ==========

void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(int offset1, int offset2) {
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    kernel_print("PIC: Remapped Successfully (IRQs 0x20-0x2F)\n", COLOR_GREEN);
}

// ========== 12. PROGRAMMABLE INTERVAL TIMER (PIT) ==========

void timer_callback(void) {
    system_ticks++;
    pic_send_eoi(0);
}

void init_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
    kernel_print("PIT: Timer Initialized at 100Hz\n", COLOR_GREEN);
}

// ========== 13. PS/2 KEYBOARD DRIVER ==========

void keyboard_callback(void) {
    uint8_t status = inb(0x64);
    if (status & 0x01) {
        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                vga_putchar(c, COLOR_WHITE);
            }
        }
    }
    pic_send_eoi(1);
}

void init_keyboard(void) {
    kernel_print("Keyboard: PS/2 Driver Initialized\n", COLOR_GREEN);
}

// ========== 14. PCI BUS ENUMERATOR ==========

uint32_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outb(0xCF8, address);
    return (uint32_t)((inb(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

void check_pci_bus(void) {
    kernel_print("PCI: Scanning Bus...\n", COLOR_LIGHT_BLUE);
    int devices_found = 0;
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
            if (vendor != 0xFFFF) {
                devices_found++;
                uint16_t device = pci_config_read_word(bus, slot, 0, 2);
                kernel_print("PCI Device Found - Vendor: ", COLOR_CYAN);
                kernel_print_hex(vendor, COLOR_CYAN);
                kernel_print(" Device: ", COLOR_CYAN);
                kernel_print_hex(device, COLOR_CYAN);
                kernel_print("\n", COLOR_CYAN);
            }
        }
    }
    if(devices_found == 0) {
        kernel_print("PCI: No devices found.\n", COLOR_LIGHT_RED);
    }
}

// ========== 15. ATA PIO HARD DISK DRIVER ==========

void ata_wait_busy(void) {
    while (inb(ATA_STATUS_PORT) & 0x80);
}

void ata_wait_ready(void) {
    while (!(inb(ATA_STATUS_PORT) & 0x40));
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait_busy();

    outb(ATA_DRIVE_PORT, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_PORT, 1);
    outb(ATA_LBA_LOW_PORT, (uint8_t) lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_PORT, 0x20);

    ata_wait_busy();
    ata_wait_ready();

    uint16_t* ptr = (uint16_t*) buffer;
    for (int i = 0; i < 256; i++) {
        uint16_t word;
        asm volatile("inw %1, %0" : "=a"(word) : "Nd"(ATA_DATA_PORT));
        ptr[i] = word;
    }
}

void ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    ata_wait_busy();

    outb(ATA_DRIVE_PORT, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_PORT, 1);
    outb(ATA_LBA_LOW_PORT, (uint8_t) lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_PORT, 0x30);

    ata_wait_busy();
    ata_wait_ready();

    const uint16_t* ptr = (const uint16_t*) buffer;
    for (int i = 0; i < 256; i++) {
        uint16_t word = ptr[i];
        asm volatile("outw %0, %1" : : "a"(word), "Nd"(ATA_DATA_PORT));
    }
}

void init_ata(void) {
    kernel_print("ATA: Primary Bus Initialized.\n", COLOR_GREEN);
}

// ========== 16. TASK MANAGER & SCHEDULER ==========

void create_new_task(void (*entry_point)(void), const char* name) {
    if (total_task_count >= MAX_TASKS) return;

    struct Task* new_task = &task_array[total_task_count];
    new_task->id = total_task_count;
    new_task->state = 1;
    new_task->entry_function = entry_point;

    int i = 0;
    while(name[i] != '\0' && i < 31) {
        new_task->task_name[i] = name[i];
        i++;
    }
    new_task->task_name[i] = '\0';

    new_task->stack_top = &new_task->stack[STACK_SIZE - 1];

    total_task_count++;
}

void task_idle(void) {
    kernel_print("TASK: Idle task active.\n", COLOR_DARK_GREY);
    while (1) {
        asm volatile("hlt");
    }
}

void task_print_a(void) {
    kernel_print("TASK A Executed.\n", COLOR_LIGHT_BLUE);
    while(1) {
        asm volatile("nop");
    }
}

void task_print_b(void) {
    kernel_print("TASK B Executed.\n", COLOR_LIGHT_MAGENTA);
    while(1) {
        asm volatile("nop");
    }
}

void scheduler_initialize(void) {
    create_new_task(task_idle, "idle_task");
    create_new_task(task_print_a, "task_a");
    create_new_task(task_print_b, "task_b");
    current_task_index = 0;
    kernel_print("Scheduler: Initialized Successfully\n", COLOR_GREEN);
}

void scheduler_switch_task(void) {
    if (total_task_count == 0) return;

    current_task_index++;
    if (current_task_index >= total_task_count) {
        current_task_index = 0;
    }
}

// ========== 17. VIRTUAL FILE SYSTEM (VFS) ==========

void filesystem_initialize(void) {
    memory_set(&main_filesystem, 0, sizeof(main_filesystem));
    main_filesystem.next_free_sector = 100;
    main_filesystem.is_mounted = 1;

    char label[] = "ZEONOS_ROOT";
    for(int i = 0; label[i] != '\0'; i++) {
        main_filesystem.volume_label[i] = label[i];
    }

    kernel_print("Filesystem: Initialized Successfully\n", COLOR_GREEN);
}

int filesystem_create_file(const char* filename, uint32_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!main_filesystem.files[i].is_used) {
            main_filesystem.files[i].is_used = 1;
            main_filesystem.files[i].start_sector = main_filesystem.next_free_sector;
            main_filesystem.files[i].file_size = size;

            int len = 0;
            while(filename[len] != '\0' && len < MAX_FILENAME_LEN - 1) {
                main_filesystem.files[i].filename[len] = filename[len];
                len++;
            }
            main_filesystem.files[i].filename[len] = '\0';

            uint32_t sectors_needed = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
            main_filesystem.next_free_sector += sectors_needed;
            return i;
        }
    }
    return -1;
}

void filesystem_list_files(void) {
    kernel_print("--- File System Directory ---\n", COLOR_LIGHT_CYAN);
    for (int i = 0; i < MAX_FILES; i++) {
        if (main_filesystem.files[i].is_used) {
            kernel_print("File: ", COLOR_WHITE);
            kernel_print(main_filesystem.files[i].filename, COLOR_WHITE);
            kernel_print(" | Size: ", COLOR_WHITE);
            kernel_print_dec(main_filesystem.files[i].file_size, COLOR_WHITE);
            kernel_print(" bytes | Sector: ", COLOR_WHITE);
            kernel_print_dec(main_filesystem.files[i].start_sector, COLOR_WHITE);
            kernel_print("\n", COLOR_WHITE);
        }
    }
}

// ========== 18. MEMORY MANAGEMENT UNIT (MMU / PAGING) ==========

void init_paging(void) {
    for(int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }

    for(int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3;
    }

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

// ========== 19. HARDWARE SUBSYSTEM INITIALIZATION ==========

void hardware_initialize(void) {
    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    init_timer(100);
    init_keyboard();
    init_ata();
    check_pci_bus();
    init_paging();

    asm volatile("sti");
    kernel_print("HW: Hardware Subsystem Ready.\n", COLOR_WHITE);
}

// ========== 20. KERNEL SHELL INTERFACE ==========

void shell_parse_command(const char* input) {
    if (string_compare(input, "help") == 0) {
        kernel_print("ZEONOS Commands: help, clear, sysinfo, ls, create\n", COLOR_LIGHT_CYAN);
    } else if (string_compare(input, "clear") == 0) {
        vga_clear_screen();
    } else if (string_compare(input, "sysinfo") == 0) {
        kernel_print("ZEONOS Kernel v0.3.0 Architecture: x86\n", COLOR_LIGHT_GREEN);
        kernel_print("Heap Start: ", COLOR_LIGHT_GREEN);
        kernel_print_hex(KERNEL_HEAP_START, COLOR_LIGHT_GREEN);
        kernel_print("\n", COLOR_LIGHT_GREEN);
    } else if (string_compare(input, "ls") == 0) {
        filesystem_list_files();
    } else {
        kernel_print("Unknown Command: ", COLOR_LIGHT_RED);
        kernel_print(input, COLOR_LIGHT_RED);
        kernel_print("\n", COLOR_LIGHT_RED);
    }
}

void shell_start(void) {
    kernel_print("ZEONOS Shell v0.1 Ready.\n", COLOR_LIGHT_BROWN);
    kernel_print("Type 'help' for available commands.\n", COLOR_WHITE);
    kernel_print("zeonos# ", COLOR_LIGHT_GREEN);
}

// ========== 21. KERNEL MAIN ENTRY POINT ==========

void kernel_main(void) {
    vga_clear_screen();
    kernel_print("========================================\n", COLOR_CYAN);
    kernel_print("        ZEONOS v0.3.0 BOOTING          \n", COLOR_WHITE);
    kernel_print("========================================\n", COLOR_CYAN);

    hardware_initialize();
    scheduler_initialize();
    filesystem_initialize();

    filesystem_create_file("system.log", 1024);
    filesystem_create_file("kernel.bin", 65536);

    shell_start();

    while(1) {
        scheduler_switch_task();
        asm volatile("hlt");
    }
}
