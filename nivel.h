#ifndef NIVEL_H
#define NIVEL_H

#define VIDA_INICIAL 5
#define DISTANCIA_ATAQUE 3

#include "stdlib.h"
#include "stdio.h"

typedef struct {
    int x, y;
} Coordenada;

typedef struct {
    Coordenada *posiciones;
    int *vida, *activos;
    int cantidad, cantidad_activos;
} Enemigos;

typedef struct {
    Coordenada *posiciones;
    int largo_camino;
} Camino;

typedef struct {
    Enemigos *enemigos;
    Camino *camino;
} Nivel;

Nivel *inicializar_nivel(int largo_camino, int cant_enemigos);

void mostrar_nivel(Nivel *info);

void liberar_nivel(Nivel*);

#endif