#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <string>
#include <vector>
#include "req.h"
#include "cromosoma.h"

using namespace std;

void leerTXT(string nombreArchivo, int& num_clases, int& num_profesores, int& num_dias, int& num_periodos, vector<Requerimiento>& reqs, vector<vector<vector<bool>>>& indisponibilidad);

void escribirArchivoSalida(string nombreArchivo, int seed, double tiempo, const Cromosoma& mejor, int num_dias, int num_periodos, int num_clases, const vector<Requerimiento>& reqs, int alfa, int beta);

#endif