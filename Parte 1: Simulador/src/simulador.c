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
#include <ctype.h>
#include <math.h>

struct system_setup{
    char* las;
    int las_valor;
    int las_unidad;
    int las_convertido;
    char* pas;
    int pas_valor;
    int pas_unidad;
    int pas_convertido;
    char* page_size;
    int page_size_valor;
    int page_size_unidad;
    int page_size_convertido;
}sistema;

void trimLeading(char * str);
int convertir(char *memoria);
int getNumero(char *memoria);
int getUnidad(char *memoria);
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
	//printf("%s\n", line);
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
	    
	    if(strcmp(parte1,"las") == 0)
	    {
		int valor = convertir(parte2);
		if(valor == -1)
		{
	    	    printf("Error archivo de configuracion.\n");
		    printf("Mal valor de memoria en la linea%d.\n", contador_l);
		    exit(0);
		}
		sistema.las = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(sistema.las,parte2);
		sistema.las_convertido = valor;
		sistema.las_valor = getNumero(parte2);
	        sistema.las_unidad = getUnidad(parte2);
	    } 

	    else if(strcmp(parte1,"pas") == 0)
	    {
		int valor = convertir(parte2);
		if(valor == -1)
		{
	    	    printf("Error archivo de configuracion.\n");
		    printf("Mal valor de memoria en la linea %d.\n", contador_l);
		    exit(0);
		}
		sistema.pas = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(sistema.pas,parte2);
		sistema.pas_convertido = valor;
		sistema.pas_valor = getNumero(parte2);
	        sistema.pas_unidad = getUnidad(parte2);
	    }
	    
	    else if(strcmp(parte1,"pagesize") == 0)
	    {
		int valor = convertir(parte2);
		if(valor == -1)
		{
	    	    printf("Error archivo de configuracion.\n");
		    printf("Mal valor de memoria en la linea %d.\n", contador_l);
		    exit(0);
		}
		sistema.page_size = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(sistema.page_size,parte2);
		sistema.page_size_convertido = valor;
	        sistema.page_size_valor = getNumero(parte2);
	        sistema.page_size_unidad = getUnidad(parte2);
	    }
	} 
		
    }
    printf("\nTodo bien\n");
    printf("===== Resultados =====\n");

    printf("\n1.- Logical address structure: page bits and offset bits.\n");
    printf("   Page bits = %d       Offset bits = %d\n", sistema.las_convertido - sistema.page_size_convertido, sistema.page_size_convertido);

    printf("\n2.- Physical address structure: frame bits and offset bits.\n");
    printf("   Page bits = %d       Offset bits = %d\n", sistema.pas_convertido - sistema.page_size_convertido, sistema.page_size_convertido);

    printf("\n3.- Max number of Page Table Entries (PTE).\n");
    printf("   PTE size = 4B       MAX number of PTE = %d\n", (int)pow(2,sistema.page_size_convertido - 2));

    printf("\n4.- Max number of Frames.\n");
    printf("   Frame size = Page Size = %s      MAX number of Frames = %d\n",sistema.page_size, (int) ((sistema.pas_valor * pow(10,sistema.pas_unidad))/(sistema.page_size_valor * pow(10, sistema.page_size_unidad))));
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

    if(1)
    {
        /* Shit all trailing characters to its left */
        i = 0;
	j = 0;
        while(str[i + index] != '\n')
        {
	    
	    if(str[i + index] != ' ' && str[i + index] != '\t')
            {
	        str[j] = tolower(str[i + index]);
                j++;
	    }
	    i++;
        }
        str[j] = '\0'; // Make sure that string is NULL terminated
    }
}

int convertir(char *memoria)
{
    int index = 0;
    int numero = 0;
    while(isdigit(memoria[index]))
    {
	numero = numero * 10 + (int)((memoria[index] - '0'));
	index++;
    }
    if(index == 0)
	return -1;
    if(strcmp((memoria + index), "b") == 0)
	return (int) (log(numero) / log(2)) ;
    if(strcmp((memoria + index), "kb") == 0)
	return 10 + (int) (log(numero) / log(2));
    if(strcmp((memoria + index), "mb") == 0)
	return 20+ (int) (log(numero) / log(2));
    if(strcmp((memoria + index), "gb") == 0)
	return 30 + (int) (log(numero) / log(2));
    if(strcmp((memoria + index), "tb") == 0)
	return 40 + (int) (log(numero) / log(2));
    return -1;
}

int getNumero(char *memoria)
{
    int index = 0;
    int numero = 0;
    while(isdigit(memoria[index]))
    {
	numero = numero * 10 + (int)((memoria[index] - '0'));
	index++;
    }
    return numero;
}

int getUnidad(char *memoria)
{
    int index = 0;
    int numero = 0;
    while(isdigit(memoria[index]))
    {
	numero = numero * 10 + (int)((memoria[index] - '0'));
	index++;
    }
    if(index == 0)
	return -1;
    if(strcmp((memoria + index), "b") == 0)
	return 0 ;
    if(strcmp((memoria + index), "kb") == 0)
	return 3;
    if(strcmp((memoria + index), "mb") == 0)
	return 6;
    if(strcmp((memoria + index), "gb") == 0)
	return 9;
    if(strcmp((memoria + index), "tb") == 0)
	return 12;
    return -1;
}
