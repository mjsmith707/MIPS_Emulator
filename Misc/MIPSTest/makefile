all:
	gcc -mips32r2 -fno-pic -Wall -EB -nostdinc --entry=_start -Ttext 80008000 entry.S main.c uart.S uart.c -o test.elf
