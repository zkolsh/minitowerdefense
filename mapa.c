#include "mapa.h"

static char casilla_a_imprimible(TipoCasilla tipo) {
    switch(tipo) {
        case CAMINO:
            return '_';
        case TORRE:
            return 'T';
        case ENEMIGO:
            return '*';
        case BLOQUEADO:
        case VACIO:
        default:
            return '+';
    }
}

Mapa *inicializar_mapa(int ancho, int alto, int cant_torres) {
    Mapa *mapa = malloc(sizeof(Mapa));
    mapa->casillas = malloc(sizeof(TipoCasilla*) * alto);

    for (int x = 0; x < alto; x++)
        mapa->casillas[x] = malloc(sizeof(TipoCasilla) * ancho);

    mapa->cant_torres = cant_torres;
    mapa->torres = malloc(sizeof(Coordenada) * cant_torres);
    mapa->ancho = ancho;
    mapa->alto = alto;

    return mapa;
}

void mostrar_mapa(Mapa *mapa) {
    for (int x = 0; x < mapa->alto; x++) {
        for (int y = 0; y < mapa->ancho; y++)
            printf("%c ", casilla_a_imprimible(mapa->casillas[x][y]));
        printf("\n");
    }

    printf("\n\n");
}

void liberar_mapa(Mapa *mapa) {
    for (int i = 0; i < mapa->alto; i++)
        free(mapa->casillas[i]);
    free(mapa->casillas);
    free(mapa->torres);

    free(mapa);
}
