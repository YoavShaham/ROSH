#include "../../libc/screen.h"
#include "../../libc/system.h"
#include <stdint.h>
#include "../GDT/gdt.h"
#include "../IDT/idt.h"
#include "../memory/paging.h"
#include "../memory/heap.h"
#include "../process/process.h"
#include "../../fs/fs.h"
#include "../networking/drivers/ethernet_driver.h"
#include "../networking/protocols/dhcp.h"
#include "../networking/protocols/udp.h"

/*
	print ROSH logo
*/
void printLogo();
void kernelShutdown();

extern void usermode(void);

void main() {
	gdt_initialize();		// initializing gdt
	
	idt_initialize();		// initializing idt
	initKernelHeap();		// init kernel heap
	initialize_paging();	// init paging
	keyboard_initialize();	// initializing keyboard

	initConsole();			// init cursor
	init_fs();				// init file system

	printLogo();		// print ROSH

	getchar();
	clearConsole();

	initialize_ethernet_driver();
	dhcp_discover();
	
	process_init();		// also jumps to userspace
}

void printLogo()
{
	char logo[] = "Kernel Loaded.\nGDT initialized.\nIDT initialized.\nHeap initialized.\nPaging initialized.\n\n"
				"\t\t\t\t\t\t\t______ _____ _____ _   _ \n"
				"\t\t\t\t\t\t\t| ___ \\  _  /  ___| | | |\n"
				"\t\t\t\t\t\t\t| |_/ / | | \\ `--.| |_| |\n"
				"\t\t\t\t\t\t\t|    /| | | |`--. \\  _  |\n"
				"\t\t\t\t\t\t\t| |\\ \\\\ \\_/ /\\__/ / | | |\n"
				"\t\t\t\t\t\t\t\\_| \\_|\\___/\\____/\\_| |_/\n"
				"\t\t\t\t  Roei Aviad and Yoav Shaham's Operating System!\n\nPress Enter to start!\n";
	puts(logo);
}

/*
	clean shutdown
*/
void kernelShutdown()
{
	kfree(getTrackerUp());
	kfree(getTrackerDown());

	shutdown();
}