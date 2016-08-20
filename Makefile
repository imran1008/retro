
all: retro

clean:
	rm -rf *.o retro

retro: retro.o main.o
	gcc -o retro retro.o main.o -lpthread `pkg-config --libs gtk+-3.0`

retro.o: retro.c
	gcc -o retro.o -c retro.c `pkg-config --cflags gtk+-3.0`

main.o: main.c
	gcc -o main.o -c main.c -I.

