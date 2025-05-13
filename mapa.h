#ifndef MAPA_H
#define MAPA_H

#include "nivel.h"
#include "stdlib.h"
#include "stdio.h"

typedef enum { CAMINO, TORRE, VACIO, BLOQUEADO, ENEMIGO } TipoCasilla;

typedef struct {
    int ancho, alto, cant_torres;
    TipoCasilla **casillas;
    Coordenada *torres;
} Mapa;

Mapa *inicializar_mapa(int, int, int);

void mostrar_mapa(Mapa*);

void liberar_mapa(Mapa *mapa);

#endif