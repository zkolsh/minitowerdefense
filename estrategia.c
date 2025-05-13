#include "estrategia.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int posiciones_validas(Coordenada *validas, TipoCasilla **casillas, int ancho, int alto) {
    int indice = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[indice].x = i;
                validas[indice++].y = j;
            }
        }
    }

    return indice;
}

static void colocar_torre(Mapa *mapa, int x, int y) {
    mapa->casillas[x][y] = TORRE;
}

static int determinar_posicion_torre(int *casillas_ocupadas, int cant_validas) {
    int nueva_posicion = rand() % cant_validas;
    while(casillas_ocupadas[nueva_posicion])
        nueva_posicion = rand() % cant_validas;

    return nueva_posicion;
}


void colocacion_basica(Nivel* nivel, Mapa* mapa) {
    const int tamano_mapa = mapa->ancho * mapa->alto;
    Coordenada posiciones_validas_torre[tamano_mapa];
    int casillas_ocupadas[tamano_mapa];
    for(int i= 0; i < tamano_mapa; casillas_ocupadas[i++] = 0);

    int cant_validas = posiciones_validas(posiciones_validas_torre, mapa->casillas, mapa->ancho, mapa->alto);
 
    for (int colocadas = 0; colocadas < mapa->cant_torres; colocadas++) {
        int nueva_torre = determinar_posicion_torre(casillas_ocupadas, cant_validas);
        colocar_torre(mapa, posiciones_validas_torre[nueva_torre].x, posiciones_validas_torre[nueva_torre].y);
    }
}

void estrategia_avanzada(Nivel* nivel, Mapa* mapa) {
    /* A cargo de la/el estudiante */
    return;
}
