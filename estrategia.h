#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "mapa.h"
#include "nivel.h"
#include "pila.h"
#include "cola.h"

typedef struct OpcionBacktracking {
    int daño_acumulado;
    int torres_colocadas;
    Coordenada comienzo_busqueda;
    Coordenada posicion;
} OpcionBacktracking;

typedef struct PosibleTorre {
    Coordenada posicion;
    int impacto;
} PosibleTorre;

typedef void (*DisposicionTorres)(Nivel*, Mapa*);

void disponer(Nivel* nivel, Mapa* mapa);

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa);

void disponer_custom(Nivel* nivel, Mapa* mapa);

#endif
