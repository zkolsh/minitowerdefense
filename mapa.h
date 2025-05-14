#ifndef MAPA_H
#define MAPA_H

#include "nivel.h"
#include "stdlib.h"
#include "stdio.h"

#define limpiar "clear"
#define ANSI_RESET "\x1b[0m"
#define CELL_WIDTH 6

typedef enum { CAMINO, TORRE, VACIO, BLOQUEADO, ENEMIGO } TipoCasilla;

typedef struct {
    int ancho, alto, cant_torres;
    TipoCasilla **casillas;
    Coordenada *torres;
} Mapa;

Mapa *inicializar_mapa(int, int, int);

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos);

void liberar_mapa(Mapa *mapa);

void limpiar_pantalla();

#endif