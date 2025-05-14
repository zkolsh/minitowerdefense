#ifndef TURNO_H
#define TURNO_H

#include "mapa.h"
#include "estrategia.h"

int simular_turno(Mapa *mapa, Nivel *nivel, Coordenada*, int);

void inicializar_turno(Nivel *nivel, Mapa *mapa, Estrategia estrategia);

#endif