#include "Archivos.h"
#include <iostream>
#include <fstream>
#include <sstream>

void leerTXT(string nombreArchivo, int& num_clases, int& num_profesores, int& num_dias, int& num_periodos, vector<Requerimiento>& reqs, vector<vector<vector<bool>>>& indisponibilidad) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "ERROR No se pudo abrir el archivo " << nombreArchivo << endl;
        return;
    }

    string linea;
    auto sgteLineaValida = [&]() {
        while (getline(archivo, linea)) {
            if (linea.empty()) continue; 
            return true;
        }
        return false;
    };

    int min_val;
    if(sgteLineaValida()) { stringstream ss(linea); ss >> min_val >> num_clases; num_clases++; }
    if(sgteLineaValida()) { stringstream ss(linea); ss >> min_val >> num_profesores; num_profesores++; }
    if(sgteLineaValida()) { stringstream ss(linea); ss >> min_val >> num_dias; num_dias++; }
    if(sgteLineaValida()) { stringstream ss(linea); ss >> min_val >> num_periodos; num_periodos++; }

    indisponibilidad.assign(num_profesores, vector<vector<bool>>(num_dias, vector<bool>(num_periodos, false)));

    int cant_reqs;
    if(sgteLineaValida()) { stringstream ss(linea); ss >> cant_reqs; }

    for (int i = 0; i < cant_reqs; ++i) {
        if(sgteLineaValida()) {
            stringstream ss(linea);
            Requerimiento req;
            req.id = i;
            ss >> req.clase_id >> req.profesor_id >> req.lecciones_totales >> req.max_por_dia >> req.lecciones_dobles_minimas;
            reqs.push_back(req);
        }
    }

    int cant_indisp;
    if(sgteLineaValida()) { stringstream ss(linea); ss >> cant_indisp; }

    for (int i = 0; i < cant_indisp; ++i) {
        if(sgteLineaValida()) {
            stringstream ss(linea);
            int profe, dia, periodo;
            ss >> profe >> dia >> periodo;
            indisponibilidad[profe][dia][periodo] = true;
        }
    }
    archivo.close();
}

void escribirArchivoSalida(string nombreArchivo, int seed, double tiempo, const Cromosoma& mejor, int num_dias, int num_periodos, int num_clases, const vector<Requerimiento>& reqs, int alfa, int beta) {
    string base_name = nombreArchivo;
    size_t pos = base_name.find(".txt");
    if(pos != string::npos) base_name.erase(pos);
    
    string nombre_salida = base_name + "_" + to_string(seed) + ".txt";
    ofstream out(nombre_salida);
    if(!out.is_open()) return;

    out << "Valor de la Función Objetivo: " << mejor.fitness << ", Tiempo de cómputo: " << tiempo << " segundos" << endl;
    out << "Cantidad total de ventanas horarias entre todos los profesores: " << mejor.total_ventanas << ", Ponderación asociada: " << alfa << endl;
    out << "Cantidad total de lecciones dobles no asignadas: " << mejor.total_dobles << ", Ponderación asociada: " << beta << endl;
    out << endl;
    out << "# Lista de Asignaciones (D x P x C) x 4" << endl;
    
    for (int d = 0; d < num_dias; ++d) {
        for (int p = 0; p < num_periodos; ++p) {
            for (int c = 0; c < num_clases; ++c) {
                int req_id = mejor.matriz[d][p][c];
                if (req_id != -1) {
                    int profe_id = reqs[req_id].profesor_id;
                    out << "Día" << d << ", Período" << p << ", Clase" << c << ", Profesor" << profe_id << endl;
                }
            }
        }
    }
    out.close();
}