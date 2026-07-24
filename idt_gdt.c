#include "zeonos_h"

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

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt_entries;
    memory_set(&idt_entries, 0, sizeof(struct idt_entry_struct) * 256);
    asm volatile("lidt %0" : : "m"(idt_ptr));
    kernel_print("IDT: Initialized Successfully\n", COLOR_GREEN);
}

void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(int offset1, int offset2) {
    unsigned char a1 = inb(PIC1_DATA);
    unsigned char a2 = inb(PIC2_DATA);
    outb(PIC1_COMMAND, 0x11); outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, offset1); outb(PIC2_DATA, offset2);
    outb(PIC1_DATA, 4); outb(PIC2_DATA, 2);
    outb(PIC1_DATA, 0x01); outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, a1); outb(PIC2_DATA, a2);
    kernel_print("PIC: Remapped Successfully\n", COLOR_GREEN);
}
