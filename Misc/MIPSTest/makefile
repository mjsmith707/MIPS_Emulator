all:
	gcc -mips32r2 -ffreestanding -Wall -EB -mno-abicalls -nostdlib -nostdinc -e reset_exception -Ttext=0xbfc00000 reset.S entry.S main.c uart.S uart.c -o test.elf
