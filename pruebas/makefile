shell.out: shell.c file_system.o list.o
	gcc -Wall -o shell.out shell.c file_system.o list.o

file_system.o: file_system.c file_system.h
	gcc -Wall -c -o file_system.o file_system.c

list.o: list.c list.h
	gcc -Wall -c -o list.o list.c

clean:
	rm shell.out file_system.o list.o
