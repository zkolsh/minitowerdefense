#include "estrategia.h"

static int alcance_torre(const Mapa* mapa, const Coordenada origen) {
    int alcance = 0;
    const int radio = mapa->distancia_ataque;

    for (int deltaX = -radio; deltaX <= radio; deltaX++) {
        for (int deltaY = -radio; deltaY <= radio; deltaY++) {
            const Coordenada nodo = {origen.x + deltaX, origen.y + deltaY};

            if (nodo.x < 0 || nodo.x >= mapa->alto || nodo.y < 0 || nodo.y >= mapa->ancho) {
                continue;
            };

            if (mapa->casillas[nodo.x][nodo.y] == CAMINO) {
                alcance++;
            };
        };
    };

    return alcance;
};

static PosibleTorre* obtener_posibles_daños(const Mapa* mapa, size_t* cantidad_torres_potenciales) {
    *cantidad_torres_potenciales = 0;

    for (size_t x = 0; x < mapa->alto; x++) {
        for (size_t y = 0; y < mapa->ancho; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;
            (*cantidad_torres_potenciales)++;
        };
    };

    PosibleTorre* torres = malloc(*cantidad_torres_potenciales * sizeof(PosibleTorre));
    assert(torres);

    size_t indice_torre = 0;
    for (size_t x = 0; x < mapa->alto; x++) {
        for (size_t y = 0; y < mapa->ancho; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;
            torres[indice_torre].posicion.x = x;
            torres[indice_torre].posicion.y = y;
            torres[indice_torre].impacto = alcance_torre(mapa, (Coordenada){x, y});
            indice_torre++;
        };
    };

    return torres;
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

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    size_t cantidad_torres_potenciales = 0;
    PosibleTorre* posibles_torres = obtener_posibles_daños(mapa, &cantidad_torres_potenciales);

    if (!cantidad_torres_potenciales) {
        free(posibles_torres);
        return;
    };

    size_t mejor_daño = 0;
    PosibleTorre* mejor_configuración = malloc(sizeof(PosibleTorre) * mapa->cant_torres);
    assert(mejor_configuración);

    // NOTE: Array indexado en 1!
    size_t* posibles_daños = malloc(sizeof(size_t) * (cantidad_torres_potenciales + 1));
    assert(posibles_daños);

    posibles_daños[0] = 0;
    for (size_t i = 0; i < cantidad_torres_potenciales; i++) {
        posibles_daños[i + 1] = posibles_daños[i] + posibles_torres[i].impacto;
    };

    PosibleTorre* configuración_actual = malloc(sizeof(PosibleTorre) * mapa->cant_torres);
    assert(configuración_actual);

    typedef struct TramaBacktracking {
        size_t torres_colocadas;
        size_t daño_actual;
        size_t indice_comienzo;
    } TramaBacktracking;

    Pila* pasos = pila_nueva();
    assert(pasos);

    TramaBacktracking* comienzo = malloc(sizeof(TramaBacktracking));
    assert(comienzo);
    comienzo->torres_colocadas = 0;
    comienzo->daño_actual = 0;
    comienzo->indice_comienzo = 0;
    pila_apilar(pasos, comienzo);

    while (!pila_esta_vacia(pasos)) {
        TramaBacktracking* estado = pila_tope(pasos);

        // heurística:
        const size_t posible_daño_restante
                = posibles_daños[cantidad_torres_potenciales]
                - posibles_daños[estado->indice_comienzo];

        if (estado->daño_actual + posible_daño_restante <= mejor_daño) {
            // este camino no llevará a ninguna estrategia mejor
            free(estado);
            pila_desapilar(pasos);
            continue;
        };

        // caso base:
        if (estado->torres_colocadas >= mapa->cant_torres) {
            if (estado->daño_actual > mejor_daño) {
                mejor_daño = estado->daño_actual;
                memcpy(mejor_configuración, configuración_actual, estado->torres_colocadas * sizeof(PosibleTorre));
            };

            free(estado);
            pila_desapilar(pasos);
            continue;
        };

        // caso recursivo:
        if (estado->indice_comienzo >= cantidad_torres_potenciales) {
            // descolocar torre, backtrack
            free(estado);
            pila_desapilar(pasos);
            continue;
        };

        const size_t indice_actual = estado->indice_comienzo;
        estado->indice_comienzo++; // La proxima vez que volvamos a este paso, vamos a considerar la proxima torre.

        // Dar un paso hacia adelante.
        TramaBacktracking* proximo = malloc(sizeof(TramaBacktracking));
        assert(proximo);
        proximo->torres_colocadas = estado->torres_colocadas + 1;
        proximo->daño_actual = estado->daño_actual + posibles_torres[indice_actual].impacto;
        proximo->indice_comienzo = indice_actual + 1;
        pila_apilar(pasos, proximo);

        configuración_actual[proximo->torres_colocadas - 1] = posibles_torres[indice_actual];
    };

    pila_destruir(pasos, free);

    for (size_t i = 0; i < mapa->cant_torres; i++) {
        colocar_torre(mapa, mejor_configuración[i].posicion.x, mejor_configuración[i].posicion.y, i);
    };

    free(mejor_configuración);
    free(configuración_actual);
    free(posibles_daños);
    free(posibles_torres);
};

static int mejor_torre(const void* lhs, const void* rhs) {
    return ((PosibleTorre*)rhs)->impacto - ((PosibleTorre*)lhs)->impacto;
};

void disponer_custom(Nivel* nivel, Mapa* mapa) {
    assert(mapa);

    size_t cantidad_torres_potenciales = 0;
    PosibleTorre* posibles_torres = obtener_posibles_daños(mapa, &cantidad_torres_potenciales);
    assert(posibles_torres);

    qsort(posibles_torres, cantidad_torres_potenciales, sizeof(PosibleTorre), mejor_torre);

    const size_t torres_a_colocar = (mapa->cant_torres < cantidad_torres_potenciales)? mapa->cant_torres : cantidad_torres_potenciales;
    for (size_t i = 0; i < torres_a_colocar; i++) {
        const PosibleTorre* torre = &posibles_torres[i];
        colocar_torre(mapa, torre->posicion.x, torre->posicion.y, i);
    };

    free(posibles_torres);
};
