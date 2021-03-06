#include "isr.h"
#include "../process/process.h"

extern interrupt_handler g_interrupt_handlers[IDT_ENTRIES];

char *exceptions[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t* registers)
{
	int interrupt = registers->interrupt_num;
	interrupt = interrupt < 0 ? -interrupt : interrupt;
    //if there is a handler for the isr
    if(g_interrupt_handlers[interrupt] != 0)
    {
        g_interrupt_handlers[interrupt](registers);
    }
    //if no handler for isr
    else
    {
        puts("invalid isr called");
    }
}

void set_interrupt(uint8_t interrupt_num, interrupt_handler interrupt)
{
    g_interrupt_handlers[interrupt_num] = interrupt;
}

void general_handler(registers_t* registers)
{
	puts(exceptions[registers->interrupt_num]);
	putc('\n');
	kill_running_process();
}