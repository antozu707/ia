#ifndef EVOLUTIVO_H
#define EVOLUTIVO_H

#include <vector>
#include "cromosoma.h"
#include "req.h"

using namespace std;

Cromosoma generarSolucionInicial(int num_dias, int num_periodos, int num_clases, vector<Requerimiento> reqs);
Cromosoma cruzar(const Cromosoma& padre1, const Cromosoma& padre2, int num_dias, int num_periodos, int num_clases);
void mutar(Cromosoma& ind, int num_dias, int num_periodos, int num_clases);
Cromosoma ejecutarAlgoritmoEvolutivo(int num_dias, int num_periodos, int num_clases, int num_profesores, vector<Requerimiento>& reqs, vector<vector<vector<bool>>>& indisponibilidad);

#endif