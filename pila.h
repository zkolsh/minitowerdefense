#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef PILA_CAPACIDAD_INICIAL
#define PILA_CAPACIDAD_INICIAL 2
#endif //PILA_CAPACIDAD_INICIAL

#ifndef PILA_VISIBILIDAD_INTERNA
typedef struct Pila Pila;
#else
typedef struct Pila {
    size_t ultimo;
    size_t capacidad;
    void** datos;
} Pila;
#endif //PILA_VISIBILIDAD_INTERNA

// Toma un puntero a un elemento y devuelve un puntero al elemento transformado.
typedef void* (*Transformador)(void* x);

// Función que destruye el parámetro dado.
typedef void (*Destructor)(void* x);

Pila* pila_nueva();
bool pila_esta_vacia(Pila* input);
size_t pila_tamaño(Pila* input);
void* pila_tope(Pila* input); // Devuelve el último elemento del stack, sin desapilarlo.
void pila_realloc(Pila* pila, size_t nueva_capacidad);
void pila_apilar(Pila* pila, void* dato); // Apilar el próximo elemento.
void pila_desapilar(Pila* input); // Desapila el último elemento pero no lo devuelve ni lo destruye.
void pila_foreach(Pila* input, Transformador funcion); // Aplica una función a cada elemento en la pila.
void pila_liberar(Pila* input); // Destruye la pila sin destruir los elementos que contiene.
void pila_destruir(Pila* input, Destructor destruir); // Destruye la pila y todos los elementos que contiene.

