/* 
Proyecto Sistemas Operativos
Parcial 2   Paralelo #2
Autor: Oswaldo Aguilar - Matricula: 201419079
Tutor: Angel Lopez.
Fecha: 03/02/2020

Parte #2 -> Concurrencia
A linked list is accessed by threads to do these operations:
Search:
Insert:
Update:
Delete:
Search operations can be executed concurrently. Insert operations are mutually
exclusive among them to avoid overwriting the last item. However, insert and search
operations can happen concurrently. Update operations are mutually exclusive. No
search can be performed over a node that is being updated. Delete operations are
mutually exclusive among them and also to the other operations (search, insert and
update).
Define a file with a set of operations to test your program.
Make sure no deadlock occurs during the execution.
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
#include <pthread.h> //Libreria para hilos
#include <sys/sysinfo.h>


// Estructura y funciones de la linked list
struct node {
   int data;
   int key;
   struct node *next;
   int op_act; // variable que almacena la operacion que esta actualmente ejecutandose en el nodo 0 es nada, 1 es insert, 2 es search, 3 es update y 4 remove
};

//Estructurando nodo
struct node *head = NULL;
struct node *current = NULL;

//display the list
void printList() {
   struct node *ptr = head;
   printf("\n[ ");
	
   //start from the beginning
   while(ptr != NULL) {
      printf("(%d,%d) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
	
   printf(" ]");
}

//insert link at the first location
int insertFirst(int key, int data) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   if(link ->op_act == 4 || link -> op_act == 1)
	return -1;
   link ->op_act = 1; //Setear que se esta insertando
   link->key = key;
   link->data = data;
   
	
   //point it to old first node
   link->next = head;
	
   //point first to new first node
   head = link;
   link ->op_act = 0; //Setear que se termino de insertar
   return 0;
}

//delete first item
struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = head;
	
   //mark next to first link as first 
   head = head->next;
	
   //return the deleted link
   return tempLink;
}

//is list empty
int isEmpty() {
   return head == NULL;
}

int length() {
   int length = 0;
   struct node *current;
	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
	
   return length;
}

//find a link with given key
struct node* find(int key) {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
   if(current -> op_act == 1 || current->op_act == 3) //Validar que no este ocupado el nodo
   {
	return NULL;
   }
   current ->op_act = 2; //Setear
   current -> op_act = 0;//Limpiar
   //if data found, return the current Link
   return current;
}

//delete a link with given key
struct node* delete(int key) {

   //start from the first link
   struct node* current = head;
   struct node* previous = NULL;
	
   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {

      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;
      }
   }

   //found a match, update the link
   if(current ->op_act != 0){ //Preguntar
	return current;
   }
   current ->op_act = 4; //Setear
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      previous->next = current->next;
   } 
   current ->op_act = 0; //Limpiar
	
   return current;
}

void sort() {

   int i, j, k, tempKey, tempData;
   struct node *current;
   struct node *next;
	
   int size = length();
   k = size ;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = head;
      next = head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data > next->data ) {
            tempData = current->data;
            current->data = next->data;
            next->data = tempData;

            tempKey = current->key;
            current->key = next->key;
            next->key = tempKey;
         }
			
         current = current->next;
         next = next->next;
      }
   }   
}

void reverse(struct node** head_ref) {
   struct node* prev   = NULL;
   struct node* current = *head_ref;
   struct node* next;
	
   while (current != NULL) {
      next  = current->next;
      current->next = prev;   
      prev = current;
      current = next;
   }
	
   *head_ref = prev;
}
void trimLeading(char * str);//Funcion de limpieza de lineas
void *ejecutar(char** ptr); //Funcion de control de hilo

int main(int argc, char **argv)
{
    //Variables para lectura de archivo
    char * line = NULL; 
    size_t len = 0;
    ssize_t read;

    //Validando entradas
    if(argc == 1){
        printf("No ha ingresado ningun archivo de configuracion\n");
	printf("Ejemplo: ./concurrencia <ruta_archivo_configuracion\n"); 	
	exit(0);
    }
	
    if(argc > 2){
	printf("Ha ingresado mas de un parametro en el comando\n");
	printf("Ejemplo: ./concurrencia <ruta_archivo_configuracion\n"); 	
	exit(0);
    }
	
    FILE *f = fopen(argv[1],"r");
    //Validar el archivo
    if(f == NULL)
    {
	printf("El archivo no existe o la ruta ingresada es incorrecta\n");   
        exit(0);             
    }
    //Procesar el archivo
    int contador_l = 0;
    int num_hilos = 0;
    pthread_t** hilos_p;
    char*** comandos;
    int cont =0;
    while((read = getline(&line, &len, f)) != -1)
    {
	contador_l +=1;
	//printf("Retrieved line of length %zu:\n", read);
	trimLeading(line);
	//printf("%s\n", line);
	if(line[0] != '#')
	{
	    //Validar consitencia de linea
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
	    //En caso que sea el numero de hilos
	    if(strcmp(parte1,"num_hilos") == 0)
	    {
		//Convertir a numerico y validar
		int index = 0;
	    
	        while(isdigit(parte2[index]))
	        {
	  	    int n=  parte2[index] - '0';
		    num_hilos = (num_hilos * 10) + (int)n;
		    index++;
	    	}
	    	if(parte2[index] != '\0'  || num_hilos <0)
	    	{
		    printf("Error archivo de configuracion.\n");
	            printf("Mal valor de numero en la linea %d.\n", contador_l);
	    	    exit(0);
	     	}
		//Crear los hilos y instanciar comandos
		hilos_p = (pthread_t**)malloc(num_hilos * sizeof(pthread_t*));
		comandos = (char***) malloc(num_hilos * sizeof(char**));
	    }
	    
	    else
	    {
		//Procesar la lista de instrucciones por hilo.
		int aux_c = 0;
		char *comando = strtok(parte2, ";");
		comandos[cont] = (char**) malloc(30* sizeof(char*));
		while(comando != NULL)
		{
			//printf("%s\n", comando);
			
			comandos[cont][aux_c] = (char*) malloc(10 * sizeof(char));
			strcpy(comandos[cont][aux_c], comando);
			
			aux_c++;
			comando = strtok(NULL, ";");
			
		}
		//Cargar los hilos y iniciar
		hilos_p[cont]= (pthread_t*) malloc(sizeof(pthread_t));
		//pthread_create(hilos_p[cont], NULL, ejecutar, NULL);
		pthread_create(hilos_p[cont], NULL, ejecutar, (char**) comandos[cont]);
		pthread_join(*(hilos_p[cont]), NULL);		
		cont++;

	    }
	    
	}
    }
    
}

//Funcion de limpieza de linea
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

//Proceso de hilo
void *ejecutar(char** ptr){
    //Carga los comandos del hilo
    char **comandos = (char**) ptr;
    for(int i=0; i< 30; i++)
    {
	if(comandos[i] == NULL)
		break;
	printf("%s\n",comandos[i]);
	char* identificador = strtok(comandos[i], ",");
	if(identificador!= NULL)
	{
	    // En caso que sea insersion
	    if(strcmp(identificador,"i") == 0)
	    {
		char *valor = strtok(NULL, ",");
		int index = 0;
	    	int cant = 0;
	        while(isdigit(valor[index]))
	        {
	  	    int n=  valor[index] - '0';
		    cant = (cant * 10) + (int)n;
		    index++;
		    
		    
	    	}
	    	if(valor[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	    	    exit(0);
	     	}
		while(1){
		int v_i = insertFirst(cant,cant);
		if(v_i == -1)
			sleep(20);
		else 
			break;
		}
		
		printList();
		
		printf("\n");
		
	    }
	    // En caso de eliminacion
	    if(strcmp(identificador,"r") == 0)
	    {
		char *valor = strtok(NULL, ",");
		int index = 0;
	    	int cant = 0;
	        while(isdigit(valor[index]))
	        {
	  	    int n=  valor[index] - '0';
		    cant = (cant * 10) + (int)n;
		    index++;
		    
		    
	    	}
	    	if(valor[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	    	    exit(0);
	     	}
		
		struct node * aux =delete(cant);
		
		printList();
		
		printf("\n");
		
	    }
	    //En caso de busqueda
	    if(strcmp(identificador,"s") == 0)
	    {
		char *valor = strtok(NULL, ",");
		int index = 0;
	    	int cant = 0;
	        while(isdigit(valor[index]))
	        {
	  	    int n=  valor[index] - '0';
		    cant = (cant * 10) + (int)n;
		    index++;
		    
		    
	    	}
	    	if(valor[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	    	    exit(0);
	     	}
		struct node * aux = find(cant);
		if(aux != NULL)
			printf("%d", find(cant)-> data);
		
		printf("\n");
		
	    }
	    // En caso de actualizacion
	    if(strcmp(identificador,"u") == 0)
	    {
		char *valor = strtok(NULL, ",");
		int index = 0;
	    	int cant = 0;
	        while(isdigit(valor[index]))
	        {
	  	    int n=  valor[index] - '0';
		    cant = (cant * 10) + (int)n;
		    index++;
		    
		    
	    	}
	    	if(valor[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	    	    exit(0);
	     	}
		
		valor = strtok(NULL, ",");
		index = 0;
	    	int cant_n = 0;
	        while(isdigit(valor[index]))
	        {
	  	    int n=  valor[index] - '0';
		    cant_n = (cant_n * 10) + (int)n;
		    index++;
		    
		    
	    	}
	    	if(valor[index] != '\0')
	    	{
		    printf("Error archivo de configuracion.\n");
	    	    exit(0);
	     	}

		struct node * aux = find(cant);
		if(aux ->op_act >= 3)
			sleep(20);
		aux ->op_act= 3;
		if(aux != NULL)
		{
		   aux -> key = cant_n;
		   aux -> data = cant_n;
		}
		aux ->op_act= 0;
		printf("\n");
		
	    }
	}
    }
	printf("\n");


    
}

