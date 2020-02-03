CC = gcc -c

todo : simulador concurrencia
simulador: simulador.o 
	gcc  Parte_1_Simulador/obj/simulador.o -o Parte_1_Simulador/bin/simulador -lpthread -lm
simulador.o: Parte_1_Simulador/src/simulador.c
	$(CC)  Parte_1_Simulador/src/simulador.c -o Parte_1_Simulador/obj/simulador.o -lpthread -lm

concurrencia: concurrencia.o 
	gcc  Parte_2_Concurrencia/obj/concurrencia.o -o Parte_2_Concurrencia/bin/concurrencia -lpthread -lm
concurrencia.o: Parte_2_Concurrencia/src/concurrencia.c
	$(CC)  Parte_2_Concurrencia/src/concurrencia.c -o Parte_2_Concurrencia/obj/concurrencia.o -lpthread -lm
.PHONY: clean
	clean: rm Parte_2_Concurrencia/bin/* Parte_2_Concurrencia/obj/*
