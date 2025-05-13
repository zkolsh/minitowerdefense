#include "nivel.h"

static Camino *inicializar_camino(int largo_camino) {
    Camino *camino = malloc(sizeof(Camino));
    camino->posiciones = malloc(sizeof(Coordenada) * largo_camino);
    camino->largo_camino = largo_camino;

    return camino;
}

static void mostrar_camino(Camino *camino) {
    printf("Camino:\n");
    for (int i = 0; i < camino->largo_camino; i++)
        printf("x: %d  - y: %d\n", camino->posiciones[i].x, camino->posiciones[i].y);
}

static void liberar_camino(Camino *camino) {
    free(camino->posiciones);
    free(camino);
}

static Enemigos *inicializar_enemigos(int cant_enemigos) {
    Enemigos *enemigos = malloc(sizeof(Enemigos));

    enemigos->vida = malloc(sizeof(int) * cant_enemigos);
    enemigos->activos = malloc(sizeof(int) * cant_enemigos);
    enemigos->posiciones = malloc(sizeof(Coordenada) * cant_enemigos);

    enemigos->cantidad = cant_enemigos;
    enemigos->cantidad_activos = cant_enemigos;

    for (int i = 0; i < cant_enemigos; i++) {
        enemigos->posiciones[i].x = enemigos->posiciones[i].y = -1;
        enemigos->vida[i] = VIDA_INICIAL;
        enemigos->activos[i] = 1;
    }
    
    return enemigos;
}

static void mostrar_enemigos(Enemigos *enemigos) {
    printf("Cantidad enemigos: %d - Cantidad enemigos activos: %d\n",
        enemigos->cantidad, enemigos->cantidad_activos);
    for (int i = 0; i < enemigos->cantidad; i++) {
        printf("x: %d  - y: %d\n", enemigos->posiciones[i].x, enemigos->posiciones[i].y);
        printf("vida: %d - activo: %d\n", enemigos->vida[i], enemigos->activos[i]);
    }
}

static void liberar_enemigos(Enemigos* enemigos) {
    free(enemigos->vida);
    free(enemigos->activos);
    free(enemigos->posiciones);
    free(enemigos);
}

void mostrar_nivel(Nivel *nivel) {
    mostrar_camino(nivel->camino);
    mostrar_enemigos(nivel->enemigos);
}

Nivel *inicializar_nivel(int largo_camino, int cant_enemigos) {
    Nivel *nivel = malloc(sizeof(Nivel));
    nivel->camino = inicializar_camino(largo_camino);
    nivel->enemigos = inicializar_enemigos(cant_enemigos);
    return nivel;
}

void liberar_nivel(Nivel *nivel) {
    liberar_camino(nivel->camino);
    liberar_enemigos(nivel->enemigos);
    free(nivel);
}