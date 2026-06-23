#include <iostream>
#include <string>
#include <chrono>
#include "archivos.h"
#include "algoritmo.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << " ERROR: Uso incorrecto del programa." << endl;
        cout << " Formato esperado: ./hstp.exe <archivo_instancia.txt> <semilla>" << endl;
        return 1; 
    }

    string archivo_instancia = argv[1];
    int semilla = stoi(argv[2]);

    int num_dias, num_periodos, num_clases, num_profesores;
    vector<Requerimiento> requerimientos;
    vector<vector<vector<bool>>> indisponibilidad;

    srand(semilla);

    cout << "---------------------------------------------------------" << endl;
    cout << " Instancia: " << archivo_instancia << endl;
    cout << " Semilla: " << semilla << endl;
    cout << "---------------------------------------------------------" << endl;

    leerTXT(archivo_instancia, num_clases, num_profesores, num_dias, num_periodos, requerimientos, indisponibilidad);

    if (!requerimientos.empty()) {
        cout << "Ejecutando motor evolutivo" << endl;
        auto start = chrono::high_resolution_clock::now();
        
        Cromosoma mejor_horario = ejecutarAlgoritmoEvolutivo(num_dias, num_periodos, num_clases, num_profesores, requerimientos, indisponibilidad);
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        
        escribirArchivoSalida(archivo_instancia, semilla, elapsed.count(), mejor_horario, num_dias, num_periodos, num_clases, requerimientos, 50, 10);
        
        cout << "Proceso terminado en " << elapsed.count() << " seg" << endl;
        cout << "Resultados exportados correctamente" << endl;
    } else {
        cout << "ERROR: No se pudo cargar la instancia correctamente o esta vacia" << endl;
    }
    
    return 0;
}