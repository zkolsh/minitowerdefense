#ifdef STACK_IMPLEMENTATION
#undef STACK_IMPLEMENTATION
#endif //STACK_IMPLEMENTATION

#define STACK_EXPOSE_INTERNALS
#include "pila.h"

Pila* pila_nueva() {
    Pila* stack = malloc(sizeof(Pila));
    stack->last = 0;
    stack->capacity = STACK_INITIAL_CAPACITY;
    stack->data = (void**)malloc(sizeof(void*) * STACK_INITIAL_CAPACITY);
    return stack;
};

bool pila_esta_vacia(Pila* input) {
    assert(input);
    return input->last == 0;
};

size_t pila_tamaño(Pila* input) {
    assert(input);
    return input->last;
};

void* pila_tope(Pila* input) {
    assert(input);
    if (input->last == 0) {
        perror("ERROR: Stack was empty.\n");
        exit(EXIT_FAILURE);
    };

    return input->data[input->last - 1];
};

void pila_realloc(Pila* stack, size_t new_capacity) {
    assert(new_capacity >= 0);
    if (new_capacity == 0) new_capacity = STACK_INITIAL_CAPACITY;

    void** new_data = (void**)realloc((void*)stack->data, new_capacity * sizeof(*new_data));
    if (new_data == NULL) {
        pila_liberar(stack);
        perror("ERROR: Out of memory.\n");
        exit(EXIT_FAILURE);
    };

    stack->capacity = new_capacity;
    stack->data = new_data;
};

void pila_apilar(Pila* stack, void* data) {
    assert(stack);
    if (stack->last >= stack->capacity) {
        pila_realloc(stack, 2 * stack->capacity);
    };

    stack->data[stack->last++] = data;
};

void pila_desapilar(Pila* input) {
    assert(input);
    if (input->last <= 0) {
        perror("ERROR: Stack undeflow.\n");
        exit(EXIT_FAILURE);
    };

    input->last--;
};

void pila_foreach(Pila* input, Transformer function) {
    assert(input);
    for (size_t i = 0; i < input->last; i++) {
        input->data[i] = function(input->data[i]);
    };
};

void pila_liberar(Pila* input) {
    if (input == NULL) return;

    free((void*)input->data);
    free(input);
};

void pila_destruir(Pila* input, Destructor destroy) {
    if (input == NULL) return;

    for (size_t i = 0; i < input->last; i++) {
        destroy(input->data[i]);
    };

    pila_liberar(input);
};
