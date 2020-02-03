# Proyecto-Operativos-Final

## Resumen
  **Proyecto correspondiente al segundo Parcial de la materia de Sistemas Operativos.**<br>
  **Autor:** Oswaldo Aguilar Mora   &nbsp; &nbsp; &nbsp; &nbsp; **Matrícula:** 201419079 <br>
  **Tutor:** Angel Lopez    &nbsp; &nbsp; &nbsp;&nbsp; **Paralelo #** 2<br>
  **Fecha:** 03/02/2020<br>
  
  ## Estructura
   + **Proyecto Parcial -Oswaldo Aguilar** -> Código
      - bin: Ejecutables
      - files: Archivos de prueba
      - include
      - obj
      - src: Archivos fuentes
        - simulacion.c -> Parte 1 del proyecto //Carpeta Primera Parte
        - concurrencia.c  -> Parte 2 del proyecto //Carpeta Segunda Parte
      - Makefile
      <br>
   + Diagramas:
      <br>
 
 ## Especificacion
   + **Simulacion** -> Validaciones necesarias por lo que permite las siguientes entradas.
     - no ingresar config file
     - ingresar más parametros
     - lineas con espacios en el config file
     - lineas con variables inexistentes
     - Errores de tipos de Datos y Unidades
     - Errores de asignacion de paginas
     - Estructura reporte
     - [executable] [args]
     
     ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/shell.PNG)
     
   + **COncurrencia** -> Validaciones necesarias por lo que permite las siguientes entradas. Se definio un archivo de configuracion
     - El archivo de configuracion debe tener numero de hilos y una lista de comandos para cada hilo.
     - Insercion: Comando insercion -> i,valor
     - Busqueda: Comando busqueda -> s, valor
     - Actualizacion: Comando actualizacion -> u, viejo, nuevo
     - Eliminacion: Comando eliminacion -> r, valor
     
     ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/twc.PNG)
     
    
 ## Estrategia
 ![Estrategia wtc](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/Estrategia%20twc.PNG)
 
 
  ## Bugs
     - No valida campos faltantes.
     - Valores muy grande pueden causar overflow de entero
     - Muchos valores en el string de proceso causan que no se aprecie bien el grafico
     
   ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/twc_bug.PNG)
   
   
   ## Discusión
     - En el caso de la simulacion se considero que la unidad maxima es Terabyte.
     - Se consideraron usar estructuras para almacenar la informacion.
     - Se opto por redundancia y uso de memoria extra para que sea mas facil el calulo directo.
     - Se convierte la cadena de referencia al mismo tiempo que se va leyendo para ahorrar tiempo.
     - No valida errores previamente, al encontrarlos muestra los mensajes.
     - Pense hacer un proceso para cada algoritmo pero las bases son las mismas en ciertos puntos por lo que opte por usar uno general y cambiar los valores para algunos casos y añadir una variable extra. Al mismo tiempo uso memoria extra y el almaceno el registro de cada iteracion para poder presentarlo con un formato adecuado al final.
     
     
     - En el caso de concurrencia, se opto por una lista elnzada simple, que almacena enteros.
     - Añadiendo una variable en cada nodo para saber si esta libre o usada por otro metodo.
     - Se usa un archivo de configuracion con un formato especifico, donde se crean n hilos.
     - Se planeo inicialmente usar semaforos pero se cambio al final a validacion por la bandera en cada nodo.
     - Al dividir un archivo puebe existir un desfase en el conteo ya que una palabra puede ser cortada y contada dós veces por lo que 
     - Dificil de comproboar en maquinas con un procesador.
     
   
