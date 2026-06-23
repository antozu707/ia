#ifndef CROMOSOMA_H
#define CROMOSOMA_H

#include <vector>
#include "req.h"

using namespace std;

class Cromosoma {
public:
    // Matriz 3D: [día][período][clase] = requerimiento_id
    vector<vector<vector<int>>> matriz;
    int fitness;
    int total_ventanas;
    int total_dobles;

    // Constructor
    Cromosoma(int num_dias, int num_periodos, int num_clases);

    // Calcula el costo (penalizaciones) del individuo
    void evaluarFitness(const vector<Requerimiento>& reqs, int num_profesores, const vector<vector<vector<bool>>>& indisponibilidad);
};

#endif