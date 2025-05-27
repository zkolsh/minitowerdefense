#include "estrategia.h"

int alcance_torre(const Mapa* mapa, const Coordenada origen) {

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

static int** obtener_posibles_daños(const Mapa* mapa) {
    int** daños = malloc(sizeof(int*) * mapa->ancho);
    if (!daños) return NULL;

    for (size_t i = 0; i < mapa->ancho; i++) {
        daños[i] = malloc(mapa->alto * sizeof(int));

        if (!daños[i]) {
            for (size_t j = 0; j < i; j++) free(daños[j]);
            free(daños);
            return NULL;
        };

        for (size_t j = 0; j < mapa->alto; j++) {
            daños[i][j] = 0;
        };
    };

    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;

            daños[x][y] = alcance_torre(mapa, (Coordenada){x, y});
        };
    };

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

static int mejor_torre(const void* lhs, const void* rhs) {
    return ((PosibleTorre*)rhs)->impacto - ((PosibleTorre*)lhs)->impacto;
};

void disponer_custom(Nivel* nivel, Mapa* mapa) {
    assert(nivel);
    assert(mapa);
    int** daño = obtener_posibles_daños(mapa);
    assert(daño);

    size_t cantidad_torres_potenciales = 0;
    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] == VACIO) cantidad_torres_potenciales++;
        };
    };

    PosibleTorre* posibles_torres = malloc(sizeof(PosibleTorre) * cantidad_torres_potenciales);
    assert(posibles_torres);
    size_t indice_torre = 0;
    for (size_t x = 0; x < mapa->ancho; x++) {
        for (size_t y = 0; y < mapa->alto; y++) {
            if (mapa->casillas[x][y] != VACIO) continue;

            posibles_torres[indice_torre].posicion.x = x;
            posibles_torres[indice_torre].posicion.y = y;
            posibles_torres[indice_torre].impacto = daño[x][y];
            indice_torre++;
        };
    };

    for (size_t i = 0; i < mapa->ancho; i++) {
        free(daño[i]);
    };

    free(daño);

    FILE* f = fopen("debug.log", "w");

    for (size_t i = 0; i < cantidad_torres_potenciales; i++) {
        fprintf(f, "(%d, %d, %d)\n", posibles_torres[i].posicion.x, posibles_torres[i].posicion.y, posibles_torres[i].impacto);
    };

    fprintf(f, "------------------\n");

    qsort(posibles_torres, cantidad_torres_potenciales, sizeof(PosibleTorre), mejor_torre);

    for (size_t i = 0; i < cantidad_torres_potenciales; i++) {
        fprintf(f, "(%d, %d, %d)\n", posibles_torres[i].posicion.x, posibles_torres[i].posicion.y, posibles_torres[i].impacto);
    };

    fclose(f);

    const size_t torres_a_colocar = (mapa->cant_torres < cantidad_torres_potenciales)? mapa->cant_torres : cantidad_torres_potenciales;
    for (size_t i = 0; i < torres_a_colocar; i++) {
        const PosibleTorre* torre = &posibles_torres[i];
        colocar_torre(mapa, torre->posicion.x, torre->posicion.y, i);
    };

    free(posibles_torres);
};
