.PHONY: all clear arc

all: main.o funcs.o mainProg

funcs.o:
	nasm -f elf32 funcs.asm -o funcs.o

# Выбор метода решения с помощью переменной method:
# SEGMENT - метод отрезков
# SECANT - метод хорд
main.o:
	gcc -m32 -c main.c -o main.o -D $(method)

mainProg:
	gcc -m32 -lm main.o funcs.o -o main

clean:
	rm *.o main

arc:
	mkdir -p ARC
	zip `date +%Y.%m.%d_%N`.zip main.c Makefile funcs.asm
	mv *.zip ARC/