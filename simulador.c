#include "simulador.h"
#include "estrategia.h"

// static void limpiar_pantalla() {
//     system(limpiar);
// }

void inicializar_simulacion(const char* filename, Nivel* nivel, Mapa *mapa) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("No se pudo abrir el archivo de nivel");
        exit(1);
    }

    int ancho, alto, cant_enemigos, cant_torres, largo_camino;
    fscanf(f, "%d %d %d %d %d\n", &ancho, &alto, &cant_enemigos, &cant_torres, &largo_camino);

    *nivel = *inicializar_nivel(largo_camino, cant_enemigos);
    *mapa = *inicializar_mapa(ancho, alto, cant_torres);

    char fila[MAX_LINEA];
    int n_camino = 0;
    for (int i = 0; i < alto; i++) {
        fscanf(f, "%s", fila);
        n_camino += procesar_fila(fila, ancho, i, mapa->casillas, nivel->camino->posiciones, n_camino);
    }

    fclose(f);
}

void liberar_simulacion(Nivel *nivel, Mapa *mapa) {
    liberar_nivel(nivel);
    liberar_mapa(mapa);
}

void simular_nivel(Nivel *nivel, Mapa *mapa, Estrategia colocar_torres) {
    inicializar_turno(nivel, mapa, colocar_torres);
    
    int escapes = 0;
    for (int turno = 0; nivel->enemigos->cantidad_activos && !escapes; turno++) {
        // limpiar_pantalla();
        escapes += simular_turno(mapa, nivel);
        mostrar_mapa(mapa);
        sleep(1);
    }

    if (escapes)
        printf("\n¡%d enemigo(s) escaparon!\n", escapes);
}

int mostrar_menu(Estrategia estrategia_actual, char *ruta_nivel_actual) {
    int opcion = 3;

    printf("\n--- Menú del simulador ---\n");
    printf("1. Seleccionar estrategia (actual: %s)\n", (estrategia_actual == colocacion_basica) ? "Básica" : "Avanzada");
    printf("2. Seleccionar nivel (actual: %s)\n", ruta_nivel_actual);
    printf("3. Iniciar simulación\n");
    printf("0. Salir\n");

    printf("Seleccione una opción: ");
    scanf("%d", &opcion);

    return opcion;
}

int main() {
    srand(time(NULL));
    Estrategia estrategia_actual = colocacion_basica;
    char ruta_nivel_actual[] = "Levels/nivel01.txt";

    Nivel nivel;
    Mapa mapa;
    inicializar_simulacion(ruta_nivel_actual, &nivel, &mapa);

    int opcion = 1;
    int memoria_a_liberar = 1;
    while (opcion != 0) {
        opcion = mostrar_menu(estrategia_actual, ruta_nivel_actual);

        switch(opcion) {
            case 1:
                estrategia_actual = (estrategia_actual == colocacion_basica) ? estrategia_avanzada : colocacion_basica;
                break;
            case 2:
                printf("Ingrese la ruta al archivo de nivel: ");
                scanf("%s", ruta_nivel_actual);

                if(memoria_a_liberar)
                    liberar_simulacion(&nivel, &mapa);
                
                inicializar_simulacion(ruta_nivel_actual, &nivel, &mapa);
                mostrar_mapa(&mapa);
                break;
            case 3:
                simular_nivel(&nivel, &mapa, estrategia_actual);
                // TODO no funciona simular sucesivas veces
                break;
            default:
                // if(memoria_a_liberar)
                    // ver que culo liberar
                    // liberar_simulacion(&nivel, &mapa);
                break;
        }            
    }

    liberar_simulacion(&nivel, &mapa);
    return 0;
}