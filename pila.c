#define PILA_VISIBILIDAD_INTERNA
#include "pila.h"

Pila* pila_nueva() {
    Pila* pila = malloc(sizeof(Pila));
    assert(pila);
    pila->ultimo = 0;
    pila->capacidad = PILA_CAPACIDAD_INICIAL;
    pila->datos = (void**)malloc(sizeof(void*) * PILA_CAPACIDAD_INICIAL);
    return pila;
};

bool pila_esta_vacia(Pila* input) {
    assert(input);
    return input->ultimo == 0;
};

size_t pila_tamaño(Pila* input) {
    assert(input);
    return input->ultimo;
};

void* pila_tope(Pila* input) {
    assert(input);
    if (input->ultimo == 0) {
        perror("ERROR: La pila estaba vacía.\n");
        exit(EXIT_FAILURE);
    };

    return input->datos[input->ultimo - 1];
};

void pila_realloc(Pila* pila, size_t nueva_capacidad) {
    assert(nueva_capacidad >= 0);

    void** nuevos_datos = (void**)realloc((void*)pila->datos, nueva_capacidad * sizeof(*nuevos_datos));
    if (nuevos_datos == NULL) {
        pila_liberar(pila);
        perror("ERROR: No queda más memoria en el sistema.\n");
        exit(EXIT_FAILURE);
    };

    pila->capacidad = nueva_capacidad;
    pila->datos = nuevos_datos;
};

void pila_apilar(Pila* pila, void* dato) {
    assert(pila);
    if (pila->ultimo >= pila->capacidad) {
        pila_realloc(pila, 2 * pila->capacidad);
    };

    pila->datos[pila->ultimo++] = dato;
};

void pila_desapilar(Pila* input) {
    assert(input);
    if (input->ultimo <= 0) {
        perror("ERROR: Subrebalsado de pila.\n");
        exit(EXIT_FAILURE);
    };

    input->ultimo--;
};

void pila_foreach(Pila* input, Transformador funcion) {
    assert(input);
    for (size_t i = 0; i < input->ultimo; i++) {
        input->datos[i] = funcion(input->datos[i]);
    };
};

void pila_liberar(Pila* input) {
    if (input == NULL) return;

    free((void*)input->datos);
    free(input);
};

void pila_destruir(Pila* input, Destructor destruir) {
    if (input == NULL) return;

    for (size_t i = 0; i < input->ultimo; i++) {
        destruir(input->datos[i]);
    };

    pila_liberar(input);
};
