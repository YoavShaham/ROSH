#ifndef TSS_H
#define TSS_H
#include <stdint.h>
#include "gdt.h"

typedef struct tss_entry {
	uint32_t prev_tss;			// previous TSS
	uint32_t esp0;				// stack pointer for kernel mode.
	uint32_t ss0;				// stack segment for kernel mode.
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry;

/*
	init tss entry in the GDT
	@param entry: empty tss entry to fill
*/
void init_tss(gdt_entry_bits* entry);

/*
	set tss kernel stack
*/
void set_kernel_stack(uint32_t stack);

#endif