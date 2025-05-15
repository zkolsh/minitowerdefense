#include "mapa.h"
#include "turno.h"

void limpiar_pantalla() {
    system(limpiar);
}

Mapa *inicializar_mapa(int ancho, int alto, int cant_torres, int distancia_ataque) {
    Mapa *mapa = malloc(sizeof(Mapa));
    mapa->casillas = malloc(sizeof(TipoCasilla*) * alto);

    for (int x = 0; x < alto; x++)
        mapa->casillas[x] = malloc(sizeof(TipoCasilla) * ancho);

    mapa->cant_torres = cant_torres;
    mapa->torres = malloc(sizeof(Coordenada) * cant_torres);
    mapa->ancho = ancho;
    mapa->alto = alto;
    mapa->distancia_ataque = distancia_ataque;

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
    if (vida <= 0) return RED;
    if (vida >= vida_max) return GREEN;

    return GREEN + (vida_max - vida) * (RED - GREEN) / vida_max;
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

static void imprimir_borde_horizontal(int ancho, int top) {
    top ? printf("╔") : printf("╚");
    for (int i = 0; i < ancho; i++)
        printf("══════");
    top ? printf("╗\n") : printf("╝");
}

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos) {
    limpiar_pantalla();

    imprimir_borde_horizontal(mapa->ancho, 1);

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

    imprimir_borde_horizontal(mapa->ancho, 0);

    printf("\n");
    printf("  .     = path\n");
    printf("  T     = tower\n");
    printf("  *     = enemy (color = health)\n");
    printf("  Enemigos vivos: %d\n\n", enemigos->cantidad_activos);
}
