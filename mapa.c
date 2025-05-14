#include "mapa.h"
#include "turno.h"

void limpiar_pantalla() {
    // system(limpiar);
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

void liberar_mapa(Mapa *mapa) {
    for (int i = 0; i < mapa->alto; i++)
        free(mapa->casillas[i]);
    free(mapa->casillas);
    free(mapa->torres);
    free(mapa);
}

static int color_de_vida(int vida, int vida_max) {
    int green = 46, red = 196;

    if (vida <= 0) return red; // dead = dark
    if (vida >= vida_max) return green; // max = green

    return green + (vida_max - vida) * (red - green) / vida_max;
}

void imprimir_casilla(TipoCasilla tipo, int vida, int vida_max) {
    switch(tipo) {
        case CAMINO:
            printf(".     ");
            return;
        case TORRE:
            printf("T     ");
            return;
        case ENEMIGO:
            int color = color_de_vida(vida, vida_max);
            printf("\x1b[38;5;%dm%-6s\x1b[0m", color, "*");
            return;
        case BLOQUEADO:
        case VACIO:
            printf("      ");
            return;
    }
}

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos) {
    limpiar_pantalla();

    printf("╔");
    for (int i = 0; i < mapa->ancho; i++)
        printf("──────");
    printf("╗\n");

    for (int x = 0; x < mapa->alto; x++) {
        printf("║");
        for (int y = 0; y < mapa->ancho; y++) {
            TipoCasilla c = mapa->casillas[x][y];
            Coordenada coord;
            coord.x = x;
            coord.y = y;
            if (c == ENEMIGO) {
                int vida = buscarEnemigo(enemigos->posiciones, enemigos->cantidad, coord);
                imprimir_casilla(c, vida, VIDA_INICIAL);
            } else {
                imprimir_casilla(c, 0, VIDA_INICIAL);
            }
        }
        printf("║\n");
    }

    printf("╚");
    for (int i = 0; i < mapa->ancho; i++)
        printf("──────");
    printf("╝\n\n");

    printf("  .     = path\n");
    printf("  T     = tower\n");
    printf("  *     = enemy (color = health)\n");
    printf("  Enemigos vivos: %d\n\n", enemigos->cantidad_activos);
}
