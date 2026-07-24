#include "zeonos.h"

struct gdt_entry_struct gdt_entries[5];
struct gdt_ptr_struct gdt_ptr;
struct idt_entry_struct idt_entries[256];
struct idt_ptr_struct idt_ptr;

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void init_gdt(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    kernel_print("GDT: Inisialisasi Berhasil\\n", COLOR_LIGHT_BLUE);
}

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt_entries;
    memory_set(&idt_entries, 0, sizeof(struct idt_entry_struct) * 256);
    kernel_print("IDT: Inisialisasi Berhasil\\n", COLOR_LIGHT_BLUE);
}

void pic_remap(int offset1, int offset2) {
    outb(PIC1_COMMAND, 0x11); outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, offset1); outb(PIC2_DATA, offset2);
    outb(PIC1_DATA, 4); outb(PIC2_DATA, 2);
    outb(PIC1_DATA, 0x01); outb(PIC2_DATA, 0x01);
    kernel_print("PIC: Remapped Berhasil\\n", COLOR_LIGHT_BLUE);
}
