#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>

using namespace std;

// Estructura de los datos de entrada
struct Requerimiento {
    int id;
    int clase_id;
    int profe_id;
    int lecciones_totales;
};

// Nuestro "Cromosoma"
struct Individuo {
    // horario[clase_id][dia][periodo] = id_requerimiento
    map<int, vector<vector<int>>> horario;
    int costo_fitness = 0; // Entre menor sea, mejor es el individuo
};

int evaluarFitness(Individuo& ind, int dias, int periodos, const vector<Requerimiento>& reqs) {
    int penalizacion = 0;
    
    // Mapa auxiliar para detectar choques de profesores: profe_id -> matriz[dia][periodo] = cantidad de clases
    map<int, vector<vector<int>>> profe_carga;

    // 1. Llenar la carga de los profesores según este horario
    for (const auto& [clase_id, matriz_clase] : ind.horario) {
        for (int d = 0; d < dias; d++) {
            for (int p = 0; p < periodos; p++) {
                int id_req = matriz_clase[d][p];
                if (id_req != -1) {
                    // Buscar qué profe da este requerimiento
                    int profe_id = -1;
                    for (const auto& r : reqs) {
                        if(r.id == id_req) { profe_id = r.profe_id; break; }
                    }
                    
                    // Si es la primera vez que vemos a este profe, inicializamos su matriz
                    if (profe_carga.find(profe_id) == profe_carga.end()) {
                        profe_carga[profe_id] = vector<vector<int>>(dias, vector<int>(periodos, 0));
                    }
                    profe_carga[profe_id][d][p]++; // Sumamos una clase a este profe en este bloque
                }
            }
        }
    }

    // 2. Calcular Castigos
    for (const auto& [profe_id, matriz] : profe_carga) {
        for (int d = 0; d < dias; d++) {
            for (int p = 0; p < periodos; p++) {
                // RESTRICCIÓN DURA: Choque de profesor
                if (matriz[d][p] > 1) {
                    penalizacion += 1000 * (matriz[d][p] - 1); // Castigo severo
                }
            }
        }
        // *Aquí también calcularías las ventanas/huecos del profesor y sumarías un castigo menor (ej. 10)*
    }

    ind.costo_fitness = penalizacion;
    return penalizacion;
}

Individuo cruzar(const Individuo& padre1, const Individuo& padre2) {
    Individuo hijo;
    // Iteramos sobre las clases
    for (const auto& [clase_id, matriz] : padre1.horario) {
        // 50% de probabilidad de heredar la clase del padre 1 o del padre 2
        if (rand() % 2 == 0) {
            hijo.horario[clase_id] = padre1.horario.at(clase_id);
        } else {
            hijo.horario[clase_id] = padre2.horario.at(clase_id);
        }
    }
    return hijo;
}

void mutar(Individuo& ind, int dias, int periodos, double probabilidad_mutacion) {
    // Si el número aleatorio es menor a la probabilidad, mutamos
    double r = ((double) rand() / (RAND_MAX));
    if (r > probabilidad_mutacion) return;

    // Elegir una clase al azar del individuo
    auto it = ind.horario.begin();
    advance(it, rand() % ind.horario.size());
    int clase_elegida = it->first;

    // Elegir dos coordenadas al azar para intercambiar
    int d1 = rand() % dias, p1 = rand() % periodos;
    int d2 = rand() % dias, p2 = rand() % periodos;

    // Intercambiar (Swap) el contenido de las dos celdas
    swap(ind.horario[clase_elegida][d1][p1], ind.horario[clase_elegida][d2][p2]);
}

class HSTPEvolutivo {
private:
    int dias, periodos;
    vector<Requerimiento> requerimientos;
    vector<Individuo> poblacion;
    int tamano_poblacion = 50;
    int generaciones = 1000;
    double prob_mutacion = 0.1;

public:
    HSTPEvolutivo(int d, int p) : dias(d), periodos(p) {}

    // (Aquí iría la función leerInstancia similar a la versión anterior)

    void generarPoblacionInicialAleatoria() {
        // Por brevedad: Debes crear 'tamano_poblacion' individuos.
        // Para cada individuo, tomas los requerimientos y los insertas en 
        // coordenadas [dia][periodo] totalmente al azar dentro de su clase_id.
    }

    void resolver() {
        generarPoblacionInicialAleatoria();

        for (int gen = 0; gen < generaciones; gen++) {
            // 1. Evaluar Fitness de toda la población
            for (auto& ind : poblacion) {
                evaluarFitness(ind, dias, periodos, requerimientos);
            }

            // Ordenar población de mejor (menor costo) a peor
            sort(poblacion.begin(), poblacion.end(), [](const Individuo& a, const Individuo& b) {
                return a.costo_fitness < b.costo_fitness;
            });

            cout << "Generación " << gen << " - Mejor Costo: " << poblacion[0].costo_fitness << "\n";

            // Si llegamos a costo 0, encontramos una solución perfecta sin choques
            if (poblacion[0].costo_fitness == 0) break;

            vector<Individuo> nueva_poblacion;
            
            // Elitismo: Guardar automáticamente al mejor individuo (el 1ro) para no perderlo
            nueva_poblacion.push_back(poblacion[0]);

            // 2. Selección y Cruza para el resto de la población
            while (nueva_poblacion.size() < tamano_poblacion) {
                // Selección por torneo simple (elegir los mejores de los primeros lugares)
                Individuo padre1 = poblacion[rand() % (tamano_poblacion / 2)];
                Individuo padre2 = poblacion[rand() % (tamano_poblacion / 2)];
                
                Individuo hijo = cruzar(padre1, padre2);
                
                // 3. Mutación
                mutar(hijo, dias, periodos, prob_mutacion);
                
                nueva_poblacion.push_back(hijo);
            }
            poblacion = nueva_poblacion;
        }
    }
};

