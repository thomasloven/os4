#include <int.h>
#include <stdint.h>
#include <ports.h>
#include <queue.h>

idt idt_table[NUM_INTERRUPTS];
struct idt_ptr_st idt_p;

int_handler int_handlers[NUM_INTERRUPTS];

thread_queue irq_handlers[NUM_IRQS];

extern gdt_entry gdt[6];
	
void *idt_raw[] = 
{
	isr0,	isr1,	isr2,	isr3,	isr4,
	isr5,	isr6,	isr7,	isr8,	isr9,	// 9
	isr10,	isr11,	isr12,	isr13,	isr14,
	isr15,	isr16,	isr17,	isr18,	isr19,	// 19
	isr20,	isr21,	isr22,	isr23,	isr24,
	isr25,	isr26,	isr27,	isr28,	isr29,	// 29
	isr30,	isr31,	isr32,	isr33,	isr34,
	isr35,	isr36,	isr37,	isr38,	isr39,	// 39
	isr40,	isr41,	isr42,	isr43,	isr44,
	isr45,	isr46,	isr47,	0,	0,	// 49
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 59
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 69
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 79
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 89
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 99
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 109
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 119
	0,	0,	0,	0,	0,
	0,	0,	0,	isr128,	0,	// 129
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 139
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 149
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 159
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 169
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 179
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 189
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 199
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 209
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 219
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 229
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 239
	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	// 249
	0,	0,	0,	0,	0,
	isr255					// 255
};

void idt_set(uint32_t num, uint32_t base, uint32_t segment, uint8_t flags)
{
	if(!base) return;
	idt_table[num].base_l = (uint16_t)( base & 0xFFFF );
	idt_table[num].base_h = (uint16_t)( base >> 16 );
	idt_table[num].seg = segment;
	idt_table[num].reserved = 0;
	idt_table[num].flags = flags;
}

void init_idt()
{
	outb(MPIC_CMD_PORT, 0x11);
	outb(SPIC_CMD_PORT, 0x11);
	outb(MPIC_DATA_PORT, 0x20);
	outb(SPIC_DATA_PORT, 0x28);
	outb(MPIC_DATA_PORT, 0x04);
	outb(SPIC_DATA_PORT, 0x02);
	outb(MPIC_DATA_PORT, 0x01);
	outb(SPIC_DATA_PORT, 0x01);
	
	outb(MPIC_DATA_PORT, 0x0);
	outb(SPIC_DATA_PORT, 0x0);
	
	idt_p.size = (sizeof(idt)*NUM_INTERRUPTS) - 1;
	idt_p.offset = (uint32_t)&idt_table;
	
	k_memset((uint8_t *)idt_table, 0, sizeof(idt)*NUM_INTERRUPTS);
	
	uint32_t i;
	for(i = 0; i < NUM_INTERRUPTS; i++)
	{
		if(idt_raw[i])
		{
			idt_set(i, (uint32_t)idt_raw[i], SEG_KERNEL_CODE, IDT_PRESENT | IDT_INT_GATE);
			int_handlers[i] = 0;
			if(ISIRQ(i))
				queue_init(&irq_handlers[INT2IRQ(i)]);
		}
	}
	
	idt_table[128].flags |= IDT_DPL_3;
	idt_table[255].flags |= IDT_DPL_3;
	
	flush_idt((uint32_t)&idt_p);
	
}

void init_tss()
{
	gdt[GDT_TSS].base_l = ((uint32_t)&global_tss & 0xFFFF);
	gdt[GDT_TSS].base_m = (((uint32_t)&global_tss >> 16) & 0xFF);
	gdt[GDT_TSS].base_h = (((uint32_t)&global_tss >> 24) & 0xFF);
	uint32_t tss_limit = ((uint32_t)&global_tss + sizeof(tss) -1);
	gdt[GDT_TSS].limit_l = (tss_limit & 0xFFFF);
	gdt[GDT_TSS].fl_limit_h = ((tss_limit >> 16) & 0xFF);
	gdt[GDT_TSS].fl_limit_h |= 0;
	gdt[GDT_TSS].access = GDT_PRESENT | GDT_EXECUTABLE | GDT_ACCESSED;
	k_memset((uint8_t *)&global_tss, 0, sizeof(tss));
	global_tss.ss0 = SEG_KERNEL_DATA;
	global_tss.ds = global_tss.es = global_tss.fs = global_tss.gs = SEG_KERNEL_DATA;
	global_tss.cs = SEG_KERNEL_CODE;
	global_tss.iomap = sizeof(tss);
	flush_tss(SEG_TSS);
}

thread *idt_handler (thread *t)
{
	if(ISIRQ(t->r.int_no))
	{
		if(INT2IRQ(t->r.int_no) > 8)
			outb(SPIC_CMD_PORT, PIC_EOI);
		outb(MPIC_CMD_PORT, PIC_EOI);
		scheduler_wake(&irq_handlers[INT2IRQ(t->r.int_no)]);
	}
	
	if(int_handlers[t->r.int_no])
	{
		t = int_handlers[t->r.int_no](t);
		return t;
	} else {
		if(ISIRQ(t->r.int_no))
			return t;
			
		k_printf("\nRecieved unhandled interrupt %d:%d", t->r.int_no, t->r.err_code);
		k_printf("\n eip:%x",t->r.eip);
		for(;;);
	}
}

void install_int_handler(uint32_t num, int_handler h)
{
	int_handlers[num] = h;
}

void add_irq_handler(uint32_t num, thread *th)
{
	queue_insert(&irq_handlers[num], th);
}
