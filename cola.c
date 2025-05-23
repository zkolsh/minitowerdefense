#include "cola.h"

static inline void reverse(size_t n, void** array);

Cola* cola_nueva() {
    Cola* queue = malloc(sizeof(Cola));
    assert(queue);
    queue->first = 0;
    queue->last = 0;
    queue->capacity = QUEUE_INITIAL_CAPACITY;
    queue->data = (void**)malloc(sizeof(void*) * QUEUE_INITIAL_CAPACITY);
    assert(queue->data);
    return queue;
};

bool cola_esta_vacia(Cola* input) {
    if (!input) return true;
    return input->first == input->last;
};

void* cola_frente(Cola* input) {
    assert(input);
    input->first %= input->capacity;
    return input->data[input->first];
};

static inline void reverse(size_t n, void** array) {
    assert(array);
    if (n < 2) return;

    size_t left = 0, right = n - 1;
    void* temp;

    while (left < right) {
        temp = array[left];
        array[left++] = array[right];
        array[right--] = temp;
    };
};

void cola_normalizar(Cola* input) {
    if (cola_esta_vacia(input)) return;
    if (input->first == 0) return;

    if (input->first < input->last) {
        reverse(input->last, input->data);
        reverse(input->last - input->first, input->data);
    } else {
        reverse(input->first, input->data);
        reverse(input->capacity - input->first, input->data + input->first);
        reverse(input->capacity, input->data);
    };

    input->last = (input->last + input->capacity - input->first) % input->capacity;
    input->first = 0;
    return;
};

void cola_realloc(Cola* queue, size_t new_capacity) {
    assert(queue);

    if (new_capacity == 0) new_capacity = QUEUE_INITIAL_CAPACITY;

    cola_normalizar(queue);
    if (new_capacity <= queue->last) {
        cola_liberar(queue);
        perror("FAIL: Queue smashing detected.\n");
        exit(EXIT_FAILURE);
    };

    void** new_data = (void**)realloc((void*)queue->data, sizeof(*queue->data) * new_capacity);
    if (!new_data) {
        cola_liberar(queue);
        perror("FATAL: Out of memory.\n");
        exit(EXIT_FAILURE);
    };

    queue->data = new_data;
    queue->capacity = new_capacity;
};

void cola_encolar(Cola* queue, void* data) {
    assert(queue);
    if ((queue->last + 1) % queue->capacity == queue->first) {
        cola_realloc(queue, queue->capacity * 2);
    };

    queue->last %= queue->capacity;
    queue->data[queue->last] = data;
    queue->last++;
};

void cola_desencolar(Cola* input) {
    assert(input);
    assert(!cola_esta_vacia(input));
    input->first = (1 + input->first) % input->capacity;
};

void cola_foreach(Cola* input, Transformer function) {
    if (input == NULL) return;
    const size_t size = (input->last + input->capacity - input->first) % input->capacity;
    for (size_t element = 0; element < size; element++) {
        const size_t i = (input->first + element) % input->capacity;
        input->data[i] = function(input->data[i]);
    };
};

void cola_destruir(Cola* input, Destructor destroy) {
    if (input == NULL) return;
    const size_t size = (input->last + input->capacity - input->first) % input->capacity;
    for (size_t element = 0; element < size; element++) {
        const size_t i = (input->first + element) % input->capacity;
        destroy(input->data[i]);
    };

    cola_liberar(input);
};

void cola_liberar(Cola* input) {
    if (input == NULL) return;
    free((void*)input->data);
    free(input);
};
