CC = gcc -Wall -c

todo : simulador
simulador: simulador.o 
	gcc  obj/simulador.o -o bin/simulador -lpthread -lm
simulador.o: src/simulador.c
	$(CC)  src/simulador.c -o obj/simulador.o -lpthread -lm

.PHONY: clean
	clean: rm bin/* obj/*
