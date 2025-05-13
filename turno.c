#include "turno.h"

static void mostrar_enemigos(Coordenada *enemigos, int cant_enemigos) {
    for (int i = 0; i < cant_enemigos ; i++)
        printf("%d - %d\n", enemigos[i].x, enemigos[i].y);
}

static void desplazar_enemigos(Coordenada *enemigos, int cant_enemigos) {
    for (int i = cant_enemigos - 1; i > 0; i--)
        enemigos[i] = enemigos[i - 1];
}

static int camino(Mapa *mapa, Coordenada *lider, int limite, int dx, int dy) {
    if(dx != 0 && (lider->x + dx < 0 || lider->x + dx >= limite))
        return 0;
    else if (lider->y + dy < 0 || lider->y + dy >= limite)
        return 0;

    int casilla = mapa->casillas[lider->y + dy][lider->x + dx];
    return casilla == CAMINO;
}
static int camino_derecha(Mapa *mapa, Coordenada *lider, int ancho) {
    return camino(mapa, lider, ancho, 1, 0);
}
static int camino_abajo(Mapa *mapa, Coordenada *lider, int alto) {
    return camino(mapa, lider, alto, 0, 1);
}
    
static int decidir_nueva_posicion(Coordenada *enemigos, Mapa *mapa) {
    Coordenada *lider = &enemigos[0];

    if (lider->x == -1 && lider->y == -1)
        return 0;

    int ancho = mapa->ancho, alto = mapa->alto;
    if (lider->x == ancho - 1 && lider->y == alto - 1) {
        lider->x = lider->y = -1;
        return 1;
    }

    int direccion = rand() % 2; // 0 = abajo, 1 = derecha
    int hay_abajo = camino_abajo(mapa, lider, alto);
    int hay_derecha = camino_derecha(mapa, lider, ancho);

    if (!hay_abajo || (hay_derecha && direccion)) { // Derecha
        // printf("%s\n", !hay_abajo ? "Derecha (forzada)" : "Derecha");
        lider->x++;
        return 0;
    }
    
    // Abajo
    // printf("%s\n", !hay_derecha ? "Abajo (forzada)" : "Abajo");
    lider->y++;
    return 0;
}

static void actualizar_activos(Nivel *nivel, Mapa *mapa) {
    int escapes = 0;
    
    for (int i = 0; i < nivel->enemigos->cantidad ; i++)
        if (nivel->enemigos->posiciones[i].x >= mapa->ancho ||
            nivel->enemigos->posiciones[i].y >= mapa->alto) {
            nivel->enemigos->activos[i] = 0;
            escapes++;
        }

    nivel->enemigos->cantidad_activos = nivel->enemigos->cantidad - escapes;
}

static int enemigo_en_mapa(Coordenada enemigo, int ancho, int alto) {
    return enemigo.x >= 0 && enemigo.x < ancho &&
           enemigo.y >= 0 && enemigo.y < alto;
}

static void actualizar_mapa(Nivel *nivel, Mapa *mapa) {
    // Actualizar camino
    for (int i = 0; i < nivel->camino->largo_camino ; i++)
        mapa->casillas[nivel->camino->posiciones[i].y][nivel->camino->posiciones[i].x] = CAMINO;

    // Actualizar enemigos
    for (int i = 0; i < nivel->enemigos->cantidad ; i++)
        if(enemigo_en_mapa(nivel->enemigos->posiciones[i], mapa->ancho, mapa->alto))
            mapa->casillas[nivel->enemigos->posiciones[i].y][nivel->enemigos->posiciones[i].x] = ENEMIGO;
}

static int area_ataque(int distancia) {
    return 4 * distancia * (distancia + 1);
    // int cant_celdas = 0;
    // for (int i = 1; i <= distancia; i++)
    //     cant_celdas += 8 * i;
    
    // return cant_celdas;
}

static int calcular_posiciones(Coordenada posicion_torre, Coordenada *posiciones_ataque, int ancho, int alto) {
    int cant_posiciones_validas = 0;

    for (int dx = -DISTANCIA_ATAQUE; dx <= DISTANCIA_ATAQUE; dx++) {
        for (int dy = -DISTANCIA_ATAQUE; dy <= DISTANCIA_ATAQUE; dy++) {
            int nuevo_x = posicion_torre.x + dx;
            int nuevo_y = posicion_torre.y + dy;
            // printf("Calcular posiciones: %d - %d - %d  - %d\n", dx, dy, nuevo_x, nuevo_y);

            if (dx == 0 && dy == 0) continue;
            if (nuevo_x < 0 || nuevo_y < 0) continue;
            if (nuevo_x >= ancho || nuevo_y >= alto) continue;

            posiciones_ataque[cant_posiciones_validas].x = nuevo_x;
            posiciones_ataque[cant_posiciones_validas++].y = nuevo_y;
        }
    }

    return cant_posiciones_validas;
}

static int es_enemigo(TipoCasilla t) {
    return t == ENEMIGO;
}

static int buscarEnemigo(Coordenada *posiciones_enemigos, int cant_enemigos, Coordenada ataque) {
    int encontrado = 0, posicion = -1;

    for (int i = 0; i < cant_enemigos && !encontrado; i++){
        if(posiciones_enemigos[i].x == ataque.x &&
           posiciones_enemigos[i].y == ataque.y) {
            encontrado = 1;
            posicion = i;   
        }
    }
    
    return posicion;
}


static void disminuir_vidas(Nivel *nivel, Mapa *mapa, Coordenada *posiciones_ataque, int nro_ataques) {
    int nro_enemigo = 0, enemigo = 0;
    
    for (int i = 0; i < nro_ataques; i++) {
        enemigo = es_enemigo(mapa->casillas[posiciones_ataque[i].x][posiciones_ataque[i].y]);
        if(!enemigo) continue;

        nro_enemigo = buscarEnemigo(nivel->enemigos->posiciones, nivel->enemigos->cantidad, posiciones_ataque[i]);
        nivel->enemigos->vida[nro_enemigo]--;
        if(nivel->enemigos->vida[nro_enemigo] == 0) {
            nivel->enemigos->cantidad_activos--;
            nivel->enemigos->activos[nro_enemigo] = 0;
        }
    }
}

int simular_turno(Mapa *mapa, Nivel *nivel) {
    // atacan las torres
    int nro_ataques = area_ataque(DISTANCIA_ATAQUE);
    Coordenada posiciones_ataque[nro_ataques];
    printf("Ataques %d\n", nro_ataques);

    for (int i = 0; i < mapa->cant_torres; i++) {
        int ataques_efectivos = calcular_posiciones(mapa->torres[i], posiciones_ataque, mapa->ancho, mapa->alto);
        disminuir_vidas(nivel, mapa, posiciones_ataque, ataques_efectivos);
    }
    
    // avanza el enemigo a paso redoblado
    desplazar_enemigos(nivel->enemigos->posiciones, nivel->enemigos->cantidad);
    int escapes = decidir_nueva_posicion(nivel->enemigos->posiciones, mapa);
    actualizar_activos(nivel, mapa);
    actualizar_mapa(nivel, mapa); // TODO mostramos solo los activos?

    // TODO Actualizar escapes
    return escapes;
}

void inicializar_turno(Nivel *nivel, Mapa *mapa, Estrategia colocar_torres) {
    colocar_torres(nivel, mapa);
    
    // posicion inicial
    nivel->enemigos->posiciones[0].x = 0;
    nivel->enemigos->posiciones[0].y = 0;

    actualizar_activos(nivel, mapa);
    actualizar_mapa(nivel, mapa);
}