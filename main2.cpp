#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// ==========================================
// 1. ESTRUCTURAS DE DATOS
// ==========================================
struct Requerimiento {
    int id;
    int clase_id;
    int profesor_id;
    int lecciones_totales;
    int max_por_dia;
    int lecciones_dobles_minimas;
};

// ==========================================
// 2. LECTURA DEL ARCHIVO DE INSTANCIAS
// ==========================================
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
    cout << "Instancia cargada correctamente" << endl;
}

// ==========================================
// 3. REPRESENTACIÓN Y FUNCIÓN DE EVALUACIÓN
// ==========================================
class Cromosoma {
public:
    // Matriz 3D: [día][período][clase] = requerimiento_id
    vector<vector<vector<int>>> matriz;
    int fitness;
    int total_ventanas;
    int total_dobles;

    Cromosoma(int num_dias, int num_periodos, int num_clases) {
        matriz.assign(num_dias, vector<vector<int>>(num_periodos, vector<int>(num_clases, -1)));
        fitness = 0;
        total_ventanas = 0;
        total_dobles = 0;
    }

    void evaluarFitness(const vector<Requerimiento>& reqs, int num_profesores, const vector<vector<vector<bool>>>& indisponibilidad) {
        int penalizacion = 0;
        this->total_ventanas = 0;
        this->total_dobles = 0;
        
        int num_dias = matriz.size();
        int num_periodos = matriz[0].size();
        int num_clases = matriz[0][0].size();
        
        int alfa = 50; // Ponderación ventanas docentes
        int beta = 10; // Ponderación dobles faltantes

        // A. Restricciones Duras (+1000 pts)
        for (int d = 0; d < num_dias; ++d) {
            vector<int> clases_por_req(reqs.size(), 0); 
            
            for (int p = 0; p < num_periodos; ++p) {
                vector<int> profes_en_periodo(num_profesores, 0);
                
                for (int c = 0; c < num_clases; ++c) {
                    int req_id = matriz[d][p][c];
                    if (req_id != -1) {
                        int profe_id = reqs[req_id].profesor_id;
                        
                        profes_en_periodo[profe_id]++;
                        if (profes_en_periodo[profe_id] > 1) penalizacion += 1000; // Choque profe
                        if (indisponibilidad[profe_id][d][p]) penalizacion += 1000; // Indisponible
                        
                        clases_por_req[req_id]++;
                    } else {
                        penalizacion += 1000; // Ventana de alumnos
                    }
                }
            }
            for(size_t r = 0; r < reqs.size(); ++r) {
                if (clases_por_req[r] > reqs[r].max_por_dia) {
                    penalizacion += 1000 * (clases_por_req[r] - reqs[r].max_por_dia); // Exceso diario
                }
            }
        }

        // B. Restricciones Blandas
        for (int d = 0; d < num_dias; ++d) {
            for (int profe_id = 0; profe_id < num_profesores; ++profe_id) {
                int primer_bloque = -1, ultimo_bloque = -1, bloques_ocupados = 0;
                for (int p = 0; p < num_periodos; ++p) {
                    bool imparte_clase = false;
                    for (int c = 0; c < num_clases; ++c) {
                        int req_id = matriz[d][p][c];
                        if (req_id != -1 && reqs[req_id].profesor_id == profe_id) {
                            imparte_clase = true; break;
                        }
                    }
                    if (imparte_clase) {
                        if (primer_bloque == -1) primer_bloque = p;
                        ultimo_bloque = p;
                        bloques_ocupados++;
                    }
                }
                if (primer_bloque != -1 && ultimo_bloque != -1) {
                    int amplitud_jornada = ultimo_bloque - primer_bloque + 1;
                    int ventanas = amplitud_jornada - bloques_ocupados;
                    this->total_ventanas += ventanas;
                    penalizacion += (ventanas * alfa); 
                }
            }
        }
        
        for (const auto& req : reqs) {
            if (req.lecciones_dobles_minimas > 0) {
                int dobles_encontradas = 0;
                for (int d = 0; d < num_dias; ++d) {
                    for (int p = 0; p < num_periodos - 1; ++p) {
                        if (matriz[d][p][req.clase_id] == req.id && matriz[d][p+1][req.clase_id] == req.id) {
                            dobles_encontradas++;
                            p++; 
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
};

// ==========================================
// 4. GENERACIÓN DE SOLUCIÓN INICIAL
// ==========================================
Cromosoma generarSolucionInicial(int num_dias, int num_periodos, int num_clases, vector<Requerimiento> reqs) {
    Cromosoma ind(num_dias, num_periodos, num_clases);
    
    // Greedy
    sort(reqs.begin(), reqs.end(), [](const Requerimiento& a, const Requerimiento& b) {
        return a.lecciones_dobles_minimas > b.lecciones_dobles_minimas;
    });

    for (const auto& req : reqs) {
        int asignadas = 0, intentos = 0; 
        
        while (asignadas < req.lecciones_totales && intentos < 1000) {
            int d = rand() % num_dias;
            int p = rand() % num_periodos;
            
            // Intento de inserción doble consecutiva
            if (req.lecciones_dobles_minimas > 0 && p < num_periodos - 1 && asignadas <= req.lecciones_totales - 2) {
                if (ind.matriz[d][p][req.clase_id] == -1 && ind.matriz[d][p+1][req.clase_id] == -1) {
                    ind.matriz[d][p][req.clase_id] = req.id;
                    ind.matriz[d][p+1][req.clase_id] = req.id;
                    asignadas += 2;
                    continue;
                }
            }

            // Inserción simple aleatoria
            if (ind.matriz[d][p][req.clase_id] == -1) {
                ind.matriz[d][p][req.clase_id] = req.id;
                asignadas++;
            }
            intentos++;
        }
    }
    
    // Chequear espacios vacíos y rellenar 
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

// ==========================================
// 5. OPERADORES GENÉTICOS
// ==========================================
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

// ==========================================
// 6. MOTOR DEL ALGORITMO EVOLUTIVO
// ==========================================
Cromosoma ejecutarAlgoritmoEvolutivo(int num_dias, int num_periodos, int num_clases, int num_profesores, vector<Requerimiento>& reqs, vector<vector<vector<bool>>>& indisponibilidad) {
    int tamano_poblacion = 30;
    int generaciones = 200;
    vector<Cromosoma> poblacion;

    for (int i = 0; i < tamano_poblacion; ++i) {
        Cromosoma inicial = generarSolucionInicial(num_dias, num_periodos, num_clases, reqs);
        inicial.evaluarFitness(reqs, num_profesores, indisponibilidad);
        poblacion.push_back(inicial);
    }

    // Selección por torneo
    auto seleccionTorneo = [&]() {
        int k = 3; // participantes
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
        nueva_poblacion.push_back(poblacion[0]); // Elitismo 1
        nueva_poblacion.push_back(poblacion[1]); // Elitismo 2

        while (nueva_poblacion.size() < tamano_poblacion) {
            Cromosoma padre1 = seleccionTorneo(); 
            Cromosoma padre2 = seleccionTorneo(); 

            Cromosoma hijo = cruzar(padre1, padre2, num_dias, num_periodos, num_clases);
            mutar(hijo, num_dias, num_periodos, num_clases);
            
            hijo.evaluarFitness(reqs, num_profesores, indisponibilidad);
            nueva_poblacion.push_back(hijo);
        }
        poblacion = nueva_poblacion;
        
        if(gen % 20 == 0) cout << "Generacion " << gen << " - Mejor Fitness: " << poblacion[0].fitness << endl;
    }
    return poblacion[0];
};

// ==========================================
// 7. ARCHIVO DE SALIDA
// ==========================================
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

// ==========================================
// 8. BLOQUE PRINCIPAL (MAIN)
// ==========================================
int main() {
    int num_dias, num_periodos, num_clases, num_profesores;
    vector<Requerimiento> requerimientos;
    vector<vector<vector<bool>>> indisponibilidad;

    string archivo_instancia = "CL-CEASD-2008-V-A.txt"; 
    int semilla = 12345; 
    srand(semilla);

    leerTXT(archivo_instancia, num_clases, num_profesores, num_dias, num_periodos, requerimientos, indisponibilidad);

    if (!requerimientos.empty()) {
        auto start = chrono::high_resolution_clock::now();
        
        Cromosoma mejor_horario = ejecutarAlgoritmoEvolutivo(num_dias, num_periodos, num_clases, num_profesores, requerimientos, indisponibilidad);
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        
        escribirArchivoSalida(archivo_instancia, semilla, elapsed.count(), mejor_horario, num_dias, num_periodos, num_clases, requerimientos, 50, 10);
        cout << "Proceso terminado. Resultados en archivo txt." << endl;
    }
    return 0;
}