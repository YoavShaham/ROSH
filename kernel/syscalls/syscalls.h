#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#include "../IDT/reg_def.h"

#define G_SYSTEM	0
	#define F_SHUTDOWN	0

#define G_OUTPUT	1
	#define F_PUTC		0
	#define F_PUTS		1
	#define F_PUTI		2
	#define F_COLOR		3

#define G_INPUT		2
	#define F_GETCHAR	0
	#define F_GETLINE	1
	#define F_BFLUSH	2

#define G_FS		3
	#define F_CREATE_F	0
	#define F_CREATE_D	1
	#define F_FS_DELETE	2
	#define F_FS_SIZE	3
	#define F_FS_READ	4
	#define F_FS_WRITE	5
	#define F_FS_TYPE	6

#define G_PROCESS	4
	#define F_NEW_PROC	0
	#define F_SLEEP		1

#define G_NET		5
	#define F_NET_INFO	0
	#define F_NET_ARP	1
	#define F_NET_ROSH	2

/*
	Syscall handler (Ring 0)
*/
void syscall_handler(registers_t* registers);

#endif