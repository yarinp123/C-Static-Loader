all: task2

task2: task2.c startup.o start.o
	gcc -m32 task2.c -c -o task2.o
	gcc -m32 task2.o startup.o -o loader
	ld -o loader task2.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

start: start.o
	 gcc -g -m32 -Wall -o start start.o

 # Depends on the source and header files 

start.o: start.s
	nasm -f elf32 -o start.o start.s  

startup: startup.o
	 	nasm -f elf32 -o startup startup.o

 # Depends on the source and header files 

startup.o: startup.s
	nasm -g -f elf32 -w+all -o startup.o startup.s  

.PHONY: clean

clean:
	rm -f *.o task2
