#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

using namespace std;

// REPRESENTACIÓN Y MANEJO DE RESTRICCIONES

// Estructura para almacenar la información de las instancias
struct Requerimiento {
    int id;
    int clase_id;
    int profe_id;
    int lecciones_totales;
    int max_diario;
    int dobles_min;
};

class HSTPSolver {
private:
    int dias, periodos;
    vector<Requerimiento> requerimientos;
    
    // Matriz centrada en las clases. Valor = id del requerimiento. -1 si está vacío.
    map<int, vector<vector<int>>> horario;
    
    // Matriz auxiliar para manejar la restricción de que un profesor no puede dar dos clases a la vez
    map<int, vector<vector<bool>>> profe_ocupado;

public:
    HSTPSolver(int d, int p) : dias(d), periodos(p) {}

// LECTURA DE INSTANCIAS Y GESTION INTERNA (ENTRADA)

void leerInstancia(const string& archivo_entrada) {
    ifstream file(archivo_entrada);
    if (!file.is_open()) {
        cerr << "Error al abrir la instancia.\n";
        return;
    }
    
    int num_reqs;
    file >> dias >> periodos >> num_reqs; // Parámetros generales
    
    for (int i = 0; i < num_reqs; i++) {
        Requerimiento req;
        // Lectura de variables del requerimiento
        file >> req.id >> req.clase_id >> req.profe_id 
                >> req.lecciones_totales >> req.max_diario >> req.dobles_min;
        requerimientos.push_back(req);
        
        // Si la clase no existe en el mapa, inicializamos su matriz con -1 (vacío)
        if (horario.find(req.clase_id) == horario.end()) {
            horario[req.clase_id] = vector<vector<int>>(dias, vector<int>(periodos, -1));
        }
        // Si el profe no existe, inicializamos su disponibilidad en falso
        if (profe_ocupado.find(req.profe_id) == profe_ocupado.end()) {
            profe_ocupado[req.profe_id] = vector<vector<bool>>(dias, vector<bool>(periodos, false));
        }
    }
    file.close();
}