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

struct process_setup{
    char* nombre;
    char* size;
    int size_valor;
    int size_unidad;
    int *mem_ref;
    int *ref_string;
    int ref_string_size;
    char *algoritmo;
    int algoritmo_codigo;
    int page_frames;
}proceso;

struct nodo{
    int valor;
    int aux;
    int aux_optimal;
} * nodos;

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
	    
	    else if(strcmp(parte1,"process_name") == 0)
	    {
		proceso.nombre = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(proceso.nombre,parte2);
	    }
	
	    else if(strcmp(parte1,"algorithm") == 0)
	    {
		proceso.algoritmo = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(proceso.algoritmo,parte2);
		if(strcmp(parte2, "fifo") == 0)
		    proceso.algoritmo_codigo = 1;
		if(strcmp(parte2, "lru") == 0)
		    proceso.algoritmo_codigo = 2;
		if(strcmp(parte2, "optimal") == 0)
		    proceso.algoritmo_codigo = 3;
	    }

	    else if(strcmp(parte1,"process_size") == 0)
	    {
		int valor = convertir(parte2);
		if(valor == -1)
		{
	    	    printf("Error archivo de configuracion.\n");
		    printf("Mal valor de memoria en la linea %d.\n", contador_l);
		    exit(0);
		}
		proceso.size = (char*) malloc(strlen(parte2) * sizeof(char));
		strcpy(proceso.size,parte2);
	        proceso.size_valor = getNumero(parte2);
	        proceso.size_unidad = getUnidad(parte2);
	    }

	    else if(strcmp(parte1,"process_memref") == 0)
	    {
		char *aux_memref = strtok(parte2, ",");
		int cont_memref = 0;
		while(aux_memref != NULL)
		{
		    int index = 0;
		    int num_memref = 0;
		    //printf("Aqui ->  %s\n", aux_memref);
		    while(isdigit(aux_memref[index]))
    		    {
			num_memref = num_memref * 10 + (int)((aux_memref[index] - '0'));
			index++;
    		    }
		    if(aux_memref[index] != '\0')
		    {
			printf("Error archivo de configuracion.\n");
		    printf("Mal valor de memoria en la linea %d.\n", contador_l);
		    exit(0);
		    }
		    if(cont_memref == 0)
		    {
			proceso.mem_ref = (int*) malloc(1 * sizeof(int));
			proceso.mem_ref[0] = num_memref; 
			proceso.ref_string = (int*) malloc(1 * sizeof(int));
			proceso.ref_string[0] = (int)((int)num_memref/(int)(sistema.page_size_valor * pow(10, sistema.page_size_unidad)));
			cont_memref++;
		    }
		    else
		    {
			proceso.mem_ref = (int*) realloc(proceso.mem_ref,(cont_memref +1) * sizeof(int));
			proceso.mem_ref[cont_memref] = num_memref; 
			proceso.ref_string = (int*) realloc(proceso.ref_string,(cont_memref +1)* sizeof(int));
			proceso.ref_string[cont_memref] = (int)((int)num_memref/(int)(sistema.page_size_valor * pow(10, sistema.page_size_unidad)));
			cont_memref++;
		    }
		    aux_memref = strtok(NULL, ",");
		}
		proceso.ref_string_size = cont_memref;
	    }
	    else if(strcmp(parte1,"page_frames") == 0)
	    {
		int num_page_frames = 0;
	    	int index = 0;
	        while(isdigit(parte2[index]))
	        {
		    num_page_frames = num_page_frames * 10 + (int)((parte2[index] - '0'));
		    index++;
	   	}
	    	if(parte2[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	            printf("Mal valor numerico en la linea %d.\n", contador_l);
	        exit(0);
	    	}
		proceso.page_frames = num_page_frames;
	    }
	    
	} 
		
    }
    printf("===== Resultados =====\n");

    printf("\n1.- Logical address structure: page bits and offset bits.\n");
    printf("   Page bits = %d       Offset bits = %d\n", sistema.las_convertido - sistema.page_size_convertido, sistema.page_size_convertido);

    printf("\n2.- Physical address structure: frame bits and offset bits.\n");
    printf("   Page bits = %d       Offset bits = %d\n", sistema.pas_convertido - sistema.page_size_convertido, sistema.page_size_convertido);

    printf("\n3.- Max number of Page Table Entries (PTE).\n");
    printf("   PTE size = 4B       MAX number of PTE = %d\n", (int)pow(2,sistema.page_size_convertido - 2));

    printf("\n4.- Max number of Frames.\n");
    printf("   Frame size = Page Size = %s      MAX number of Frames = %d\n",sistema.page_size, (int) ((sistema.pas_valor * pow(10,sistema.pas_unidad))/(sistema.page_size_valor * pow(10, sistema.page_size_unidad))));

    printf("\n5.- Size in bytes required to represent the Page Table (Hint: memory is byte addressable)\n");

    printf("\n6.- Number of pages marked as valid for the process)\n");
    printf("   Pages Valid = %s/%s = %d\n",proceso.size, sistema.page_size,(int)((proceso.size_valor / sistema.page_size_valor)*(pow(10,proceso.size_unidad - sistema.page_size_unidad))));

    printf("\n7.- Reference string based on the address sequence)\n");
    printf("    Mem Ref: ");
    for(int i = 0; i < proceso.ref_string_size; i++)
    {
	printf(" %d  ", proceso.mem_ref[i]);
	if(i % 5 == 4)
	    printf("\n             ");
    }
    printf("\n");

    printf("    Reference String: ");
    for(int i = 0; i < proceso.ref_string_size; i++)
    {
	printf(" %d  ", proceso.ref_string[i]);
	if(i % 5 == 4)
	    printf("\n                      ");
    }
    printf("\n");

    printf("\n8.- Show the output (table) of the page-replacement algorithm)\n");
    nodos = (struct nodo*) malloc(proceso.page_frames * sizeof(struct nodo));
    int contador_ingresos=0;
    int hit = 0;
    int **registro = (int**) malloc(proceso.page_frames * sizeof(int*));
    for(int i = 0; i < proceso.page_frames; i++){
	registro[i] = (int*) malloc(proceso.ref_string_size * sizeof(int));
	nodos[i].valor = -1;
	nodos[i].aux = (-1 * proceso.page_frames) + i;
    }
    int *result = (int*) malloc(proceso.ref_string_size * sizeof(int));
    
    
    for(int i = 0; i < proceso.ref_string_size; i++)
    {
	int bandera = 0;
	for(int j=0; j< proceso.page_frames; j++)
	{
	    if(nodos[j].valor == proceso.ref_string[i])
	    {
		hit++;
		if(proceso.algoritmo_codigo >= 2 )
		    nodos[j].aux = i;
		result[i] = 1;
		bandera =1;
		break;
	    }
	     
	}
	if(bandera == 0)
	{
	    result[i] = -1;
	    if(proceso.algoritmo_codigo == 1 || proceso.algoritmo_codigo == 2)
    	    {
	    int aux_menor = nodos[0].aux;
	    int indice_menor = 0;
	    for(int k=0; k< proceso.page_frames; k++)
	    {
		if(nodos[k].aux < aux_menor)
		{
		    aux_menor = nodos[k].aux;
		    indice_menor = k;
		}   
	    }
	    nodos[indice_menor].valor = proceso.ref_string[i];
	    nodos[indice_menor].aux = contador_ingresos;
	    contador_ingresos++;
	    }
	    if(proceso.algoritmo_codigo == 3)
	    {
		for(int k=0; k< proceso.page_frames; k++)
	   	{
		    for(int l=i;l< proceso.ref_string_size; l++)
		    {
			if(proceso.ref_string[l] == nodos[k].valor)
			{
			    nodos[k].aux_optimal = l - i;
			    break;
			}
			else
			{
			    nodos[k].aux_optimal = proceso.ref_string_size + (i - nodos[k].aux);
			}
		    }
		}
		int aux_menor = nodos[0].aux_optimal;
	    	int indice_menor = 0;
	    	for(int k=0; k< proceso.page_frames; k++)
	     	{
		    if(nodos[k].aux_optimal > aux_menor)
		    {
			aux_menor = nodos[k].aux_optimal;
		   	indice_menor = k;
		    }   
	   	}
		nodos[indice_menor].valor = proceso.ref_string[i];
	        nodos[indice_menor].aux = contador_ingresos;
		contador_ingresos++;
		
	    }

	}
	for(int k=0; k< proceso.page_frames; k++)
	{
	    registro[k][i] = nodos[k].valor;
	}
    }
    
    printf("\nAlgorithm: %s", proceso.algoritmo);
    printf("\nRef Str: |");
    for(int j=0; j< proceso.ref_string_size; j++)
	printf(" %d |",proceso.ref_string[j]);
    printf("\nFrame\n");
    
    for(int i=0; i<proceso.page_frames; i++)
    {
	printf("   %d :   |", i);
	for(int j=0; j< proceso.ref_string_size; j++)
		if(registro[i][j] == -1)
		   printf(" - |");
		else
		   printf(" %d |", registro[i][j]);
	printf("\n");
    }
    printf("\n   R :   |");
    for(int j=0; j< proceso.ref_string_size; j++)
	if(result[j] == 1)
	    printf(" H |");
	else
	    printf(" M |");
    printf("\n");

    printf("\n9.- Summarise number of hits and page fault\n");
    printf("   Hit = %d       Miss = %d\n",hit, proceso.ref_string_size - hit);


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
