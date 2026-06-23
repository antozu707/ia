#include "cromosoma.h"

// Constructor que inicializa la matriz 3D con -1 (vacío)
Cromosoma::Cromosoma(int num_dias, int num_periodos, int num_clases) {
    matriz.assign(num_dias, vector<vector<int>>(num_periodos, vector<int>(num_clases, -1)));
    fitness = 0;
    total_ventanas = 0;
    total_dobles = 0;
}

void Cromosoma::evaluarFitness(const vector<Requerimiento>& reqs, int num_profesores, const vector<vector<vector<bool>>>& indisponibilidad) {
    int penalizacion = 0;
    this->total_ventanas = 0;
    this->total_dobles = 0;
    
    int num_dias = matriz.size();
    int num_periodos = matriz[0].size();
    int num_clases = matriz[0][0].size();
    
    int alfa = 50; // Peso ventanas
    int beta = 10; // Peso dobles

    // A. RESTRICCIONES DURAS
    for (int d = 0; d < num_dias; ++d) {
        vector<int> clases_por_req(reqs.size(), 0); 
        
        for (int p = 0; p < num_periodos; ++p) {
            vector<int> profes_en_periodo(num_profesores, 0);
            for (int c = 0; c < num_clases; ++c) {
                int req_id = matriz[d][p][c];
                if (req_id != -1) {
                    int profe_id = reqs[req_id].profesor_id;
                    profes_en_periodo[profe_id]++;
                    
                    //Choque de profesores (crítico)
                    if (profes_en_periodo[profe_id] > 1) penalizacion += 1000; 
                    
                    //Indisponibilidad docente (grave)
                    if (indisponibilidad[profe_id][d][p]) penalizacion += 1000;
                    
                    clases_por_req[req_id]++;
                } else {
                    //Ventanas de alumnos (crítico)
                    penalizacion += 1000;
                }
            }
        }

        // Límite máximo de clases por día para una asignatura (grave)
        for(size_t r = 0; r < reqs.size(); ++r) {
            if (clases_por_req[r] > reqs[r].max_por_dia) {
                penalizacion += 1000 * (clases_por_req[r] - reqs[r].max_por_dia);
            }
        }
    }

    // B. RESTRICCIONES BLANDAS

    // Ventanas horarias de profesores
    for (int d = 0; d < num_dias; ++d) {
        for (int profe_id = 0; profe_id < num_profesores; ++profe_id) {
            int primer_bloque = -1, ultimo_bloque = -1, bloques_ocupados = 0;
            for (int p = 0; p < num_periodos; ++p) {
                bool imparte = false;
                for (int c = 0; c < num_clases; ++c) {
                    int req_id = matriz[d][p][c];
                    if (req_id != -1 && reqs[req_id].profesor_id == profe_id) {
                        imparte = true; break;
                    }
                }
                if (imparte) {
                    if (primer_bloque == -1) primer_bloque = p;
                    ultimo_bloque = p;
                    bloques_ocupados++;
                }
            }
            if (primer_bloque != -1 && ultimo_bloque != -1) {
                int amplitud = ultimo_bloque - primer_bloque + 1;
                int ventanas = amplitud - bloques_ocupados;
                this->total_ventanas += ventanas;
                penalizacion += (ventanas * alfa); 
            }
        }
    }
    
    // Lecciones dobles faltantes
    for (const auto& req : reqs) {
        if (req.lecciones_dobles_minimas > 0) {
            int dobles_encontradas = 0;
            for (int d = 0; d < num_dias; ++d) {
                for (int p = 0; p < num_periodos - 1; ++p) {
                    if (matriz[d][p][req.clase_id] == req.id && matriz[d][p+1][req.clase_id] == req.id) {
                        dobles_encontradas++; p++; 
                    }
                }
            }
            if (dobles_encontradas < req.lecciones_dobles_minimas) {
                int faltantes = req.lecciones_dobles_minimas - dobles_encontradas;
                this->total_dobles += faltantes;
                penalizacion += (faltantes * beta);
            }
        }
    }
    this->fitness = penalizacion;
}