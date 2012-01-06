#include <timer.h>
#include <stdint.h>
#include <int.h>
#include <scheduler.h>
#include <thread.h>
#include <k_debug.h>

uint32_t ticks;

void init_timer(uint32_t freq)
{
	ticks = 0;
	install_int_handler(IRQ2INT(PIT_IRQ), timer_handler);
	
	outb(PIT_CMD_PORT, PIT_USE_CH0 | PIT_ACCESS_LOHI | PIT_MODE_3);
	
	uint32_t divisor = PIT_DIVISOR_CONST / freq;
	outb(PIT_CH0_DATA_PORT, (divisor & 0xFF));
	outb(PIT_CH0_DATA_PORT, ((divisor >> 8) & 0xFF));
}

thread *timer_handler(thread *th)
{
	ticks ++;
	thread *t;
	if(ticks %10 == 0)
	{
		//if(th->flags != THREAD_NOINT)
		{
		t = switch_thread(th, scheduler_next());
		}
	}
	
	else
		t = th;
//	if(t != th)
//		k_printf("Timer %x",t->tid);
	return t;
}