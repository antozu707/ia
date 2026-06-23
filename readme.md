# High School Timetabling Problem (HSTP) - Algoritmo Evolutivo

## Estructura del Proyecto
El proyecto ha sido modularizado para facilitar la lectura y encapsulamiento:
- `Requerimiento.h`: Estructura de datos base.
- `Cromosoma.h/cpp`: Modelo matemático de la solución y función de Evaluación (Fitness).
- `Archivos.h/cpp`: Módulo de Entrada/Salida (Lectura de .txt y creación de salida).
- `Evolutivo.h/cpp`: Motor central del algoritmo genético (Operadores de cruzamiento y mutación).
- `main.cpp`: Ejecución central.

## Dependencias
El código fuente **NO requiere el uso de librerías externas**. Utiliza únicamente la librería estándar de C++ (`std`).

## Instrucciones de Compilación y Ejecución (Vía Makefile)
Para compilar y ejecutar el proyecto a través de terminal, siga los siguientes pasos:

1. Asegúrese de tener un compilador de C++ (`g++`) y `make` instalados en su sistema.
2. Coloque el archivo de instancia (ej: `CL-CEASD-2008-V-A.txt`) en el directorio raíz junto al código.
3. Abra la terminal en la carpeta del proyecto.
4. Para compilar, ejecute el comando:
   ```bash
   make