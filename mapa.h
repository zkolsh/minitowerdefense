#ifndef MAPA_H
#define MAPA_H

#include "nivel.h"
#include "stdlib.h"
#include "stdio.h"

#define limpiar "clear"
#define GREEN 46
#define RED 196

typedef enum { CAMINO, TORRE, VACIO, BLOQUEADO, ENEMIGO } TipoCasilla;

typedef struct {
    int ancho, alto, cant_torres, distancia_ataque;
    TipoCasilla **casillas;
    Coordenada *torres;
} Mapa;

Mapa *inicializar_mapa(int, int, int, int);

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos);

void liberar_mapa(Mapa *mapa);

void limpiar_pantalla();

#endif