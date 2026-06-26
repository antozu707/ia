# High School Timetabling Problem (HSTP) - Algoritmo Evolutivo

## Estructura
El proyecto ha sido modularizado para facilitar la lectura y encapsulamiento:
- `req.h`: estructura de datos base de los requerimientos
- `cromosoma.h cromosoma.cpp`: modelo matemático de la solución y función de evaluación (Fitness)
- `archivos.h archivos.cpp`: lectura de .txt y creación de archivo de salida (I/O)
- `algoritmo.h algoritmo.cpp`: motor central del algoritmo genético (operadores de cruzamiento y mutación)
- `main.cpp`: ejecución central

Adicionalmente, se encuentra la carpeta `instancias` que contiene las instancias utilizadas para los experimentos, además de un subdirectorio `resultados_proyecto` con las ejecuciones de estas que fueron utilizadas para el análisis de resultados. Se adjunta también la planilla Excel en donde se fueron registrando los valores de Fitness y tiempo de ejecución.

## Instrucciones
Para compilar y ejecutar el proyecto a través de terminal en entorno Linux, se debe hacer lo siguiente:

1. Ejecutar el comando en terminal:
   ```bash
   make
2. Ejecutar el algoritmo. Se debe pasar la instancia y la semilla por consola:
   ```bash
   ./hstp instancias/CL-CEASD-2008-V-A.txt 12345
3. Se mostrará por consola el progreso de la ejecución del motor evolutivo y se generará el archivo de salida con el mejor horario encontrado en el directorio `instancias`.