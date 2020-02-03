/* 
Proyecto Sistemas Operativos
Parcial 2   Paralelo #2
Autor: Oswaldo Aguilar - Matricula: 201419079
Tutor: Angel Lopez.
Fecha: 03/02/2020

Parte #1 -> Simulador de Remplazo de Paginas.
Crear un aplicativo en c que permita simular una version simplificada del shell
de Linux con dos principales funciones exit and cd implementadas usando las llamadas del sistema necesarias. Adicionalmente la creacion de un subproceso que incie un ejecutable sin perder el principal.
*/

//Importaciones Necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include<unistd.h>
#include <errno.h> // Ayuda a validar errores
#include<sys/wait.h> // Necesaria para realizar las pausas

struct system_setup{
    int las;
    int las_u;
    int pas;
    int pas_u;
    int page_size;
    int page_size_u;
}sistema;

void trimLeading(char * str);
int main(int argc, char **argv)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //Validando entradas
    if(argc == 1){
        printf("No ha ingresado ningun archivo de configuracion\n");
	printf("Ejemplo: ./simulador <ruta_archivo_configuracion\n"); 	
	exit(0);
    }
	
    if(argc > 2){
	printf("Ha ingresado mas de un parametro en el comando\n");
	printf("Ejemplo: ./simulador <ruta_archivo_configuracion\n"); 	
	exit(0);
    }
	
    FILE *f = fopen(argv[1],"r");
    //Validar el archivo
    if(f == NULL)
    {
	printf("El archivo no existe o la ruta ingresada es incorrecta\n");   
        exit(0);             
    }
    int contador_l = 0;
    while((read = getline(&line, &len, f)) != -1)
    {
	contador_l +=1;
	//printf("Retrieved line of length %zu:\n", read);
	trimLeading(line);
	//printf("%s", line);
	if(line[0] != '#')
	{
	    //En caso que sea LAS el campo
	    char *parte1 = strtok(line, "=");
	    //printf("%s\n", token);
	    char *parte2 = strtok(NULL, "=");
	    //printf("%s\n", token);
	    char *parte3 = strtok(NULL, "=");
    	    if(parte3 != NULL)
	    {
		printf("Error archivo de configuracion.\n");
		printf("Existen mas de un igual en la linea %d.\n", contador_l);
		exit(0);
            }	
	} 
		
    }
    printf("Todo bien\n");
}

void trimLeading(char * str)
{
    int index, i, j;
    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}
