#include "algoritmo.h"
#include <algorithm>
#include <random>

Cromosoma generarSolucionInicial(int num_dias, int num_periodos, int num_clases, vector<Requerimiento> reqs) {
    Cromosoma ind(num_dias, num_periodos, num_clases);
    
    sort(reqs.begin(), reqs.end(), [](const Requerimiento& a, const Requerimiento& b) {
        return a.lecciones_dobles_minimas > b.lecciones_dobles_minimas;
    });

    for (const auto& req : reqs) {
        int asignadas = 0, intentos = 0; 
        while (asignadas < req.lecciones_totales && intentos < 1000) {
            int d = rand() % num_dias;
            int p = rand() % num_periodos;
            
            if (req.lecciones_dobles_minimas > 0 && p < num_periodos - 1 && asignadas <= req.lecciones_totales - 2) {
                if (ind.matriz[d][p][req.clase_id] == -1 && ind.matriz[d][p+1][req.clase_id] == -1) {
                    ind.matriz[d][p][req.clase_id] = req.id;
                    ind.matriz[d][p+1][req.clase_id] = req.id;
                    asignadas += 2;
                    continue;
                }
            }
            if (ind.matriz[d][p][req.clase_id] == -1) {
                ind.matriz[d][p][req.clase_id] = req.id;
                asignadas++;
            }
            intentos++;
        }
    }
    
    for(int d=0; d<num_dias; d++){
        for(int p=0; p<num_periodos; p++){
            for(int c=0; c<num_clases; c++){
                if(ind.matriz[d][p][c] == -1) {
                    for(const auto& r : reqs){
                        if(r.clase_id == c){ ind.matriz[d][p][c] = r.id; break; }
                    }
                }
            }
        }
    }
    return ind;
}

Cromosoma cruzar(const Cromosoma& padre1, const Cromosoma& padre2, int num_dias, int num_periodos, int num_clases) {
    Cromosoma hijo(num_dias, num_periodos, num_clases);
    for (int c = 0; c < num_clases; ++c) {
        bool heredarDePadre1 = (rand() % 2 == 0); 
        for (int d = 0; d < num_dias; ++d) {
            for (int p = 0; p < num_periodos; ++p) {
                hijo.matriz[d][p][c] = heredarDePadre1 ? padre1.matriz[d][p][c] : padre2.matriz[d][p][c];
            }
        }
    }
    return hijo;
}

void mutar(Cromosoma& ind, int num_dias, int num_periodos, int num_clases) {
    int prob_mutacion = 20; 
    if ((rand() % 100) < prob_mutacion) {
        int clase_mutar = rand() % num_clases;
        int d1 = rand() % num_dias, p1 = rand() % num_periodos;
        int d2 = rand() % num_dias, p2 = rand() % num_periodos;
        swap(ind.matriz[d1][p1][clase_mutar], ind.matriz[d2][p2][clase_mutar]);
    }
}

Cromosoma ejecutarAlgoritmoEvolutivo(int num_dias, int num_periodos, int num_clases, int num_profesores, vector<Requerimiento>& reqs, vector<vector<vector<bool>>>& indisponibilidad) {
    int tamano_poblacion = 30;
    int generaciones = 200;
    vector<Cromosoma> poblacion;

    for (int i = 0; i < tamano_poblacion; ++i) {
        Cromosoma inicial = generarSolucionInicial(num_dias, num_periodos, num_clases, reqs);
        inicial.evaluarFitness(reqs, num_profesores, indisponibilidad);
        poblacion.push_back(inicial);
    }

    auto seleccionTorneo = [&]() {
        int k = 3; 
        int mejor_idx = rand() % tamano_poblacion;
        for(int i = 1; i < k; ++i) {
            int idx = rand() % tamano_poblacion;
            if(poblacion[idx].fitness < poblacion[mejor_idx].fitness) mejor_idx = idx;
        }
        return poblacion[mejor_idx];
    };

    for (int gen = 0; gen < generaciones; ++gen) {
        sort(poblacion.begin(), poblacion.end(), [](const Cromosoma& a, const Cromosoma& b) {
            return a.fitness < b.fitness;
        });

        vector<Cromosoma> nueva_poblacion;
        nueva_poblacion.push_back(poblacion[0]); 
        nueva_poblacion.push_back(poblacion[1]); 

        while (nueva_poblacion.size() < tamano_poblacion) {
            Cromosoma padre1 = seleccionTorneo(); 
            Cromosoma padre2 = seleccionTorneo(); 

            Cromosoma hijo = cruzar(padre1, padre2, num_dias, num_periodos, num_clases);
            mutar(hijo, num_dias, num_periodos, num_clases);
            
            hijo.evaluarFitness(reqs, num_profesores, indisponibilidad);
            nueva_poblacion.push_back(hijo);
        }
        poblacion = nueva_poblacion;
    }
    return poblacion[0];
}