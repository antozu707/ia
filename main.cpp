#include <iostream>
#include <chrono>
#include "archivos.h"
#include "algoritmo.h"

using namespace std;

int main() {
    int num_dias, num_periodos, num_clases, num_profesores;
    vector<Requerimiento> requerimientos;
    vector<vector<vector<bool>>> indisponibilidad;

    string archivo_instancia = "CL-CEASD-2008-V-A.txt"; 
    int semilla = 12345; 
    srand(semilla);

    cout << "Leyendo instancia..." << endl;
    leerTXT(archivo_instancia, num_clases, num_profesores, num_dias, num_periodos, requerimientos, indisponibilidad);

    if (!requerimientos.empty()) {
        cout << "Ejecutando motor evolutivo..." << endl;
        auto start = chrono::high_resolution_clock::now();
        
        Cromosoma mejor_horario = ejecutarAlgoritmoEvolutivo(num_dias, num_periodos, num_clases, num_profesores, requerimientos, indisponibilidad);
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        
        escribirArchivoSalida(archivo_instancia, semilla, elapsed.count(), mejor_horario, num_dias, num_periodos, num_clases, requerimientos, 50, 10);
        cout << "¡Exito! Proceso terminado en " << elapsed.count() << "s. Resultados exportados." << endl;
    }
    return 0;
}