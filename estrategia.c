#include "estrategia.h"

static int alcance_torre(const Mapa* mapa, const Coordenada origen) {
    int alcance = 0;
    const int radio = mapa->distancia_ataque;

    for (int deltaX = -radio; deltaX <= radio; deltaX++) {
        for (int deltaY = -radio; deltaY <= radio; deltaY++) {
            const Coordenada nodo = {origen.x + deltaX, origen.y + deltaY};

            if (nodo.x < 0 || nodo.x >= mapa->ancho || nodo.y < 0 || nodo.y >= mapa->alto) {
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

    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;
            (*cantidad_torres_potenciales)++;
        };
    };

    PosibleTorre* torres = malloc(*cantidad_torres_potenciales * sizeof(PosibleTorre));
    assert(torres);

    size_t indice_torre = 0;
    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
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

static void buscar_proxima_torre(ConfiguraciónBacktracking* config, const Mapa* mapa, PosibleTorre* posibles_torres, size_t cantidad_torres_potenciales, EstadoBacktracking* estado, size_t indice_comienzo) {
    if (estado->torres_colocadas >= mapa->cant_torres) {
        if (estado->daño_actual > config->daño_global) {
            // Nueva mejor disposición; guardar.
            memcpy(config->torres, estado->torres, sizeof(Coordenada) * estado->torres_colocadas);
            config->daño_global = estado->daño_actual;
            config->torres_colocadas = estado->torres_colocadas;
        };

        return;
    };

    for (size_t i = indice_comienzo; i < cantidad_torres_potenciales; i++) {
        const size_t x = posibles_torres[i].posicion.x;
        const size_t y = posibles_torres[i].posicion.y;

        if (mapa->casillas[x][y] != VACIO) continue;

        // colocar torre
        estado->torres[estado->torres_colocadas++] = (Coordenada){x, y};
        estado->daño_actual += posibles_torres[i].impacto;

        buscar_proxima_torre(config, mapa, posibles_torres, cantidad_torres_potenciales, estado, i + 1);

        // descolocar torre
        estado->torres_colocadas--;
        estado->daño_actual -= posibles_torres[i].impacto;
    };
};

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    size_t cantidad_torres_potenciales = 0;
    PosibleTorre* posibles_torres = obtener_posibles_daños(mapa, &cantidad_torres_potenciales);

    ConfiguraciónBacktracking config;
    config.daño_global = 0;
    config.torres_colocadas = 0;
    config.torres = malloc(sizeof(Coordenada) * mapa->cant_torres);
    assert(config.torres);

    EstadoBacktracking comienzo;
    comienzo.daño_actual = 0;
    comienzo.torres_colocadas = 0;
    comienzo.torres = malloc(sizeof(Coordenada) * mapa->cant_torres);
    assert(comienzo.torres);

    buscar_proxima_torre(&config, mapa, posibles_torres, cantidad_torres_potenciales, &comienzo, 0);

    free(comienzo.torres);

    for (size_t i = 0; i < config.torres_colocadas; i++) {
        colocar_torre(mapa, config.torres[i].x, config.torres[i].y, i);
    };

    free(config.torres);
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
