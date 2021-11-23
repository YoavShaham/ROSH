run: clean_output compile_boot compile_libc compile_kernel qemu
debug: clean_output compile_boot compile_libc compile_kernel qemu_debug

clean_output:
	@rm -rf compiled/
	@mkdir compiled/
	@rm -rf objects/
	@mkdir objects/
	@mkdir objects/kernel/

compile_boot:
	@echo "Compiling boot..."
	@nasm -fbin boot/boot_sect.s -o compiled/boot_sect.bin

compile_libc:
	@echo "Compiling libc..."
	@i386-elf-gcc -ffreestanding -c libc/screen.c -o objects/screen.o
	@i386-elf-gcc -ffreestanding -c libc/string.c -o objects/string.o
	
compile_kernel:
	@echo "Compiling kernel..."
	@i386-elf-gcc -ffreestanding -c kernel/kernel_main.c -o objects/kernel/kernel_main.o
	@i386-elf-gcc -ffreestanding -c kernel/ports.c -o objects/ports.o
	@i386-elf-gcc -ffreestanding -c kernel/memory/paging.c -o objects/paging.o
	@i386-elf-gcc -ffreestanding -c kernel/GDT/gdt.c -o objects/gdt.o
	@i386-elf-gcc -ffreestanding -c kernel/IDT/time.c -o objects/time.o
	@i386-elf-gcc -ffreestanding -c kernel/IDT/keyboard.c -o objects/keyboard.o
	@i386-elf-gcc -ffreestanding -c kernel/IDT/irq.c -o objects/irq.o
	@i386-elf-gcc -ffreestanding -c kernel/IDT/idt.c -o objects/idt.o
	@i386-elf-gcc -ffreestanding -c kernel/IDT/isr.c -o objects/isr.o
	@nasm kernel/kernel_entry.s -f elf -o objects/kernel/kernel_entry.o
	@nasm kernel/IDT/interrupt_main.s -f elf -o objects/interrupt_main.o
	@nasm kernel/GDT/load_gdt.s -f elf -o objects/load_gdt.o
	@nasm kernel/IDT/load_idt.s -f elf -o objects/load_idt.o
	@i386-elf-ld -o compiled/kernel_main.bin -Ttext 0x6400000 objects/kernel/*.o objects/*.o --oformat binary

qemu:
	@echo "Launching..."
	@cat compiled/boot_sect.bin compiled/kernel_main.bin /dev/zero | head -c 1048576 > rosh.bin
	@qemu-system-i386 -drive file=rosh.bin,index=0,format=raw

qemu_debug:
	@echo "Launching Debug..."
	@cat compiled/boot_sect.bin compiled/kernel_main.bin /dev/zero | head -c 1048576 > rosh.bin
	@qemu-system-i386 -s -S -drive file=rosh.bin,index=0,format=raw