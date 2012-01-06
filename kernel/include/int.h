#ifndef INT_H
#define INT_H

#include <thread.h>
#include <stdint.h>

#define NUM_INTERRUPTS	256

#define ISIRQ(num) ((num >= 32) && ( num <= 47))
#define INT2IRQ(num) (num - 32)
#define IRQ2INT(num) (num + 32)

#define NUM_IRQS 16

#define MPIC_CMD_PORT	0x20
#define MPIC_DATA_PORT	0x21
#define SPIC_CMD_PORT	0xA0
#define SPIC_DATA_PORT	0xA1

#define PIC_EOI		0x20

#define IDT_PRESENT	0x80
#define IDT_DPL_0	0x00
#define IDT_DPL_1	0x20
#define IDT_DPL_2	0x40
#define IDT_DPL_3	0x60
#define IDT_S		0x10
#define IDT_TASK_GATE	0x5
#define IDT_INT_GATE	0xE
#define IDT_TRAP_GATE	0xF

#define GDT_PRESENT	0x80
#define GDT_DPL_0	0x00
#define GDT_DPL_1	0x20
#define GDT_DPL_2	0x40
#define GDT_DPL_3	0x60
#define GDT_ALWAYS	0x10
#define GDT_EXECUTABLE	0x8
#define GDT_GROW_DOWN	0x4
#define GDT_PRIV_LT	0x4
#define GDT_CODE_READ	0x2
#define GDT_DATA_WRITE	0x2
#define GDT_ACCESSED	0x1

#define GDT_KERNEL_CODE	0x1
#define GDT_KERNEL_DATA	0x2
#define GDT_USER_CODE	0x3
#define GDT_USER_DATA	0x4
#define GDT_TSS		0x5
#define SEG_KERNEL_CODE	0x08
#define SEG_KERNEL_DATA	0x10
#define SEG_USER_CODE	0x18
#define SEG_USER_DATA	0x20
#define SEG_TSS		0x28

#define EFLAGS_CPL3	0x3000
#define EFLAGS_IF	0x200

typedef thread* (*int_handler) (thread*);

typedef struct idt_struct {
	uint16_t base_l;
	uint16_t seg;
	uint8_t reserved;
	uint8_t flags;
	uint16_t base_h;
}__attribute__((packed)) idt;

struct idt_ptr_st {
	uint16_t size;
	uint32_t offset;
}__attribute__((packed));

typedef struct gdt_struct
{
	uint16_t limit_l;
	uint16_t base_l;
	uint8_t base_m;
	uint8_t access;
	uint8_t fl_limit_h;
	uint8_t base_h;
}__attribute__((packed)) gdt_entry;


typedef struct tss_struct {
	uint32_t ptl;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[15];
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt;
	uint16_t trap, iomap;
}__attribute__((packed)) tss;

tss global_tss;


extern void int_return(void);
extern void
	isr0(void), isr1(void), isr2(void), isr3(void), isr4(void),
	isr5(void), isr6(void), isr7(void), isr8(void), isr9(void),
	isr10(void), isr11(void), isr12(void), isr13(void), isr14(void),
	isr15(void), isr16(void), isr17(void), isr18(void), isr19(void),
	isr20(void), isr21(void), isr22(void), isr23(void), isr24(void),
	isr25(void), isr26(void), isr27(void), isr28(void), isr29(void),
	isr30(void), isr31(void), isr32(void), isr33(void), isr34(void),
	isr35(void), isr36(void), isr37(void), isr38(void), isr39(void),
	isr40(void), isr41(void), isr42(void), isr43(void), isr44(void),
	isr45(void), isr46(void), isr47(void),
	isr128(void), isr255(void);
	
extern void flush_idt(uint32_t);
extern void flush_tss(uint32_t);

void idt_set(uint32_t num, uint32_t base, uint32_t segment, uint8_t flags);
void init_idt();
void init_tss();
thread *idt_handler (thread *t);
void install_int_handler(uint32_t num, int_handler h);

#endif