#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef STACK_INITIAL_CAPACITY
#define STACK_INITIAL_CAPACITY 2
#endif //STACK_CAPACITY

#ifndef STACK_EXPOSE_INTERNALS
typedef struct Pila Pila;
#else
typedef struct Pila {
    size_t last;
    size_t capacity;
    void** data;
} Pila;
#endif //STACK_EXPOSE_INTERNALS

// Accepts a pointer to an element, and returns the new
typedef void* (*Transformer)(void* x);

// Function that destroys the given parameter
typedef void (*Destructor)(void* x);

Pila* pila_nueva();
bool pila_esta_vacia(Pila* input);
size_t pila_tamaño(Pila* input);
void* pila_tope(Pila* input); // Returns the top element from the stack, but does not pop it.
void pila_realloc(Pila* stack, size_t new_capacity);
void pila_apilar(Pila* stack, void* data); // Append a new element to the stack.
void pila_desapilar(Pila* input); // Pops the top element off the stack, but does not return it.
void pila_foreach(Pila* input, Transformer function); // Apply a function to each element on the stack.
void pila_liberar(Pila* input); // Destroy the stack without freeing any of its members.
void pila_destruir(Pila* input, Destructor destroy); // Destroy the stack and every element it contains.

