#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef QUEUE_INITIAL_CAPACITY
#define QUEUE_INITIAL_CAPACITY 4
#endif //QUEUE_INITIAL_CAPACITY

typedef struct Cola {
    void** data;
    size_t capacity;
    size_t first;
    size_t last;
} Cola;

// Accepts a pointer to an element, and returns a pointer to the new one
typedef void* (*Transformer)(void* x);

// Function that destroys the given parameter
typedef void (*Destructor)(void* x);

Cola* cola_nueva();
bool cola_esta_vacia(Cola* input);
void* cola_frente(Cola* input);
void cola_normalizar(Cola* input);
void cola_realloc(Cola* queue, size_t new_capacity);
void cola_encolar(Cola* queue, void* data);
void cola_desencolar(Cola* input);
void cola_foreach(Cola* input, Transformer function);
void cola_destruir(Cola* input, Destructor destroy);
void cola_liberar(Cola* input);

