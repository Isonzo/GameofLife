all: gof clean
gof: main.o menu.o
	gcc main.o menu.o -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o gof

debug:
	gcc -g main.c menu.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

main.o: main.c
	gcc -c main.c

menu.o: menu.c menu.h
	gcc -c menu.c

clean:
	rm *.o