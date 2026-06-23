#ifndef REQ_H
#define REQ_H

struct Requerimiento {
    int id;
    int clase_id;
    int profesor_id;
    int lecciones_totales;
    int max_por_dia;
    int lecciones_dobles_minimas;
};

#endif