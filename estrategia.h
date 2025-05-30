#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mapa.h"
#include "nivel.h"
#include "pila.h"

typedef struct PosibleTorre {
    Coordenada posicion;
    int impacto;
} PosibleTorre;

typedef void (*DisposicionTorres)(Nivel*, Mapa*);

void disponer(Nivel* nivel, Mapa* mapa);

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa);

void disponer_custom(Nivel* nivel, Mapa* mapa);

#endif
