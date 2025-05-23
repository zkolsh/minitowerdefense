#include "estrategia.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int** obtener_posibles_daños(Mapa* mapa) {
    int** daños = malloc(sizeof(int*) * mapa->ancho);
    for (size_t i = 0; i < mapa->ancho; i++) {

        daños[i] = malloc(mapa->alto * sizeof(int));
        for (size_t j = 0; j < mapa->alto; j++) {
            daños[i][j] = 0;
        };
    };

    Cola* vecinos = cola_nueva();
    struct info_vecino {
        unsigned int codigo;
        Coordenada posicion;
        int distancia;
    };

    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;

            struct info_vecino* nodo = malloc(sizeof(struct info_vecino));
            nodo->codigo = y * mapa->ancho + x;
            nodo->posicion.x = x;
            nodo->posicion.y = y;
            nodo->distancia = 0;
            cola_encolar(vecinos, nodo);
        };
    };

    while (!cola_esta_vacia(vecinos)) {
        struct info_vecino* nodo = cola_frente(vecinos);
        cola_desencolar(vecinos);

        if (nodo->posicion.x < 0 || nodo->posicion.x >= mapa->ancho
         || nodo->posicion.y < 0 || nodo->posicion.y >= mapa->alto
         || nodo->distancia > mapa->distancia_ataque
         || daños[nodo->posicion.x][nodo->posicion.y] == -1
        ) {
             free(nodo);
             continue;
        };

        Coordenada origen;
        origen.x = nodo->codigo % mapa->ancho;
        origen.y = (nodo->codigo - origen.x) / mapa->ancho;

        if (nodo->distancia != 0) {
            daños[origen.x][origen.y]++;
            daños[nodo->posicion.x][nodo->posicion.y] = -1;
        };

        struct info_vecino* izq = malloc(sizeof(struct info_vecino));
        *izq = *nodo;
        izq->distancia++;
        izq->posicion.x--;
        cola_encolar(vecinos, izq);

        struct info_vecino* sup_izq = malloc(sizeof(struct info_vecino));
        *sup_izq = *nodo;
        sup_izq->distancia++;
        sup_izq->posicion.x--;
        sup_izq->posicion.y--;
        cola_encolar(vecinos, sup_izq);

        struct info_vecino* sup = malloc(sizeof(struct info_vecino));
        *sup = *nodo;
        sup->distancia++;
        sup->posicion.y--;
        cola_encolar(vecinos, sup);

        struct info_vecino* sup_der = malloc(sizeof(struct info_vecino));
        *sup_der = *nodo;
        sup_der->distancia++;
        sup_der->posicion.x++;
        sup_der->posicion.y--;
        cola_encolar(vecinos, sup_der);

        struct info_vecino* der = malloc(sizeof(struct info_vecino));
        *der = *nodo;
        der->distancia++;
        der->posicion.x++;
        cola_encolar(vecinos, der);

        struct info_vecino* inf_der = malloc(sizeof(struct info_vecino));
        *inf_der = *nodo;
        inf_der->distancia++;
        inf_der->posicion.x++;
        inf_der->posicion.y++;
        cola_encolar(vecinos, inf_der);

        struct info_vecino* inf = malloc(sizeof(struct info_vecino));
        *inf = *nodo;
        inf->distancia++;
        inf->posicion.y++;
        cola_encolar(vecinos, inf);

        struct info_vecino* inf_izq = malloc(sizeof(struct info_vecino));
        *inf_izq = *nodo;
        inf_izq->distancia++;
        inf_izq->posicion.x--;
        inf_izq->posicion.y++;
        cola_encolar(vecinos, inf_izq);

        free(nodo);
    };

    cola_destruir(vecinos, free);
    return daños;
};

static int posiciones_validas(Coordenada *validas, TipoCasilla **casillas, int alto, int ancho) {
    int cant_posiciones_validas = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[cant_posiciones_validas].x = i;
                validas[cant_posiciones_validas++].y = j;
            }
        }
    }

    return cant_posiciones_validas;
}

static void colocar_torre(Mapa *mapa, int x, int y, int nro_torre) {
    // actualizar torre
    mapa->torres[nro_torre].x = x;
    mapa->torres[nro_torre].y = y;

    // actualizar mapa
    mapa->casillas[x][y] = TORRE;
}

static int determinar_posicion_torre(int *casilla_elegida, int cant_validas) {
    int nueva_posicion = rand() % cant_validas;
    while(casilla_elegida[nueva_posicion])
        nueva_posicion = rand() % cant_validas;

    return nueva_posicion;
}


void disponer(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada posiciones_validas_torre[cantidad_casillas];
    int casilla_elegida[cantidad_casillas];
    for(int i = 0; i < cantidad_casillas; casilla_elegida[i++] = 0);

    int cant_validas = posiciones_validas(posiciones_validas_torre, mapa->casillas, mapa->alto, mapa->ancho);

    for (int colocadas = 0; colocadas < mapa->cant_torres; colocadas++) {
        int nueva_torre = determinar_posicion_torre(casilla_elegida, cant_validas);
        casilla_elegida[nueva_torre] = 1;
        int nueva_torre_x = posiciones_validas_torre[nueva_torre].x;
        int nueva_torre_y = posiciones_validas_torre[nueva_torre].y;
        colocar_torre(mapa, nueva_torre_x, nueva_torre_y, colocadas);
    };
};

OpcionBacktracking maximizar_daño(const Mapa* mapa, int** daño, const Coordenada comienzo) {
    OpcionBacktracking mejorResultado;
    mejorResultado.daño_acumulado = 0;
    mejorResultado.torres_colocadas = 0;

    for (int x = comienzo.x; x < mapa->ancho; x++) {
        for (int y = comienzo.y; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;

            OpcionBacktracking posible = maximizar_daño(mapa, daño, (Coordenada){x + 1, y});
            if (mapa->cant_torres - posible.torres_colocadas >= 1) {
                posible.daño_acumulado += daño[x][y];
                posible.torres_colocadas++;
            };

            if (posible.daño_acumulado > mejorResultado.daño_acumulado) {
                mejorResultado = posible;
            };
        };
    };

    return mejorResultado;
};

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    int** daño = obtener_posibles_daños(mapa);
    OpcionBacktracking mejorDisposición;
    mejorDisposición.daño_acumulado = 0;
    mejorDisposición.torres_colocadas = 0;

    for (int x = 0; x < mapa->ancho; x++) {
        for (int y = 0; y < mapa->alto; y++) {
            const OpcionBacktracking disp = maximizar_daño(mapa, daño, (Coordenada){x, y});
            if (disp.torres_colocadas > mapa->cant_torres) continue;

            if (disp.daño_acumulado > mejorDisposición.daño_acumulado) {
                mejorDisposición = disp;
            };
        };
    };

    for (size_t i = 0; i < mapa->ancho; i++) {
        free(daño[i]);
    };

    free(daño);
};

static int mejor_torre(const PosibleTorre* lhs, const PosibleTorre* rhs) {
    return 0;
};

void disponer_custom(Nivel* nivel, Mapa* mapa) {
    int** daño = obtener_posibles_daños(mapa);

    size_t cantidad_torres_potenciales = 0;
    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] == VACIO) cantidad_torres_potenciales++;
        };
    };

    PosibleTorre* posibles_torres = malloc(sizeof(PosibleTorre) * cantidad_torres_potenciales);
    size_t i = 0;
    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;

            posibles_torres[i].posicion.x = x;
            posibles_torres[i].posicion.y = y;
            posibles_torres[i].impacto = daño[x][y];
        };
    };

    for (size_t i = 0; i < mapa->ancho; i++) {
        free(daño[i]);
    };

    free(daño);
};
